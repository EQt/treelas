using Test
import TreeLas.Dual: dual!
import Random: MersenneTwister


@testset "dual: test_dual3               " begin
    root = 1
    local parent = Int[1, 1, 1]
    z = Float64[-3, 1, 2]
    proc_order = Int[2, 3, 1]
    @test sum(z) ≈ 0
    alpha = copy(z)
    dual!(alpha, proc_order, parent, NaN)
    @test isnan(alpha[root])
    @test alpha[2:end] == [1, 2]
end


@testset "dual: cumsum                   " begin
    n = 10
    rseed() = MersenneTwister(42)
    pi = collect(0:(n-1))
    pi[1] = 1
    post_order = n:-1:1 |> collect
    z = randn(rseed(), n)
    z .-= sum(z)/n
    @test sum(z) <= eps() * n
    alpha = copy(z)
    dual!(alpha, post_order, pi)
    @test alpha ≈ reverse!(cumsum(reverse(z)))
end
