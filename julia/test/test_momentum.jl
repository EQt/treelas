module TestMomentum

using Test
import GraphIdx
import TreeLas: Momentum
import TreeLas: MGT


include("demo3x7.jl")
n1, n2 = size(y)
lam = GraphIdx.Const(0.1)


@testset "Momentum Demo 3x7              " begin
    graph = GraphIdx.Grid.GridGraph(n1, n2)
    x = Momentum.gaplas(copy(y), graph, lam, max_iter=10, Momentum.MomMem, verbose=true)
    if false    # compare to old implementation?
        x2 = MGT.gaplas(copy(y), graph, lam, max_iter=10, learn=0.85, verbose=true)
    end
end


end
