include("../src/gaplas.jl")

using Test
import .GapLas
import GraphIdx.Grid: num_nodes, num_edges, GridGraph, iter_edges
import GraphIdx: NeighborIndex


function neighbors_lambda(g::GridGraph)
    n = num_nodes(g)
    idx = zeros(Int, n+1)
    local m = 0
    iter_edges(g) do h::Int, t::Int, _
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
    iter_edges(g) do u::Int, v::Int, lam_i::Float64
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


@testset "GapLas   : 2x3                 " begin
    g = GridGraph(2, 3)
    m = 2*2 + 3
    n = 6
    @test num_nodes(g) == n
    @test num_edges(g) == m

    x = zeros(Float64, 2, 3)
    gam = zeros(Float64, m)
    alpha = zeros(Float64, m)
    gam .= -1
    @test all(gam .< 0)
    GapLas.gap_vec!(gam, x, alpha, g)
    @test all(gam .≈ 0)

    x[1] = 13
    gam .= -1
    @test all(gam .< 0)
    GapLas.gap_vec!(gam, x, alpha, g)
    @test gam[1] ≈ 13
    @test gam[4] ≈ 13
    for e = 1:m
        if !(e in [1, 4])
            @test gam[e] ≈ 0
        end
    end

    x .= 0
    x[1, 1] = 13
    x[1, 2] = 13
    gam .= -1
    @test all(gam .< 0)
    GapLas.gap_vec!(gam, x, alpha, g)
    @test gam[1] ≈ +13
    @test gam[2] ≈ +13
    @test gam[3] ≈ 0
    @test gam[4] ≈ 0
    @test gam[5] ≈ 13
    @test gam[6] ≈ 0
    @test gam[7] ≈ 0
end


@testset "NeighborIndex(GridGraph(2, 3)  " begin
    g = GridGraph(2, 3)
    idx, lam = neighbors_lambda(g)
    @test all(lam .≈ 1)
    @test Set(idx[1]) == Set([(2, 1), (3, 4)])
end
