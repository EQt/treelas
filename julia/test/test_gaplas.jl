include("../src/gaplas.jl")

using Test
import .GapLas
import GraphIdx.Grid: num_nodes, num_edges, GridGraph


@testset "GapLas: 2x3                    " begin
    g = GridGraph(2, 3)
    @test num_nodes(g) == 6
    @test num_edges(g) == 2*2 + 3
end

