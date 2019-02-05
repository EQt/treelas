import .Grid


function neighbors_lambda(g::Grid.GridGraph)
    n = Grid.num_nodes(g)
    idx = zeros(Int, n+1)
    local m::Int = 0
    Grid.iter_edges(g) do h::Int, t::Int, _::Float64
        m += 1
        idx[h] += 1
        idx[t] += 1
    end
    acc = 1                        # accumulate degrgees ==> positions
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
    local i::Int = 0
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


function Base.getindex(g::Grid.GridGraph, v::Int)
    i, j = divrem(v-1, g.n1)
    i += 1
    j += 1
    if (i, j) == (1, 1)
        :nw
    elseif (i, j) == (1, g.n2)
        :ne
    elseif (i, j) == (g.n1, 1)
        :sw
    elseif (i, j) == (g.n1, g.n2)
        :se
    elseif i == 1
        :n
    elseif i == g.n1
        :s
    elseif j == 1
        :w
    elseif j == g.n2
        :e
    else
        :m
    end
end
