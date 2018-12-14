/**
  A thin header-only C++ layer to read and write HDF5 files.
  Inspired by h5py (Python) and HDF5.jl (Julia)

  Author: Elias Kuthe
  Copyright: 2018 MIT


  Private functions start with _ (as in Python).
  Some of them are copied (and possibly adapted) from hdf5hl.

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

    template<typename T>
    size_t readv(std::vector<T> &v, const char *data_name) {
        v = read<T>(data_name);
        return v.size();
    }

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

    /** Return the HDF5 liberary version (extracted dynamically), e.g.
        "1.10.10" */
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
        std::cerr << "HDF5::~HDF5(): This should not happen" << std::endl;
    }
}


void
HDF5::close_compression_filter()
{
    if (cid != H5P_DEFAULT) {
        status = H5Pclose(cid);
    }
}


herr_t
_get_attribute( hid_t loc_id,
                const char *obj_name,
                const char *attr_name,
                hid_t mem_type_id,
                void *data)
{
    /* identifiers */
    hid_t obj_id = -1;
    hid_t attr_id = -1;

    /* check the arguments */
    if (obj_name == NULL)
        return -1;
    if (attr_name == NULL)
        return -1;

    /* Open the object */
    if ((obj_id = H5Oopen(loc_id, obj_name, H5P_DEFAULT)) < 0)
        goto out;

    if ((attr_id = H5Aopen(obj_id, attr_name, H5P_DEFAULT)) < 0)
        goto out;

    if (H5Aread(attr_id, mem_type_id, data) < 0)
        goto out;

    if (H5Aclose(attr_id) < 0)
        goto out;
    attr_id = -1;

    /* Close the object */
    if (H5Oclose(obj_id) < 0)
        goto out;
    obj_id = -1;

    return 0;

out:
    if(obj_id > 0)
        H5Oclose(obj_id);
    if(attr_id > 0)
        H5Aclose(attr_id);
    return -1;
}


