module TestIncMat
using Test
import SparseArrays: nnz

include("../src/incmat.jl")

@testset "Create D: Square               " begin
    head = [1, 1, 2, 3]
    tail = [2, 3, 4, 4]
    n = max(maximum(tail), maximum(head))
    m = length(edges)
    edges = collect(zip(head, tail))::Edges
    lambda = fill(0.5, length(edges))
    D = create_D(edges, lambda, n)
    @test size(D) == (m, n)
    @test nnz(D) == 2m
end

end
