import TreeLas
import GraphIdx
import HDF5: h5read, h5write

fname = ARGS[1]
lam = parse(Float64, ARGS[2])
outfn = ARGS[3]

println(fname, " lam ", lam, " => ", outfn)
dn = 1
niter = 150
y = h5read(fname, "y")
graph = GraphIdx.Grid.GridGraph(size(y)..., dn)
edges = GraphIdx.Grid.collect_edges(graph)[1]
x = TreeLas.MGT.gaplas(y, edges, lam, max_iter=niter, learn=0.78);
h5write(outfn, "xgrid", x)
