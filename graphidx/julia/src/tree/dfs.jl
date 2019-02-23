"""
    dfs_walk(f, tree [,stack])

Call `f` on each node of `tree` in depth-first search (DFS) order.
Hereby the node `v` is negative `v < 0` if the node is discovered
the first time and non-negative (`v >= 0`) if the node has been
finished.

The `tree` can be given as `ChildrenIndex` or parent `Vector{Int}`;
in the latter case `ChildrenIndex` will be constructed.
To avoid allocation, you can pass a `stack` Vector.

# Example

```jldoctest
julia> tree = ChildrenIndex([1, 1, 1, 3, 1]); hierarchy(tree)
1
├─2
├─3
│ └─4
└─5

julia> dfs_walk(tree) do v
          println(v >= 0 ? "finished   " : "discovered ", abs(v))
       end
discovered 1
discovered 2
finished   2
discovered 3
discovered 4
finished   4
finished   3
discovered 5
finished   5
finished   1
    

```

"""
function dfs_walk(f::Function, tree::ChildrenIndex, stack::Vector{Int} = Int[])
    @assert isempty(stack)
    sizehint!(stack, length(tree))
    push!(stack, -root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        f(v)
        if v < 0
            v = -v
            push!(stack, v)
            for u in @view tree[v][end:-1:1]
                push!(stack, -u)
            end
        end
    end
end


dfs_walk(f::Function, parent::Vector{Int}, stack::Vector{Int} = Int[]) =
    dfs_walk(f, ChildrenIndex(parent), stack)


"""
    dfs_walk_rev(f, tree::ChildrenIndex [, stack])

Like [`dfs_walk`](@ref) but process the children in reversed order!

# Example

```jldoctest
julia> tree = ChildrenIndex([1, 1, 1, 3, 1]); hierarchy(tree)
1
├─2
├─3
│ └─4
└─5

julia> dfs_walk_rev(tree) do v
          println(v >= 0 ? "finished   " : "discovered ", abs(v))
       end
discovered 1
discovered 5
finished   5
discovered 3
discovered 4
finished   4
finished   3
discovered 2
finished   2
finished   1


```
"""
function dfs_walk_rev(f::Function, tree::ChildrenIndex, stack::Vector{Int} = Int[])
    @assert isempty(stack)
    sizehint!(stack, length(tree))
    push!(stack, -root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        f(v)
        if v < 0
            v = -v
            push!(stack, v)
            for u in tree[v]
                push!(stack, -u)
            end
        end
    end
end


"""
    dfs_finish(parent)

For each node, compute the DFS finish time (without computing a ChildrenIndex).

```jldoctest
julia> pi = [1, 1, 2, 1, 2, 3]; hierarchy(ChildrenIndex(pi))
1
├─2
│ ├─3
│ │ └─6
│ └─5
└─4

julia> dfs_finish(pi)
6-element Array{Int64,1}:
 6
 3
 5
 2
 4
 1

```
"""
dfs_finish(parent::Vector{Int}, stack::Vector{Int} = Int[])::Vector{Int} =
    dfs_finish(ChildrenIndex(parent), stack)


function dfs_finish(cidx::ChildrenIndex, stack::Vector{Int} = Int[])::Vector{Int}
    local n = num_nodes(cidx)
    local dfs::Vector{Int} = zeros(Int, n)
    local time::Ref{Int} = Ref{Int}(0)

    dfs_walk(cidx, stack) do v::Int
        if v >= 0
            time[] += 1
            dfs[time[]] = v
        end
    end

    return dfs
end
