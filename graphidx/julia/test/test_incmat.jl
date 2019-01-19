module TestIncMat
include("../src/lina.jl")
include("../../../julia/test/square.jl")

using Test
import SparseArrays: nnz
import .LinA: IncMat


@testset "Create D: Square               " begin
    edges, n = square_edges()
    m = length(edges)
    lambda = fill(0.5, length(edges))
    D = IncMat(edges, lambda, n)
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
