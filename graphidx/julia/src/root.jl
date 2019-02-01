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
