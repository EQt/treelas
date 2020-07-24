module TestCycleGap

using Test
import GraphIdx

include("../src/cyclegap.jl")
include("demo3x7.jl")
n1, n2 = size(y)


@testset "Cycle Gap RotateDemo 3x7        " begin
    graph = GraphIdx.Grid.GridGraph(n1, n2)
    lam = GraphIdx.Const(1.0)
    x = CycleGap.gaplas(copy(y), graph, lam, max_iter = 3, CycleGap.CycMem)
    @test size(x) == size(y)
end


end
