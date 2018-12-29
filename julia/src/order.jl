function find_root(pi::Vector{Int})::Int
    for i = 1:length(pi)
        pi[i] == i && return i
    end
    error("no root found")
end


function dfs_finish(pi::Vector{Int}, root=0)
    if root <= 0    # find root node
        root = find_root(pi)
    end

    n = length(pi)
    stack = Vector{Int}()
    sizehint!(stack, n)
    push!(stack, root)
    while !isempty(stack)
        v = pop!(stack)
    end
end 
