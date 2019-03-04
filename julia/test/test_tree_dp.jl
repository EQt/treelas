module TestTreeDP

using Test
import Printf: @sprintf
import TreeLas.TreeDP
import TreeLas: dual
import GraphIdx;
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


@testset "MGT     : demo 3x7             " begin
    y = [0.62 0.73 0.71 1.5 1.17 0.43 1.08
         0.62 1.73 0.95 1.46 1.6 1.16 0.38
         0.9 0.32 -0.48 0.95 1.08 0.02 0.4]
    local n = length(y)
    local root = 1
    edges = Tuple{Int64,Int64}[
        (1,  2),  (4,  5),  (7, 8), (10, 11), (13, 14), (16, 17),
        (19, 20), (2,  3),  (5, 6), (8,   9), (11, 12), (14, 15),
        (17, 18), (20, 21), (1, 4), (4,   7), (7,  10), (10, 13),
        (13, 16), (16, 19), (2, 5), (5,   8), (8,  11), (11, 14),
        (14, 17), (17, 20), (3, 6), (6,   9), (9,  12), (12, 15),
        (15, 18), (18, 21)
    ]

    γ = [-abs(y[u] - y[v]) for (u, v) in edges]

    parent =
        [1, 5, 6, 1, 4, 5, 8, 9, 6, 7, 8, 9, 10, 15, 18, 13, 16, 17, 20, 17, 18]

    # local dp_mem = TreeDPMem(n)
    local tree = GraphIdx.Tree.RootedTree(root, parent)
    hierarchy(ChildrenIndex(parent))
end

end
end
