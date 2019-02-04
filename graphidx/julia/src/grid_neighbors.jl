import .Grid


function neighbors_lambda(g::Grid.GridGraph)
    n = Grid.num_nodes(g)
    idx = zeros(Int, n+1)
    local m = 0
    Grid.iter_edges(g) do h::Int, t::Int, _::Float64
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
    @assert(idx[end] + deg_i == 2m + 1,
            "idx[$(length(idx))]: $(idx[end] + deg_i) != $(2m + 1)")
    pi = Vector{Tuple{Int,Int}}(undef, 2m)
    lam = Vector{Float64}(undef, m)
    local i = 0
    Grid.iter_edges(g) do u::Int, v::Int, lam_i::Float64
        i += 1
        lam[i] = lam_i
        pi[idx[u+1]] = (v, i)
        idx[u+1] += 1
        pi[idx[v+1]] = (u, i)
        idx[v+1] += 1
    end
    @assert(idx[end] == 2m + 1, "$(idx[end]) vs $(2m + 1)")
    return NeighborIndex(idx, pi), lam
end
