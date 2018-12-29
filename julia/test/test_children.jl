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


#=
@testset "generate(width=21): some indices" begin
    cidx = children_index(TreeInstance.generate(21)[4])
    @test cidx[162] == []
    @test Set(cidx[137]) == Set([138, 136, 158])
    @test Set(cidx[1]) == Set([2, 22])
    @test cidx[89] == [110]
end

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
