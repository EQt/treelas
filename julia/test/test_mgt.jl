module TestMGT

include("square.jl")

using Test
import TreeLas.MGT
import GraphIdx
import GraphIdx: EdgeGraph
import GraphIdx.Io: prettyarr


Base.sort(i::T, j::T) where {T} =
    i < j ? (i, j) : (j, i)


@testset "MGT     : Square               " begin
    edges, n = square_edges()
    m = length(edges)
    lambda = GraphIdx.Const(0.5)
    y = Float64[0 1; 0 0.5]
    x = MGT.gaplas(y, edges, lambda)
    @test size(x) == size(y)
    @test sum(x) ≈ sum(y)
    @test x ≈ mean(y) * ones(size(y)...)
end


@testset "MGT     : demo 3x7             " begin
    include("demo3x7.jl")
    n1, n2 = 3, 7
    n = n1 * n2
    m = 32
    root = 1
    s = collect(1:n) |> x -> reshape(x, n1, n2)
    @test prettyarr(s) == """
    3×7 Array{Int64,2}:
     1  4  7  10  13  16  19
     2  5  8  11  14  17  20
     3  6  9  12  15  18  21
    """ |> chomp
    @test size(y) == (n1,n2)

    edges = GraphIdx.Edge{Int}[
        (1,  2),  (4,  5),  (7, 8), (10, 11), (13, 14), (16, 17),
        (19, 20), (2,  3),  (5, 6), (8,   9), (11, 12), (14, 15),
        (17, 18), (20, 21), (1, 4), (4,   7), (7,  10), (10, 13),
        (13, 16), (16, 19), (2, 5), (5,   8), (8,  11), (11, 14),
        (14, 17), (17, 20), (3, 6), (6,   9), (9,  12), (12, 15),
        (15, 18), (18, 21)
    ]

    @test 7*2 + 6*3 == m
    @test length(edges) == m
    
    γ = [-0.0,  -1.0,  -0.24, -0.04, -0.43, -0.73,
         -0.7,  -0.28, -1.41, -1.43, -0.51, -0.52,
         -1.14, -0.02, -0.11, -0.02, -0.79, -0.33,
         -0.74, -0.65, -1.11, -0.78, -0.51, -0.14,
         -0.44, -0.78, -0.58, -0.80, -1.43, -0.13,
         -1.06, -0.38]

    @testset "compute γ manually" begin
        gam = [-abs(y[u] - y[v]) for (u, v) in edges]
        @test γ ≈ gam
    end

    tree_mask = GraphIdx.kruskal_mst(n, edges, γ)
    tree_cost = γ[tree_mask]
    tree_edges = edges[tree_mask]
    idx = GraphIdx.IncidenceIndex(n, edges)
    local parent::Vector{Int}, selected =
        GraphIdx.prim_mst_edges(n, edges, γ, root)

    @test GraphIdx.Tree.find_root(parent) == root
    @test parent[root] == root
    @test parent ==
        [1, 5, 6, 1, 4, 5, 8, 9, 6, 7, 8, 9, 10, 15, 18, 13, 16, 17, 20, 17, 18]
    @test -sum(tree_cost) ≈ 15.79

    prim_edges = [sort(i, j) for (i, j) in enumerate(parent) if i != j]
    @test length(prim_edges) == n-1
    s_prim, s_kruskal = Set(prim_edges), Set(tree_edges)
    prim_mask = [e ∈ s_prim for e in edges]
    prim_cost = γ[prim_mask]
    @test length(s_prim) == n-1
    @test length(s_kruskal) == n-1
    @test -sum(prim_cost) ≈ 15.79

    sdiff = symdiff(s_prim, s_kruskal)
    sdiff_mask = [e ∈ sdiff for e in edges]
    sdiff_cost = Set(γ[sdiff_mask])
    @test length(sdiff_cost) == 1
    @test sdiff_cost == Set([-0.51])

    @test length(selected) == n - 1
    @test Set(edges[selected]) == s_prim

    @testset "extract non-tree" begin
        z = copy(y)
        lambda = GraphIdx.Ones{Float64}()
        tlam = fill(NaN, length(y))
        alpha = zeros(length(edges))
        MGT.extract_non_tree!(EdgeGraph(n, edges), parent, z, alpha, tlam, lambda)
        @test z ≈ y
        @test all(tlam[1:end .!= root] .≈ 1.0)
    end

    @testset "call gaplas" begin
        local lam = GraphIdx.Ones{Float64}()
        x = MGT.gaplas(y, edges, lam, max_iter=5)
        @test mean(x) ≈ mean(y)
    end
end

end
