/**
  A thin header-only C++ layer to read and write HDF5 files.
  Inspired by h5py (Python) and HDF5.jl (Julia)

  Author: Elias Kuthe
  Copyright: 2018 MIT 

  Documentation:
     https://support.hdfgroup.org/HDF5/doc/RM/RM_H5Front.html
  Compression
     https://support.hdfgroup.org/HDF5/Tutor/compress.html
  Source Code:
     https://portal.hdfgroup.org/display/support/Downloads

  TODO: Write attributes.
    https://github.com/mortenpi/hdf5/blob/master/hl/src/H5LT.c
    Distinguish between numerical and string attributes.
*/
#pragma once
#include <iostream>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <hdf5.h>
#include "hdf5hl.hpp"


/**
   HDF5 file (and utility static methods).

   Whenever an error occurs, it throws a std::runtime_error.
*/
class HDF5
{
public:
    /**
       As long as an instance of this class exists, there won't be any HDF5
       error/warning traces (e.g. when opening a non-existing file).

       See: https://support.hdfgroup.org/HDF5/doc/H5.user/Errors.html
    */
    struct ShutUp
    {
        herr_t (*old_func)(hid_t, void*);   // old handler
        void *old_client_data = nullptr;    // old data

        ShutUp() {  /* (1) save old handler, (2) replace by NULL */
            H5Eget_auto2(H5E_DEFAULT, &old_func, &old_client_data);  // (1)
            H5Eset_auto2(H5E_DEFAULT, NULL, NULL);                   // (2)
        }

        ~ShutUp() {  /* Restore previous error handler */
            H5Eset_auto2(H5E_DEFAULT, old_func, old_client_data);
        }
    };


    /** Open or create a file. Valid `mode`s are "r", "r+" and "w". */
    HDF5(const char *fname, const char *mode = "r", int compress = 3);
 
   ~HDF5();

    /** Check whether there exists, from the current location, a group
       or dataset named `data_name` */
    bool has(const char *data_name);

    /** Number of dimensions, i.e. rank of a dataset */
    int ndims(const char *data_name);

    /** Dimensions of an hdf5 array */
    typedef std::vector<hsize_t> Dims;

    /** Dimensions of a dataset */
    Dims dimensions(const char *data_name);
    void dimensions(const char *data_name, Dims *dims, H5T_class_t *c = nullptr);

    /** Number of elements in a dataset, i.e. product of dimensions */
    size_t size(const char *data_name);
    static size_t size(const Dims &dims);

    /** Read the content of a dataset */
    template<typename T>
    std::vector<T> read(const char *data_name, Dims *dims = nullptr);

    /** Return the value of an existing attribute */
    template<typename T>
    T attr(const char *attr_name, const char *obj_name = "/");

    /** Write into a new dataset, i.e. data_name must not exist */
    template<typename T>
    void write(const char *data_name, const std::vector<T> &data,
               Dims *dims = nullptr);

    /** Install a compression filter for writing; assert 0 <= c <= 9. */
    void set_compression(int c);

    /** Overwrite (if exists) */
    template<typename T>
    void owrite(const char *data_name, const std::vector<T> &data,
                Dims *dims = nullptr);

    /** Disable libhdf5 warnings/errors */
    static void shutup();

    /** Get (g = "") set (len(g) > 0)  and, if not exists,
        create the current group */
    std::string group(const char *g = "");
    std::string group(const std::string &g) {  return group(g.c_str()); }

    /** Return the HDF5 liberary version */
    static std::string libversion();

private:
    hid_t file_id, group_id;

    unsigned hm = H5F_ACC_EXCL;     /// file flags: in read-only mode?
    bool read_only() const { return hm == (hid_t)H5F_ACC_RDONLY; }

    hid_t status = 0;               /// error handling: status < 0?
    void check_error(const char *msg);
    void check_error(const std::string s) { check_error(s.c_str()); }

    int compress = 0;               /// compression level: 0 <= compress < 10
    hid_t cid = H5P_DEFAULT;        /// compression filter
    void close_compression_filter();

    hid_t find(const char *data_name);  /// find loc_id of data_name, or -1
};


