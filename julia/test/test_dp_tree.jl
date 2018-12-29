module TestDPTree
include("../src/dp_tree.jl")

using Test
import .DPTree

@testset "dp_tree: 10                    " begin
    root = 1
    pi = Int[0, 0, 1, 2, 3, 0, 7, 8, 3, 8] .+ 1
    n = length(pi)
    t = DPTree.Tree(root, pi)
    y = Float64[8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0

    x = DPTree.dp_tree(y, lam, t)
    @test size(x) == size(y)
    @test 3x ≈ [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2]
end

end
