module TestIncMat
using Test
import SparseArrays: nnz

include("../src/incmat.jl")
include("square.jl")

@testset "Create D: Square               " begin
    edges, n = square_edges()
    m = length(edges)
    lambda = fill(0.5, length(edges))
    D = create_D(edges, lambda, n)
    D_expected =
        [0.5  -0.5   0.0   0.0
         0.5   0.0  -0.5   0.0
         0.0   0.5   0.0  -0.5
         0.0   0.0   0.5  -0.5]

    @test size(D) == (m, n)
    @test nnz(D) == 2m
    @test D_expected ≈ Array(D)
end

end
