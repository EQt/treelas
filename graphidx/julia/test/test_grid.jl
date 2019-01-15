module TestGrid
include("../src/grid.jl")

using Test
import LinearAlgebra: norm, norm2
import .Grid: Pixel, line_D


@testset "Pixel                          " begin
    p0 = Pixel(0, 0)
    @test norm(p0) == 0.0
    @test norm2(p0) == 0.0

    p1 = Pixel(1, 1)
    @test norm2(p1) == 2.0
    @test norm(p1) ≈ √2

    p3 = Pixel(1, 3)
    @test norm2(p3) == 10
    @test norm(p3)  ≈ √10
end

@testset "line_D                         " begin
    D = line_D(3)
    @test D ≈ [1  -1   0
               0  +1  -1]
    D = line_D(5)
    @test D ≈ [1  -1   0   0   0
               0  +1  -1   0   0
               0   0  +1  -1   0
               0   0   0  +1  -1]
end

end
