"""
`GraphIdx`
==========

Most important submodules are
- [`LinA`](@ref): Linear algebra (e.g. incidence matrix)
- [`Grid`](@ref): Specialized code for grid graphs.
- [`Heap`](@ref): Priority queues (e.g. binary heap)
"""
module GraphIdx

"""
A rooted tree.
Often you might want to have a `ChildrenIndex`.
"""
struct Tree
    root::Int
    parent::Vector{Int}
end


include("degrees.jl")
include("children.jl")
include("cluster.jl")
include("dfs.jl")
# include("neighbors.jl")   # will be included by mst.jl
include("prim_mst.jl")
include("kruskal_mst.jl")
include("lina.jl")
include("grid.jl")
include("grididx.jl")

end
