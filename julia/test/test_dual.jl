using Test
import TreeLas: dual!


@testset "test_dual3                     " begin
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
