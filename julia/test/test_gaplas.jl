include("../src/gaplas.jl")

using Test
import .GapLas
import GraphIdx.Grid: num_nodes, num_edges, GridGraph, iter_edges


@testset "GridGraph: 2x3                 " begin
    g = GridGraph(2, 3)
    m = 2*2 + 3
    n = 6
    @test num_nodes(g) == n
    @test num_edges(g) == m

    begin
        local k = 0
        iter_edges(g) do u, v, lam
            k += 1
            if k == 1
                @test u == 1
                @test v == 2
            elseif k == 4
                @test u == 1
                @test v == 4
            elseif k == 5
                @test u == 2
                @test v == 5
            else
                @test u < v
            end
            @test lam ≈ 1
        end
        @test k == m
    end

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
    # @test gam[6] ≈ 0
    # @test gam[7] ≈ 0
end
