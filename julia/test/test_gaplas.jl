include("../src/gaplas.jl")

using Test
import .GapLas
import GraphIdx.Grid: num_nodes, num_edges, GridGraph


@testset "GridGraph: 2x3                 " begin
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
end