herr_t
_read_dataset(hid_t loc_id,
              const char *dset_name,
              hid_t tid,
              void *data)
{
    hid_t   did;

    /* check the arguments */
    if (dset_name == NULL)
        return -1;

    /* Open the dataset. */
    if ((did = H5Dopen2(loc_id, dset_name, H5P_DEFAULT)) < 0)
        return -1;

    /* Read */
    if (H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
        goto out;

    /* End access to the dataset and release resources used by it. */
    if (H5Dclose(did))
        return -1;

    return 0;

out:
    H5Dclose(did);
    return -1;

}


herr_t
_get_dataset_ndims(hid_t loc_id,
                   const char *dset_name,
                   int *rank)
{
    hid_t       did = -1;
    hid_t       sid = -1;


    /* check the arguments */
    if (dset_name == NULL)
        return -1;

    /* Open the dataset. */
    if ((did = H5Dopen2(loc_id, dset_name, H5P_DEFAULT)) < 0)
        return -1;

    /* Get the dataspace handle */
    if ((sid = H5Dget_space(did)) < 0)
        goto out;

    /* Get rank */
    if ((*rank = H5Sget_simple_extent_ndims(sid)) < 0)
        goto out;

    /* Terminate access to the dataspace */
    if (H5Sclose(sid) < 0)
        goto out;

    /* End access to the dataset */
    if (H5Dclose(did))
        return -1;

    return 0;

out:
    H5E_BEGIN_TRY {
        H5Dclose(did);
        H5Sclose(sid);
    } H5E_END_TRY;
    return -1;
}


inline herr_t
_get_dataset_info(hid_t loc_id,
                  const char *dset_name,
                  hsize_t *dims,
                  H5T_class_t *type_class,
                  size_t *type_size)
{
    hid_t       did = -1;
    hid_t       tid = -1;
    hid_t       sid = -1;

    /* check the arguments */
    if (dset_name == NULL)
        return -1;

    /* open the dataset. */
    if ((did = H5Dopen2(loc_id, dset_name, H5P_DEFAULT)) < 0)
        return -1;

    /* get an identifier for the datatype. */
    tid = H5Dget_type(did);

    /* get the class. */
    if (type_class != NULL)
        *type_class = H5Tget_class(tid);

    /* get the size. */
    if (type_size != NULL)
        *type_size = H5Tget_size(tid);

    if (dims != NULL) {
        /* get the dataspace handle */
        if ((sid = H5Dget_space(did)) < 0)
            goto out;

        /* get dimensions */
        if (H5Sget_simple_extent_dims(sid, dims, NULL) < 0)
            goto out;

        /* terminate access to the dataspace */
        if (H5Sclose(sid) < 0)
            goto out;
    }

    /* release the datatype. */
    if (H5Tclose(tid))
        return -1;

    /* end access to the dataset */
    if (H5Dclose(did))
        return -1;

    return 0;

out:
    H5E_BEGIN_TRY {
        H5Tclose(tid);
        H5Sclose(sid);
        H5Dclose(did);
    } H5E_END_TRY;
    return -1;

}


inline herr_t
make_dataset(hid_t loc_id,
             const char *dset_name,
             int rank,
             const hsize_t *dims,
             hid_t tid,
             const void *data,
             hid_t cid = H5P_DEFAULT,
             int compress = 0)
{
    // copied from
    // https://github.com/mortenpi/hdf5/blob/
    //   8a275ab7831002f3e402947facc2d373d9d7926c/hl/src/H5LT.c#L518
    hid_t
        did = -1,
        sid = -1;

    /* Create the data space for the dataset. */
    if ((sid = H5Screate_simple(rank, dims, NULL)) < 0)
        return -1;

    if (cid != H5P_DEFAULT) {
        herr_t status;
        /* Dataset must be chunked for compression */
        status = H5Pset_chunk(cid, rank, dims);
        if (status < 0)
            throw std::runtime_error("Error: H5Pset_chunk");
        /* Set ZLIB / DEFLATE Compression using compression level 6.
         * To use SZIP Compression comment out these lines.
         */
        status = H5Pset_deflate(cid, compress);
        if (status < 0)
            throw std::runtime_error("Error: H5Pset_deflate");
    }

    /* Create the dataset. */
    if ((did = H5Dcreate(loc_id, dset_name, tid, sid,
                         H5P_DEFAULT, cid, H5P_DEFAULT)) < 0)
        goto out;

    /* Write the dataset only if there is data to write */
    if (data)
        if (H5Dwrite(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
            goto out;

    /* End access to the dataset and release resources used by it. */
    if (H5Dclose(did) < 0)
        return -1;

    /* Terminate access to the data space. */
    if (H5Sclose(sid) < 0)
        return -1;

    return 0;

out:
    H5E_BEGIN_TRY {
        H5Dclose(did);
        H5Sclose(sid);
    } H5E_END_TRY;
    return -1;
}


/* TODO: Error handling
   https://support.hdfgroup.org/HDF5/doc/H5.user/Errors.html
*/
inline herr_t
H5Ewalk_error(int n, H5E_error_t *err_desc, void *client_data)
{
    FILE       *stream = (FILE *)client_data;
    const char *maj_str = NULL;
    const char *min_str = NULL;
    const int   indent = 2;

    /* Check arguments */
    assert (err_desc);
    if (!client_data) client_data = stderr;

    /* Get descriptions for the major and minor error numbers */
    maj_str = H5Eget_major (err_desc->maj_num);
    min_str = H5Eget_minor (err_desc->min_num);

    /* Print error message */
    fprintf (stream, "%*s#%03d: %s line %u in %s(): %s\n",
             indent, "", n, err_desc->file_name, err_desc->line,
             err_desc->func_name, err_desc->desc);
    fprintf (stream, "%*smajor(%02d): %s\n",
             indent*2, "", int(err_desc->maj_num), maj_str);
    fprintf (stream, "%*sminor(%02d): %s\n",
             indent*2, "", int(err_desc->min_num), min_str);

    return 0;
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
    _read_dataset(file_id, data_name, h5t<T>(), buf.data());
    check_error("_read_dataset");
    return buf;
}


int
HDF5::ndims(const char *data_name)
{
    // printf("data_name --> %s\n", data_name);
    int ndims;
    status = _get_dataset_ndims(file_id, data_name, &ndims);
    check_error("_get_dataset_ndims");
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
    check_error(std::string("_get_dataset_info: '") +
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
    status = _get_attribute(file_id, obj_name, attr_name, h5t<T>(), &a);
    check_error("_get_attribute");
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
