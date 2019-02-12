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
function dfs_walk(f::Function, tree::ChildrenIndex,
                  stack::Vector{Int} = Int[])
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


dfs_walk(f::Function, parent::Vector{Int}, stack::Vector{Int} = Vector{Int}()) =
    dfs_walk(f, ChildrenIndex(parent), stack)


function dfs_finish(parent::Vector{Int}, root::Int = Int(0))::Vector{Int}
    if root <= 0                            # find root node
        root = find_root(pi)
    end
    @assert parent[root] == root

    n = length(parent)
    neigh = [Int[] for i=1:n]
    for (u, v) in enumerate(parent)
        push!(neigh[u], v)
        push!(neigh[v], u)
    end
    dfs = zeros(Int, n)
    stack = Int[root]
    sizehint!(stack, n)
    time = 0
    while !isempty(stack)
        time += 1
        v = pop!(stack)
        dfs[time] = v
        for u in neigh[v]
            parent[v] == u && continue      # skip root
            push!(stack, u)
        end
    end
    return dfs
end
