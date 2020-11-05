module TestCycleGap

using Test
import GraphIdx

include("../src/cyclegap.jl")
include("demo3x7.jl")
n1, n2 = size(y)
lam = GraphIdx.Const(0.1)


@testset "Cycle Gap RotateDemo 3x7       " begin
    graph = GraphIdx.Grid.GridGraph(n1, n2)
    x = CycleGap.gaplas(copy(y), graph, lam, max_iter = 3, CycleGap.CycMem)
    @test size(x) == size(y)
end


end
