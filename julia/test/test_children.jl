module TestChildren
using Test

include("../src/children.jl")

@testset "generate(width=5): low level   " begin
    # root, parent = TreeInstance.generate(5)[3:4]
    root = 1
    parent = Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9]
    pi_, idx_ = _compute_children(parent, root)
    @test pi_ == [1, 2, 6, 3, 4, 5, 9, 7, 8, 10]
    @test idx_ == [2, 4, 5, 6, 8, 8, 8, 8, 9, 11, 11]
end


@testset "generate(width=5): index access" begin
    # root, parent = TreeInstance.generate(5)[3:4]
    root = 1
    parent = Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9]
    cidx = children_index(parent)
    @test cidx[1] == [2,6]
    @test cidx[2] == [3]
    @test cidx[9] == [8, 10]
    @test cidx[4] == [5, 9]
    @test cidx[10] == []
    @test cidx[7] == []
end


@testset "generate(width=21): some tests " begin
    # pi = TreeInstance.generate(21)[4]
    pi = [1, 1, 2, 3, 4, 7, 8, 29, 8, 9, 32, 33, 34, 35, 36, 37, 16,
    19, 40, 21, 42, 1, 22, 25, 4, 25, 26, 27, 28, 29, 10, 31, 34, 55,
    56, 57, 36, 17, 18, 41, 20, 63, 64, 23, 44, 67, 48, 69, 28, 29,
    72, 51, 52, 33, 76, 55, 56, 57, 38, 59, 60, 63, 84, 65, 86, 65,
    66, 47, 70, 49, 70, 73, 74, 95, 74, 75, 98, 99, 78, 59, 80, 81,
    104, 105, 86, 87, 108, 67, 68, 111, 92, 71, 92, 93, 94, 95, 76,
    97, 98, 79, 122, 103, 82, 103, 104, 127, 108, 109, 110, 89, 110,
    91, 114, 93, 114, 115, 138, 117, 98, 119, 120, 121, 144, 103, 146,
    105, 128, 107, 130, 131, 110, 131, 134, 155, 134, 137, 116, 137,
    138, 141, 142, 121, 164, 145, 124, 145, 126, 127, 128, 149, 152,
    153, 132, 153, 154, 155, 156, 137, 138, 159, 140, 163, 142, 163,
    166, 145, 146, 147]

    cidx = children_index(pi)
    @test cidx[162] == []
    @test Set(cidx[137]) == Set([138, 136, 158])
    @test Set(cidx[1]) == Set([2, 22])
    @test cidx[89] == [110]
end

#=
@testset "generate(width=725)" begin
    parent = TreeInstance.generate(725)[4]
    n = length(parent)
    @test all(parent .>= 1)
    @test all(parent .<= n)
    deg = node_degrees(parent)
    println(deg[end])
    cidx = children_index(parent)
end
=#

end
