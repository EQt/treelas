"""
In a graph with numbered edges, provide access to neighbors and edge numbers for a specified node.

!!! note

    [`NeighborIndex`](@ref) always includes both directions, i.e. for an edge `(u, v)` with edge index `ei`, it is
    `(u, ei) ∈ neighbors[v]` and `(v, ei) ∈ neighbors[u]`.
"""
struct NeighborIndex
    idx::Vector{Int}
    pi::Vector{Tuple{Int, Int}}
end

@inline Base.getindex(c::NeighborIndex, j::Int) =
    view(c.pi, c.idx[j]:c.idx[j+1]-1)


"""
    num_edges(::NeighborIndex)

Actual number of (undirected) edges (**not counting** both directions).
"""
num_edges(c::NeighborIndex) = length(c.pi) ÷ 2

num_nodes(c::NeighborIndex) = length(c.idx) - 1

Base.length(c::NeighborIndex) = num_nodes(c)


"""
    NeighborIndex(n, head, tail)

Construct an index for `n` nodes of the (undirected) neighbors given
by the edges `zip(head, tail)`.
If `head[i] <= 0` then edge `i` is excluded.
""" 
function NeighborIndex(n::Int, head::Vector{Int}, tail::Vector{Int})
    @assert length(head) == length(tail)
    NeighborIndex(n, () -> zip(head, tail))
end


"""
    NeighborIndex(n, edges)

Same but for edges like `[(1, 2), (2, 3)]`
"""
NeighborIndex(n::Int, edges::Vector{Tuple{Int, Int}}) =
    NeighborIndex(n, () -> edges)

"""
    NeighborIndex(n, m, iter::Function)

Provide an iterator over the edges
"""
function NeighborIndex(n::Int, iter::Function)
    local m::Int = 0
    idx = zeros(Int, n+1)
    for (h::Int, t::Int) in iter()
        h <= 0 && continue
        m += 1
        idx[h] += 1
        idx[t] += 1
    end
    acc = 1                        # accumulate degrees ==> positions
    deg_i = 0
    deg_ii = idx[1]
    for i = 1:n
        idx[i] = acc
        acc += deg_i
        deg_i, deg_ii = deg_ii, idx[i+1]
    end
    idx[n+1] = acc
    pi = Vector{Tuple{Int,Int}}(undef, 2m)
    @assert(idx[end] + deg_i == 2m + 1,
            "idx[$(length(idx))]: $(idx[end] + deg_i) != $(2m + 1)")
    for (i, (u, v)) in enumerate(iter())
        u <= 0 && continue
        pi[idx[u+1]] = (v, i)
        idx[u+1] += 1
        pi[idx[v+1]] = (u, i)
        idx[v+1] += 1
    end
    @assert(idx[end] == 2m + 1, "$(idx[end]) vs $(2m + 1)")
    return NeighborIndex(idx, pi)
end
