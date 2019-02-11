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

include("tree/root.jl")
include("tree/degrees.jl")
include("tree/children.jl")
include("tree/dfs.jl")
include("tree/lca.jl")
include("tree/inorder.jl")
include("tree/hierarchy.jl")

end