HDF5::HDF5(const char *fname, const char *mode, int compress)
    : file_id(0)
{
    // https://github.com/h5py/h5py/blob/master/h5py/_hl/files.py#L95
    if (!strcmp(mode, "r")) {
        hm = H5F_ACC_RDONLY;
    } else if (!strcmp(mode, "r+")) {
        hm = H5F_ACC_RDWR;
    } else if (!strcmp(mode, "w")) {
        hm = H5F_ACC_TRUNC;
        ShutUp _;
        file_id = H5Fcreate(fname,
                            hm,
                            H5P_DEFAULT,
                            H5P_DEFAULT);
    } else
        std::runtime_error(std::string("Unknown mode: '") + mode + "'");

    if (file_id == 0) {
        ShutUp _;
        file_id = H5Fopen(fname, hm, H5P_DEFAULT);
    }
    if (file_id < 0)
        throw std::runtime_error(std::string("Could not open '") + fname +
                                 "'; returned " + std::to_string(file_id));
    group_id = file_id;
    if (compress > 0) {
        set_compression(compress);
    }
}


HDF5::~HDF5()
{
    close_compression_filter();
    if (group_id != file_id && group_id >= 0) {
        status = H5Gclose(group_id);
        group_id = -1;
        check_error("~HDF5::H5Gclose");
    }
    if (file_id >= 0) {
        const auto err = H5Fclose(file_id);
        if (err != 0)
          std::cerr << (std::string("HDF5: Error while closing ") +
                        std::to_string(err));
    } else {
        // throw std::runtime_error("Should not happen");
    }
}


void
HDF5::close_compression_filter()
{
    if (cid != H5P_DEFAULT) {
        status = H5Pclose(cid);
    }
}


std::string
_group_name(hid_t g)
{
    const size_t n = H5Iget_name(g, nullptr, 0);
    if (n == 0) return "/";
    std::string s (n, '\0');
    H5Iget_name(g, &s.front(), n+1);
    return s;
}


inline bool
_h5exists(hid_t id, const char *name)
{
    // https://support.hdfgroup.org/HDF5/doc/RM/RM_H5L.html#Link-Exists
    const auto status = H5Lexists(id, name, H5P_DEFAULT);
    if (status < 0)
        throw std::runtime_error(std::string("_h5exists(...") + name + ")");

    if (status > 0)
        return true;
    return false;
}


