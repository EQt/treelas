import GraphIdx
import HDF5: h5read, h5write


fname = ARGS[1]
outfn = ARGS[2]

dn = 1
y = h5read(fname, "y")
graph = GraphIdx.Grid.GridGraph(size(y)..., dn)
edges = GraphIdx.Grid.collect_edges(graph)
edges = edges[1]
head = [h for (h, _) in edges]
tail = [t for (_, t) in edges]
h5write(outfn, "head", head)
h5write(outfn, "tail", tail)

