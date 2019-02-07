"""
`GraphIdx`
==========

Most important submodules are
- [`LinA`](@ref): Linear algebra (e.g. incidence matrix)
- [`Grid`](@ref): Specialized code for grid graphs.
- [`Utils.Heap`](@ref): Priority queues (e.g. binary heap)
"""
module GraphIdx

module Utils

include("unionfind.jl")
include("heap.jl")

end

include("cluster.jl")
include("neighbors.jl")
include("tree.jl")
include("prim_mst.jl")
include("kruskal_mst.jl")
include("lina.jl")
include("grid.jl")
include("grididx.jl")

end
