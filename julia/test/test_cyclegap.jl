module TestCycleGap

using Test
import GraphIdx

include("../src/cyclegap.jl")

@testset "Cyccle Gap RotateDemo 3x7      " begin
    include("demo3x7.jl")
    n1, n2 = size(y)
    graph = GraphIdx.Grid.GridGraph(n1, n2)
    lam = GraphIdx.Const(1.0)
    x = CycleGap.gaplas(y, graph, lam, max_iter = 3)
    @test size(x) == size(y)
end


end
