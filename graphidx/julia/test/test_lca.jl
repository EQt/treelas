include("../src/lca.jl")

using Test
import .Tree: lowest_common_ancestors, ChildrenIndex

@testset "LCA NeighborIndex              " begin
    pairs = [(5, 7), (5, 2), (5, 3), (2, 3)]
end


@testset "LCA Wikipedia                  " begin
    #     1  2  3  4  5  6  7
    pi = [1, 1, 1, 2, 4, 4, 6]
    #=
         1
        / \
       /   \
      2     3
      |
      4
     / \
    5   6
        |
        7
    =#
    idx = ChildrenIndex(pi)
    @test Set(idx[1]) == Set([2, 3])
    @test Set(idx[2]) == Set([4])
    @test Set(idx[3]) == Set([])
    @test Set(idx[4]) == Set([5, 6])
    @test Set(idx[5]) == Set([])
    @test Set(idx[6]) == Set([7])
    @test Set(idx[7]) == Set([])
    @test length(idx) == 7
    pairs = [(5, 7), (5, 3), (2, 3), (5, 2)]
    ans = [4, 1, 1, 2]
    @test length(pairs) == length(ans)
    @test lowest_common_ancestors(idx, pi, pairs) == ans
end

