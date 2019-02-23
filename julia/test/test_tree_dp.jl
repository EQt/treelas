module TestTreeDP

using Test
import Printf: @sprintf
import TreeLas.TreeDP
import GraphIdx.Tree: ChildrenIndex, hierarchy


@testset "tree_dp: tree.mini.h5          " begin
    t = TreeDP.Tree([1, 1, 2, 1, 2, 3])
    y = [0.53 0.45 0.02
         0.17 0.96 0.97]
    lam = 0.2
    x = TreeDP.tree_dp(y, t, lam)
    @test size(x) == size(y)
    @test x ≈ [0.4  0.4  0.4
               0.37 0.76 0.77]
end


@testset "tree_dp: 10                    " begin
    t = TreeDP.Tree(Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9])
    y = Float64[8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0

    x = TreeDP.tree_dp(y, t, lam)
    @test size(x) == size(y)
    expect3x = [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2]
    @test 3x ≈ expect3x
    hierarchy(ChildrenIndex(t.parent),
              i ->
            # @sprintf("%.1f=%.1f-%.1f", 3y[i] - expect3x[i], 3y[i], expect3x[i]))
              @sprintf("%.1f:%.1f", 3y[i], expect3x[i]))
end

end
