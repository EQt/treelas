"""
    dfs_walk(f, tree [,stack])

Call `f` on each node of `tree` in depth-first search (DFS) order.
Hereby the node `v` is negative `v < 0` if the node is discovered
the first time and non-negative (`v >= 0`) if the node has been
finished.

To avoid allocation, you can pass a `stack` Vector.
"""
function dfs_walk(f::Function, tree::ChildrenIndex,
                  stack::Vector{Int} = Vector{Int}())
    sizehint!(stack, length(tree))
    push!(stack, ~root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        f(v)
        if v < 0
            v = ~v
            push!(stack, v)
            for u in tree[v]
                push!(stack, ~u)
            end
        end
    end
end


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
