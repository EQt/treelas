using Test
import GraphIdx.Stats: weighted_median


@testset "unweighted median              " begin
    @test weighted_median([13]) == (13, 13)
    @test weighted_median([13, -42]) == (-42, 13)
    @test weighted_median([1, 3.9, 50, 13]) == (3.9, 13)
    @test weighted_median([1, 3.9, 50, 13, 5]) == (5, 5)
end


@testset "weighted median                " begin
    @test weighted_median([1, 2, 3, 4, 5], [.15, .1, .2, .3, .25]) == (4, 4)
    @test weighted_median([1, 2, 3, 4, 5], [.15, .25, .2, .3, .1]) == (3, 3)
    @test weighted_median([1, 2, 3, 4, 5], [1, 1, 1, 2, 3]) == (4, 4)
    @test weighted_median([1, 2, 3, 4, 5], [1, 1, 1, 1, 5]) == (5, 5)
    @test weighted_median([1, 2, 3, 4, 5], [1, 4, 1, 1, 5]) == (3, 4)
    @test weighted_median([1, 2, 3, 4, 5], [2, 4, 2, 1, 5]) == (3, 3)
end


@testset "weighted median + function     " begin
    @test weighted_median([1, 3, 2], [1, 3, 1]) == (3, 3)
    @test (2, 2) == weighted_median([1, 3, 2], [1, 3, 1], x -> x)
    @test (1, 1) == weighted_median([1, 3, 2], [1, 3, 1], x -> 5x)
    @test (3, 3) == weighted_median([1, 3, 2], [1, 3, 1], x -> x-10)
end
