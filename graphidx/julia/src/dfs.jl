"""
    signed_dfs(f, tree [,stack])

Call `f` on each node of `tree` in depth-first search (DFS) order.
Hereby the node `v` is negative `v < 0` if the node is discovered
the first time and non-negative (`v >= 0`) if the node has been
finished.

To avoid allocation, you can pass a `stack` Vector.
"""
function signed_dfs(f::Function, tree::ChildrenIndex,
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

