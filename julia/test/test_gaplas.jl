module TestGapLas

using Test
import TreeLas: GapLas
import GraphIdx.Grid: GridGraph
import GraphIdx

@testset "GapLas  : demo 3x7             " begin
    include("demo3x7.jl")
    graph = GridGraph(size(y)...)
    λ = GraphIdx.Ones{Float64}()
    μ = GraphIdx.Ones{Float64}()
    mem = GapLas.GapMem(y, graph, λ)
    GapLas.gaplas!(mem, y, graph, λ, μ)
    @test round.(mem.α_tree, digits=3) ≈ [
        -0.0, 0.179, -0.101, -0.179, -0.248, 0.505, -0.273,
        -1.0, 0.127, -0.405, -0.618, -0.151, 0.254, -0.758,
        -0.997, 0.582, 0.171, 0.266, -0.238, 0.223, 0.442
    ]
    @test round.(mem.sol.α, digits=3) ≈ [
        +0.0,   +0.248, -0.273, +0.0,   +0.0,   -0.171, -0.238, +0.0,
        -0.505, -1.0,   +0.0,   -0.758, -0.266, +0.0,   +0.179, +0.0,
        +0.405, -0.254, -0.582, +0.0,   +0.179, +0.0,   +0.618, +0.0,
        +0.0,   -0.223, -0.101, -0.127, +0.151, +0.0,   -0.997, -0.442
    ]
end


end
