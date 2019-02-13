module TestTreeDP

using Test
import TreeLas.TreeDP

@testset "tree_dp: 10                    " begin
    t = TreeDP.Tree(Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9])
    y = Float64[8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0

    x = TreeDP.tree_dp(y, t, lam)
    @test size(x) == size(y)
    @test 3x ≈ [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2]
end

end