std::string
HDF5::group(const char *g)
{
    std::string owner (g);
    char *grp = &owner.front();
    char *part = strtok(grp, "/");
    while (part != 0) {
        if (_h5exists(group_id, part)) {
            status = H5Gopen2(group_id, part, H5P_DEFAULT);
            check_error("H5Gopen2");
        } else {
            if (read_only())
                throw std::runtime_error(std::string("Cannot create group ") + g);
            status = H5Gcreate2(group_id, part,
                                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            check_error("H5Gcreate2");
        }
        hid_t next_group = status;
        if (group_id != file_id) {
            status = H5Gclose(group_id);
            check_error("group()::H5Gclose");
        }
        group_id = next_group;
        part = strtok(nullptr, "/");
    }
    return _group_name(group_id);
}


hid_t
HDF5::find(const char *data_name)
{
   // ShutUp _;
    std::string owner (data_name);
    char *s = &owner.front();
    hid_t loc = data_name[0] == '/' ? file_id : group_id;
    char *part = strtok(s, "/");
    while (part != nullptr) {
        const auto type = H5Iget_type(loc);
        if ((type != H5I_GROUP && type != H5I_FILE) || !_h5exists(loc, part))
            break;
        status = H5Oopen(loc, part, H5P_DEFAULT);
        check_error("find::H5Open");
        hid_t next_loc = status;
        if (loc != group_id && loc != file_id) {
            status = H5Gclose(loc);
            check_error("find::H5Gclose");
        }
        loc = next_loc;
        part = strtok(nullptr, "/");
    }
    if (part != nullptr) {
        if (_h5exists(loc, part)) {
            status = H5Oopen(loc, part, H5P_DEFAULT);
            check_error("find::H5Open, 2");
            loc = status;
            return loc;
        }
        auto confirm = _h5exists(loc, part);
        if (loc != group_id && loc != file_id) {
            status = H5Oclose(loc);
            check_error("find::H5Oclose");
        }
        return -1;
    }
    return loc;
}


bool
HDF5::has(const char *data_name)
{
    hid_t loc = find(data_name);
    if (loc != -1) {
        if (loc != group_id && loc != file_id) {
            status = H5Oclose(loc);
            check_error("has()::H5Oclose");
        }
        return true;
    }
    return false;
}


void
HDF5::set_compression(int c)
{
    if (0 <= c && c < 10) {
        if (c != compress) {
            compress = c;
            close_compression_filter();
            cid = H5Pcreate(H5P_DATASET_CREATE);
            // check_error("H5Pcreate");    // no effect
        }
    } else {
        throw std::range_error(
            std::string("compress must be in [0..9]"));
    }
}


// https://support.hdfgroup.org/HDF5/doc/H5.user/Datatypes.html
template<typename T> hid_t h5t();
template<> hid_t h5t<char>()            { return H5T_NATIVE_CHAR; }
template<> hid_t h5t<short>()           { return H5T_NATIVE_SHORT; }
template<> hid_t h5t<int>()             { return H5T_NATIVE_INT; }
template<> hid_t h5t<int64_t>()         { return H5T_NATIVE_INT64; }
template<> hid_t h5t<unsigned char>()   { return H5T_NATIVE_UCHAR; }
template<> hid_t h5t<unsigned short>()  { return H5T_NATIVE_SHORT; }
template<> hid_t h5t<unsigned>()        { return H5T_NATIVE_UINT; }
template<> hid_t h5t<uint64_t>()        { return H5T_NATIVE_UINT64; }
template<> hid_t h5t<float>()           { return H5T_NATIVE_FLOAT; }
template<> hid_t h5t<double>()          { return H5T_NATIVE_DOUBLE; }
// template<> hid_t h5t<long>()            { return H5T_NATIVE_LONG; }


template<typename T>
std::vector<T>
HDF5::read(const char *data_name, Dims *dims)
{
    if (!has(data_name))
        throw std::runtime_error(std::string("Does not exists \"") +
                                 data_name + "\"");

    Dims _dims;
    Dims *dims_ = &_dims;
    H5T_class_t c;

    if (dims != nullptr)
        dims_ = dims;
    // printf("dimensions(%s)\n", data_name);
    dimensions(data_name, dims_, &c);
    std::vector<T> buf (size(*dims_));
    H5LTread_dataset(file_id, data_name, h5t<T>(), buf.data());
    check_error("H5LTread_dataset");
    return buf;
}


int
HDF5::ndims(const char *data_name)
{
    // printf("data_name --> %s\n", data_name);
    int ndims;
    status = H5LTget_dataset_ndims(file_id, data_name, &ndims);
    check_error("H5LTget_dataset_ndims");
    return ndims;
}


void
HDF5::check_error(const char *msg)
{
    if (status < 0)
        throw std::runtime_error(std::string("Error: ") + msg);
}


HDF5::Dims
HDF5::dimensions(const char *data_name)
{
    Dims dims;
    dimensions(data_name, &dims);
    return dims;
}


void
HDF5::dimensions(const char *data_name, Dims *dims, H5T_class_t *c)
{
    if (!_h5exists(group_id, data_name))
        throw std::runtime_error(std::string("dimensions: '") +
                                 data_name + "' does not exist");
    if (data_name[0] == '\0')
        throw std::runtime_error("dimensions: Internal error");
    dims->resize(ndims(data_name));
    status = _get_dataset_info(group_id, data_name,
                               dims->data(), c, NULL);
    check_error(std::string("H5LTget_dataset_info: '") +
                data_name + "'");
}


size_t
HDF5::size(const char *data_name)
{
    Dims dims = dimensions(data_name);
    return size(dims);
}


size_t
HDF5::size(const Dims &dims)
{
    size_t s = 1;
    for (const auto &d : dims)
        s *= d;
    return s;
}


template<typename T>
T
HDF5::attr(const char *attr_name, const char *obj_name)
{
    T a;
    status = H5LTget_attribute(file_id, obj_name, attr_name, h5t<T>(), &a);
    check_error("H5LTget_attribute");
    return a;
}


template<typename T>
void
HDF5::write(const char *name, const std::vector<T> &data, Dims *dims)
{
    if (read_only()) {
        throw std::runtime_error(std::string("Cannot write\"") +
                                 name +
                                 "\": file is opened read-only");
    }
    // printf("name = %s\n", name);
    Dims _dims {data.size()};
    if (dims == nullptr) {
        dims = &_dims;
    }
    const int rank = int(dims->size());
    status = make_dataset(file_id,
                          name,
                          rank,
                          dims->data(),
                          h5t<T>(),
                          data.data(),
                          cid,
                          compress);
    check_error("make_dataset");
}


template<typename T>
void
HDF5::owrite(const char *name, const std::vector<T> &data, Dims *dims)
{
    if (hm == int(H5F_ACC_RDONLY))
        throw std::runtime_error("Can't write because opened with mode \"r\"");
    if (has(name))
        H5Ldelete(file_id, name, H5P_DEFAULT);
    write(name, data, dims);
}


std::string
HDF5::libversion()
{
    unsigned major, minor, release;
    H5get_libversion(&major, &minor, &release);
    return
        std::to_string(major) + "." +
        std::to_string(minor) + "." +
        std::to_string(release);
}
