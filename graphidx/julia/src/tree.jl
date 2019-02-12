"""
Bundle functions regarding (rooted) tree graphs.
"""
module Tree

include("tree/root.jl")

"""
A rooted tree.
Often you might want to have a [`ChildrenIndex`](@ref).
For debugging, the [`hierarchy`](@ref) function might be useful.
"""
struct RootedTree
    root::Int
    parent::Vector{Int}
    RootedTree(parent::Vector{Int}) =
        RootedTree(find_root(parent), parent)
end

include("tree/degrees.jl")
include("tree/children.jl")
include("tree/dfs.jl")
include("tree/lca.jl")
include("tree/inorder.jl")
include("tree/hierarchy.jl")

end
