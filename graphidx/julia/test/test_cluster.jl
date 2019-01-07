module TestCluster
using Test

include("../src/cluster.jl")

@testset "Partition 1112                 " begin
    nums = [1, 1, 1, 2]
    @test all(nums .> 0)
    p = Cluster.PartitionIndex(nums)
    @test length(p) == 2
    @test Set(p[1]) == Set([1, 2, 3])
    @test Set(p[2]) == Set([4])
end


@testset "Test Partition5                " begin
    nums = [2, 3, 1, 1, 3]
    @test all(nums .> 0)
    p = Cluster.PartitionIndex(nums)
    @test length(p) == 3
    for i in 1:3
        @test Set(findall(nums .== i)) == Set(p[i])
    end
end


@testset "Test Cluster                   " begin
    neigh = Dict(1 => [2, 3],
                 2 => [1, 4],
                 3 => [1, 4],
                 4 => [2, 3])
    x = [ 0.0  0.1
         -0.1  1.1]
    @test size(x) == (2,2)
    p = Cluster.cluster(x, neigh, eps=0.2)
    @test length(p) == 2
    @test isperm(p.V)
    @test Set(p[1]) == Set([4])
    @test Set(p[2]) == Set([1,2,3])
end

end
