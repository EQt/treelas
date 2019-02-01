"""
    find_root(pi)

Find the first index `i` with `pi[i] == i`.
Report an error if non such exist.
"""
function find_root(pi::Vector{Int})::Int
    for i = 1:length(pi)
        pi[i] == i && return i
    end
    error("no root found")
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
