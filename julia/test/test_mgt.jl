module TestMGT
include("square.jl")

using Test
import TreeLas.MGT



@testset "MGT     : Square               " begin
    edges, n = square_edges()
    m = length(edges)
    lambda = fill(0.5, length(edges))
    y = Float64[0 1; 0 0.5]
    x = MGT.gaplas(y, edges, lambda)
    @test size(x) == size(y)
    @test sum(x) ≈ sum(y)
    @test x ≈ mean(y) * ones(size(y)...)
end

end
