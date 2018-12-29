module TestNeighbors
using Test

include("../src/neighbors.jl")

@testset "NeighborIndex: Square          " begin
    head = [1, 1, 2, 3]
    tail = [2, 3, 4, 4]
    idx = NeighborIndex(4, head, tail)
    @test length(idx) == 4
    @test Set(idx[1]) == Set([(2, 1), (3, 2)])
end

end
