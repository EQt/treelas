/*
  A thin C++ layer to handle basic HDF5
  Inspired by h5py (Python) and HDF5.jl (Julia)

  DONE: Add compression
     https://support.hdfgroup.org/HDF5/Tutor/compress.html

  TODO: Write attributes.
   https://github.com/mortenpi/hdf5/blob/master/hl/src/H5LT.c
   Distinguish between numerical and string attributes.

  File Interface:
     https://support.hdfgroup.org/HDF5/doc/RM/RM_H5F.html
  Source Code:
     https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8.17/src/hdf5-1.8.17.zip
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
   HDF5 file (and utility methods).
*/
class HDF5
{
public:
    /**
       As long as an instance of this class exists, there won't be any HDF5
       error/warning traces (e.g. when opening a non-existing file.

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

    /**
       Check whether there exists, from the current location, a group or dataset
       named `data_name`
    */
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

    /** Return an attribute */
    template<typename T>
    T attr(const char *attr_name, const char *obj_name = "/");

    /** Write into a new dataset, i.e. data_name must not exist */
    template<typename T>
    void write(const char *data_name, const std::vector<T> &data, Dims *dims = nullptr);

    /** Install a compression filter for writing; assert 0 <= c <= 9. */
    void set_compression(int c);

    /** Overwrite (if exists) */
    template<typename T>
    void owrite(const char *data_name, const std::vector<T> &data, Dims *dims = nullptr);

    /** Disable libhdf5 warnings/errors */
    static void shutup();

    /** Get and set, and, if not exists, create the current group */
    const std::string& group(const char *g = "/");
    const std::string& group(const std::string &g) {  return group(g.c_str()); }

    /** Return the HDF5 liberary version */
    static std::string libversion();

private:
    hid_t file_id, loc_id;
    unsigned hm = H5F_ACC_EXCL;
    herr_t status = 0;
    std::string _group = "/";
    hid_t cid = H5P_DEFAULT;
    int compress = 0;

    void close_compression_filter();

    void check_error(const char *msg);
    void check_error(const std::string s) { check_error(s.c_str()); }

    bool read_only() const { return hm == (hid_t)H5F_ACC_RDONLY; }
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
    loc_id = file_id;
    if (compress > 0) {
        set_compression(compress);
    }
}


HDF5::~HDF5()
{
    close_compression_filter();
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


const std::string&
HDF5::group(const char *g)
{
    std::string grp (g);
    if (grp[0] != '/') {    // relative to current group
        if (_group == "/")
            grp = "/" + grp;
        else
            grp = _group + "/" + grp;
    }
    if (grp != "/" && grp != _group) {
        if (!has(grp.c_str())) {
            if (read_only()) {
                throw std::runtime_error(grp + "does not exist and read only");
            } else {
                // https://support.hdfgroup.org/HDF5/doc1.6/UG/09_Groups.html
                const auto g = H5Gcreate1(file_id, grp.c_str(), 0);
                if (g < 0) {
                    status = herr_t(g);
                    check_error("H5Gcreate");
                }
            }
        }
        _group = grp;
    } 
    return _group;
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


inline bool
_h5exists(hid_t id, const char *name)
{
    // https://support.hdfgroup.org/HDF5/doc/RM/RM_H5L.html#Link-Exists
    const auto status = H5Lexists(id, name, H5P_DEFAULT);
    if (status > 0)
        return true;
    if (status < 0)
        throw std::runtime_error(std::string("_h5exists(...") + name + ")");
    return false;
}



bool
HDF5::has(const char *data_name)
{
    // ShutUp _;
    std::string buf (data_name);
    data_name = buf.c_str();
    // printf("abs = %s\n", data_name);
    std::size_t split = buf.rfind('/');
    if (split == std::string::npos || split == 0) {
        return _h5exists(loc_id, data_name);
    }
    buf[split] = '\0';
    const char *group_name = data_name;
    data_name = data_name + split + 1;
    // printf("g=%s, d=%s\n", group_name, data_name);
    hid_t group_id = H5Gopen2(file_id, group_name, H5P_DEFAULT);
    return _h5exists(group_id, data_name);
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
    throw std::runtime_error(std::string("data_name = ") + data_name);
    if (data_name[0] == '\0')
        throw std::runtime_error("Internal error");
    if (std::string(data_name) == "")
        throw std::runtime_error("Internal error");
    dims->resize(ndims(data_name));
    status = _get_dataset_info(file_id, data_name,
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
