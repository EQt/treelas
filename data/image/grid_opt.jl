import TreeLas
import GraphIdx
import HDF5: h5read, h5write

fname = ARGS[1]
lam = parse(Float64, ARGS[2])
outfn = ARGS[3]
niter = parse(Int, get(ARGS, 4, "150"))
learn = parse(Float64, get(ARGS, 5, "0.78"))
dn = parse(Int, get(ARGS, 6, "1"))

println(fname, " lam ", lam, " => ", outfn)
y = h5read(fname, "y")
graph = GraphIdx.Grid.GridGraph(size(y)..., dn)
edges = GraphIdx.Grid.collect_edges(graph)[1]
x = TreeLas.MGT.gaplas(y, edges, lam, max_iter=niter, learn=learn);
h5write(outfn, "xgrid", x)
