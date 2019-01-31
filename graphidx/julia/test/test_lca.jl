include("../src/lca.jl")

using Test
import .Tree: lowest_common_ancestors, ChildrenIndex

@testset "LCA NeighborIndex              " begin
    pairs = [(5, 7), (5, 2), (5, 3), (2, 3)]
end


@testset "LCA Wikipedia                  " begin
    #     1  2  3  4  5  6  7
    pi = [1, 1, 1, 2, 4, 4, 6]
    idx = ChildrenIndex(pi)
    @test length(idx) == 7
    pairs = [(5, 7), (5, 2), (5, 3), (2, 3)]
    ans = [4, 2, 1, 1]
    @test length(pairs) == length(ans)
    @test lowest_common_ancestors(idx, pi, pairs) == ans
end

