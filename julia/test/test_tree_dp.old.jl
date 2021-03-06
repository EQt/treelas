module TestDPTree
include("../src/tree_dp.old.jl")

using Test
import .DPTree

@testset "tree_dp: 10                    " begin
    t = DPTree.Tree(Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9])
    y = Float64[8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0

    x = DPTree.dp_tree(y, lam, t)
    @test size(x) == size(y)
    @test 3x ≈ [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2]
end

end
