module TestGrid
include("../src/grid.jl")

using Test
import LinearAlgebra: norm, norm2
import .Grid: Pixel, line_D, incmat, GridGraph, iter_edges


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

    for n in [11, 13, 42]
        @test line_D(n) == incmat(n, 1)
        @test line_D(n) == incmat(1, n)
    end
end


@testset "GridGraph: 2x3                 " begin
    g = GridGraph(2, 3)
    m = 2*2 + 3
    n = 6
    @test num_nodes(g) == n
    @test num_edges(g) == m

    begin
        local k = 0
        iter_edges(g) do u, v, lam::Float64
            k += 1
            if k == 1
                @test u == 1
                @test v == 2
            elseif k == 4
                @test u == 1
                @test v == 4
            elseif k == 5
                @test u == 2
                @test v == 5
            else
                @test u < v
            end
            @test lam ≈ 1
        end
        @test k == m
    end
end


end
