struct NeighborIndex
    idx::Vector{Int}
    pi::Vector{Tuple{Int, Int}}
end

@inline Base.getindex(c::NeighborIndex, j::Int) =
    view(c.pi, c.idx[j]:c.idx[j+1]-1)


function compute_undirected_index(selected::Vector{Bool},
                                  head::Vector{Int},
                                  tail::Vector{Int},
                                  n::Int)
    m = length(selected)
    @assert m >= n-1
    pi = Vector{Tuple{Int,Int}}(undef, 2m)
    idx = zeros(T, n+1)
    for eii in selected         # compute degrees
        ei = edges[eii]
        idx[source(ei)] += 1
        idx[target(ei)] += 1
    end
    acc = 1                     # accumulate degrees ==> positions
    deg_i = 0
    deg_ii = idx[1]
    for i = 1:n
        idx[i] = acc
        acc += deg_i
        deg_i, deg_ii = deg_ii, idx[i+1]
    end
    idx[n+1] = acc
    @assert(idx[end] + deg_i == 2m + 1,
            "idx[$(length(idx))]: $(idx[end] + deg_i) != $(2m + 1)")
    for (i, eii) in enumerate(selected)
        ei = edges[eii]
        u, v = source(ei), target(ei)
        pi[idx[u+1]] = (v, i)
        idx[u+1] += 1
        pi[idx[v+1]] = (u, i)
        idx[v+1] += 1
    end
    @assert(idx[end] == 2m + 1, "$(idx[end]) vs $(2m + 1)")
    return NeighborIndex(idx, pi)
end


function compute_undirected_index(edges::E, n, T::Type = Int) where E
    m = length(edges)
    pi = Vector{Tuple{T,T}}(undef, 2m)
    idx = Vector{T}(undef, n+1)

    return _neighbor_index(m, n, pi, idx, edges)
end


function _neighbor_index(m::Int, n::Int, pi::Vector{Tuple{Int,Int}},
                         idx::Vector{Int}, edges::E) where E
    idx .= 0
    for ei in edges             # compute degrees
        idx[source(ei)] += 1
        idx[target(ei)] += 1
    end
    acc = 1                     # accumulate degrees ==> positions
    deg_i = 0
    deg_ii = idx[1]
    for i = 1:n
        idx[i] = acc
        acc += deg_i
        deg_i, deg_ii = deg_ii, idx[i+1]
    end
    idx[n+1] = acc
    @assert(idx[end] + deg_i == 2m + 1,
            "idx[$(length(idx))]: $(idx[end] + deg_i) != $(2m + 1)")
    for (i, ei) in enumerate(edges)
        u, v = source(ei), target(ei)
        pi[idx[u+1]] = (v, i)
        idx[u+1] += 1
        pi[idx[v+1]] = (u, i)
        idx[v+1] += 1
    end
    @assert(idx[end] == 2m + 1, "$(idx[end]) vs $(2m + 1)")
    return NeighborIndex(idx, pi)
end
