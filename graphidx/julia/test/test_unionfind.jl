include("../src/utils/unionfind.jl")

using Test


@testset "UnionFind, mini_test_2         " begin
    u = UnionFind(2)
    @test find(u, 1) == 1
    @test find(u, 2) == 2

    unite!(u, find(u, 1), find(u, 1))
    @test find(u, 1) == 1
    @test find(u, 2) == 2

    unite!(u, find(u, 1), find(u, 2))
    @test find(u, 1) == find(u, 2)
end


@testset "UnionFind, mini_test_3         " begin
    u = UnionFind(3)
    @test find(u, 1) == 1
    @test find(u, 2) == 2
    @test find(u, 3) == 3

    unite!(u, find(u, 3), find(u, 1))

    @test find(u, 3) == find(u, 1)
    @test find(u, 2) != find(u, 1)
    @test find(u, 2) != find(u, 3)

    unite!(u, find(u, 2), find(u, 1))
    @test length(Set(find(u, i) for i = 1:3)) == 1
end


@testset "UnionFind, mini_test_4         " begin
    u = UnionFind(4)
    for i = 1:4
        @test find(u, i) == i
    end

    unite!(u, find(u, 4), find(u, 1))

    @test find(u, 4) == find(u, 1)
    @test find(u, 3) != find(u, 4)
    @test find(u, 3) != find(u, 1)

    unite!(u, find(u, 4), find(u, 3))

    @test find(u, 4) == find(u, 1)
    @test find(u, 3) == find(u, 4)
    @test find(u, 3) == find(u, 1)
end
