module TestRoot
using Test

include("../src/tree/root.jl")

@testset "root:  tree10                  " begin
    pi = Int[0, 0, 1, 2, 3, 0, 7, 8, 3, 8] .+ 1
    @test find_root(pi) == 1
end

end


