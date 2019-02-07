"""
Bundle functions regarding tree graphs.
"""
module Tree

"""
A rooted tree.
Often you might want to have a [`ChildrenIndex`](@ref).
"""
struct RootedTree
    root::Int
    parent::Vector{Int}
end


include("root.jl")
include("degrees.jl")
include("children.jl")
include("dfs.jl")
include("lca.jl")

end
