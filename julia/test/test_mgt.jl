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


@testset "MGT     : demo 3x7             " begin
    y = [0.62 0.73 0.71 1.5 1.17 0.43 1.08
         0.62 1.73 0.95 1.46 1.6 1.16 0.38
         0.9 0.32 -0.48 0.95 1.08 0.02 0.4]

    edges = Tuple{Int64,Int64}[
        (1, 2), (4, 5), (7, 8), (10, 11), (13, 14),
        (16, 17), (19, 20), (2, 3), (5, 6), (8, 9), (11, 12),
        (14, 15), (17, 18), (20, 21), (1, 4), (4, 7), (7, 10),
        (10, 13), (13, 16), (16, 19), (2, 5), (5, 8), (8, 11),
        (11, 14), (14, 17), (17, 20), (3, 6), (6, 9), (9, 12),
        (12, 15), (15, 18), (18, 21)
    ]

    lam = ones(Float64, length(edges))
    x = MGT.gaplas(y, edges, lam, max_iter=3)

end

end
