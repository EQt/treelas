module TestInOrder
using Test

include("../src/inorder.jl")


@testset "Inorder                        " begin
    @test binary_inorder(1) == [0]
    @test binary_inorder(2) == [1, 0, 2]
    @test binary_inorder(3) == [3, 1, 4, 0, 5, 2, 6]
    @test binary_inorder(4) == [7, 3, 8, 1, 9, 4, 10, 0, 11, 5, 12, 2, 13, 6, 14]
end


end
