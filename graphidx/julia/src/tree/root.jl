"""
    find_root(pi)

Find the first index `i` with `pi[i] == i`.
Report an error if non such exist.

# Example

```jldoctest
julia> find_root([2, 1, 3, 2])
3

```
"""
function find_root(pi::Vector{Int})::Int
    for i = 1:length(pi)
        pi[i] == i && return i
    end
    error("no root found")
end
