module TestMGT
using Test

include("../src/mgt.jl")
include("square.jl")


@testset "MGT     : Square               " begin
    edges, n = square_edges()
    m = length(edges)
    lambda = fill(0.5, length(edges))
    y = Float64[0 1; 0 0.5]
    x = MGT.max_gap_tree(y, edges, lambda)
    @test size(x) == size(y)
    @test sum(x) ≈ sum(y)
end

end
