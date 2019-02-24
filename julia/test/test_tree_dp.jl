module TestTreeDP

using Test
import Printf: @sprintf
import TreeLas.TreeDP
import TreeLas: dual
import GraphIdx.Tree: ChildrenIndex, hierarchy, hierarchy_with


@testset "TreeDP                         " begin

@testset "tree_dp: tree.mini.h5          " begin
    t = TreeDP.Tree([1, 1, 2, 1, 2, 3])
    y = [0.53 0.45 0.02
         0.17 0.96 0.97]
    y = Array(y')
    lam = 0.2
    x = TreeDP.tree_dp(y, t, lam)
    @test size(x) == size(y)
    xt = [0.4  0.4  0.4
          0.37 0.76 0.77]
    xt = Array(xt')
    if false
        hierarchy_with(ChildrenIndex(t.parent)) do i
            @sprintf("%.2f:%.2f", y[i], xt[i])
        end
    end
    @test x ≈ xt
end


@testset "tree_dp: 10                    " begin
    t = TreeDP.Tree(Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9])
    y = Float64[8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0

    x = TreeDP.tree_dp(copy(y), t, lam)
    @test size(x) == size(y)
    expect3x = [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2]
    @test 3x ≈ expect3x
    local cidx = ChildrenIndex(t.parent)
    if true
        let z = expect3x/3 - y
            local alpha = dual(z, t.parent, cidx)
            # @info "alpha=" *string(round.(alpha, digits=4))
            hierarchy(cidx, i -> @sprintf("%.1f≈%+.1f", -3y[i]+expect3x[i],
                                          -3alpha[i]))
        end
    end
end

end
end
