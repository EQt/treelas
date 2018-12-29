module TestTreeOrder
using Test

include("../src/graphidx/order.jl")

@testset "order: tree10                  " begin
    pi = Int[0, 0, 1, 2, 3, 0, 7, 8, 3, 8] .+ 1
    @test find_root(pi) == 1
end

end


