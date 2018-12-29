module TestNeighbors
using Test

include("../src/neighbors.jl")

@testset "NeighborIndex: Square          " begin
    #       1  2  3  4
    head = [1, 1, 2, 3]
    tail = [2, 3, 4, 4]
    idx = NeighborIndex(4, head, tail)
    @test length(idx) == 4
    @test Set(idx[1]) == Set([(2, 1), (3, 2)])
    @test Set(idx[2]) == Set([(1, 1), (4, 3)])
    @test Set(idx[3]) == Set([(1, 2), (4, 4)])
    @test Set(idx[4]) == Set([(2, 3), (3, 4)])
end

end
