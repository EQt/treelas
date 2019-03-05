module TestTreeDP

using Test
import Printf: @sprintf
import TreeLas
import TreeLas.TreeDP
import TreeLas: dual
import GraphIdx
import GraphIdx.Tree: ChildrenIndex, hierarchy, hierarchy_with

include(joinpath(dirname(pathof(TreeLas)), "cxx.jl"))


mean(x) = sum(x) / length(x)


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

    local tree = GraphIdx.Tree.RootedTree(root, parent)
    local dp_mem = TreeDP.TreeDPMem(n)
    local x = fill(NaN, size(y)...)
    local tree_alpha = fill(NaN, n)
    let mu = 1.0, tlam = ones(Float64, n), z = copy(y)
        TreeDP.tree_dp!(x,
                        z,
                        tree,
                        TreeDP.ArrayWeights(tlam),
                        TreeDP.ConstantWeights(mu),
                        dp_mem)
        @test !any(isnan.(x))
    end

    @testset "analyse solution" begin
        @test length(unique(x)) == 2
        @test mean(y) ≈ mean(x)
    end

    @testset "compare to cxx tree_dp" begin
        cxx_x = Cxx.cxx_tree_dp(y, parent, root, 1.0)
        @test cxx_x ≈ x
    end

    @testset "dual!" begin
        @test dp_mem.proc_order[end] == root
        @test dp_mem.proc_order[1:end-1] ==
            [12, 11, 19, 20, 21, 14, 15, 18, 17, 16, 13, 10, 7, 8, 9, 3, 6, 2, 5, 4]
        TreeLas.dual!(tree_alpha, x, copy(y), dp_mem.proc_order, parent)
        local alpha = vec(x) - vec(y)
        for i in @view dp_mem.proc_order[1:end-1]
            alpha[parent[i]] += alpha[i]
        end
        if !(alpha[2:end] ≈ tree_alpha[2:end])
            @show alpha - tree_alpha
            @test alpha[2:end] ≈ tree_alpha[2:end]
        end
        exceed = findall(abs.(tree_alpha) .> 1 + 1e12)
        if exceed != []
            for ei in exceed
                @show ei
                @show alpha[ei]
            end
        end
    end

    @testset "compare to tree_dual" begin
        cxx_alpha = Cxx.cxx_tree_dual!(x - copy(y), parent, root)
        @test abs.(tree_alpha[2:end]) ≈ abs.(cxx_alpha[2:end])
        sign01(x) = Int((sign(x) + 1) / 2)
        if !(sign01.(tree_alpha[2:end]) == sign01.(cxx_alpha[2:end]))
            hierarchy(ChildrenIndex(parent))
            @test sign01.(tree_alpha[2:end]) == sign01.(cxx_alpha[2:end])
        end
        @test tree_alpha[2:end] ≈ cxx_alpha[2:end]
    end

    local wtree = GraphIdx.Tree.WeightedTree(tree, TreeDP.ConstantWeights(1.0))
    let gam = fill(NaN, n)
        TreeLas.MGT.gap_vec!(gam, x, tree_alpha, wtree)
        @test isnan(gam[root])
        gam[root] = 0.0
        wrong_ids = [i for i in 1:n if abs(gam[i]) > 1e-16]
        if wrong_ids != []
            for i in wrong_ids
                @show i, parent[i]
                @show x[i], x[parent[i]]
                @show tree_alpha[i]
                @show gam[i]
            end
            hierarchy(ChildrenIndex(parent))
            @test wrong_ids == []
        end
    end
end

end
end
