using HDF5

fname = "belgium.h5"

head = h5read(fname, "head")
tail = h5read(fname, "tail")

n, m = 1441295, 1549970
@assert all(head .!= tail)
@assert maximum(head) < n

