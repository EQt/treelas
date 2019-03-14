module TestNeighbors
using Test

include("../src/incidence.jl")

@testset "IncidenceIndex: Square         " begin
    #       1  2  3  4
    head = [1, 1, 2, 3]
    tail = [2, 3, 4, 4]
    idx = IncidenceIndex(4, head, tail)
    @test length(idx) == 4
    @test Set(idx[1]) == Set([(2, 1), (3, 2)])
    @test Set(idx[2]) == Set([(1, 1), (4, 3)])
    @test Set(idx[3]) == Set([(1, 2), (4, 4)])
    @test Set(idx[4]) == Set([(2, 3), (3, 4)])
end


@testset "IncidenceIndex: Square -1      " begin
    #       1  2  3  4
    head = [1, 1, 2, 0]
    tail = [2, 3, 4, 4]
    idx = IncidenceIndex(4, head, tail)
    @test length(idx) == 4
    @test Set(idx[1]) == Set([(2, 1), (3, 2)])
    @test Set(idx[2]) == Set([(1, 1), (4, 3)])
    @test Set(idx[3]) == Set([(1, 2)])
    @test Set(idx[4]) == Set([(2, 3)])
end


@testset "IncidenceIndex: Test_1         " begin
    #       1  2  3  4
    head = [1, 2, 3, 4]
    tail = [2, 4, 2, 3]
    idx = IncidenceIndex(4, head, tail)
    @test length(idx) == 4
    @test Set(idx[1]) == Set([(2, 1)])
    @test Set(idx[2]) == Set([(1, 1), (4, 2), (3, 3)])
    @test Set(idx[3]) == Set([(2, 3), (4, 4)])
    @test Set(idx[4]) == Set([(2, 2), (3, 4)])
end


@testset "IncidenceIndex: Petgraph::Csr  " begin
    edges = [   # zero indexed!
        (0, 1),
        (0, 2),
        (1, 0),
        (1, 1), # self-reference is not a problem
        (2, 2),
        (2, 4),
    ]
    edges = reshape(reinterpret(Int, edges), 2, length(edges))' .+ 1
    n = maximum(edges)
    @test n == 5
    edges = vec(mapslices(Tuple, edges, dims = [2]))
    idx = IncidenceIndex(n, edges)
    @test Set(map(first, idx[1])) == Set([2, 3])
    @test Set(map(first, idx[2])) == Set([0, 1] .+ 1)
    @test Set(map(first, idx[3])) == Set([0, 2, 4] .+ 1)  # we take both directions!
    @test Set(map(first, idx[4])) == Set([])
    @test Set(map(first, idx[5])) == Set([3])     # both directions!
end


end
