"""
`GraphIdx.jl`
=============

Nodes
-----

A node (sometimes also *vertex* called) in a graph is always represented as an integer.
To be able to use a node as an index, the nodes should be within `1:n` whereby `n` is the number of nodes.
Furtheron, we always use **signed** integers because we might include extra information within the first (sign) bit.


Most important submodules are
- [`LinA`](@ref): Linear algebra (e.g. incidence matrix)
- [`Grid`](@ref): Specialized code for grid graphs.
- [`Utils.Heap`](@ref): Priority queues (e.g. binary heap)
"""
module GraphIdx

module Utils

include("utils/unionfind.jl")
include("utils/heap.jl")

end

include("cluster.jl")
include("neighbors.jl")
include("tree.jl")
include("prim_mst.jl")
include("kruskal_mst.jl")
include("lina.jl")
include("grid.jl")
include("grididx.jl")
include("weightedgraph.jl")
include("bits.jl")

end
