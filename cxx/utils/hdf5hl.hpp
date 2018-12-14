#pragma once
#include <hdf5.h>


/*-------------------------------------------------------------------------
 * Function: H5LTget_attribute
 *
 * Purpose: Reads an attribute named attr_name with the memory type mem_type_id
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: September 19, 2002
 *
 * Comments: Private function
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
_get_attribute( hid_t loc_id,
                const char *obj_name,
                const char *attr_name,
                hid_t mem_type_id,
                void *data )
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




/*-------------------------------------------------------------------------
 * Function: H5LTread_dataset
 *
 * Purpose: Reads a dataset from disk.
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: June 13, 2001
 *
 *-------------------------------------------------------------------------
 */
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


/*-------------------------------------------------------------------------
 * Function: H5LTget_dataset_ndims
 *
 * Purpose: Gets the dimensionality of a dataset.
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: September 4, 2001
 *
 *-------------------------------------------------------------------------
 */
herr_t
_get_dataset_ndims( hid_t loc_id,
                    const char *dset_name,
                    int *rank )
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
