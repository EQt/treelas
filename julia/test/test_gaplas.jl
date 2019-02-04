include("../src/gaplas.jl")

using Test
import .GapLas
import GraphIdx.Grid: num_nodes, num_edges, GridGraph, iter_edges
import GraphIdx: NeighborIndex
import GraphIdx


function GraphIdx.NeighborIndex(g::GridGraph)
    lambda = zeros(num_edges(g))
    # drop3rd = (a, b, c) -> (a, b)
    # for (u, v) in it()
    #     println(u, " -- ", v)
    # end
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
    idx = NeighborIndex(g)
end
