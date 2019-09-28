module TestLines

using Test
import Pkg: TOML
import TreeLas
import GraphIdx


function instance(line)
    y = Float64.(line["y"])
    n = length(y)
    mu = GraphIdx.create_weights(Float64.(get(line, "mu", 1.0)))
    lam = Float64.(line["lam"])
    if lam isa Array
        prepend!(lam, NaN)
    end
    lam = GraphIdx.create_weights(lam)
    return y, lam, mu
end


const lines_toml = joinpath(dirname(pathof(TreeLas)),
                            "..", "..", "data", "test", "lines.toml")
lines = TOML.parsefile(lines_toml)["test"]


@testset "TreeLas LineLas                " begin
    @test length(lines) >= 5
    for (i, line) in enumerate(lines[1:1])
        y, lam, mu = instance(line)
        n = length(y)
        @testset "line_las[$i]: n = $n" begin
            @test TreeLas.LineDP.line_las(y, lam, mu) ≈ line["x"]
        end
    end
end


@testset "TreeLas line by trees          " begin
    for (i, line) in enumerate(lines)
        y, lam, mu = instance(line)
        n = length(y)
        @testset "Line[$i]: n = $n" begin
            parent = collect(0:n-1)
            parent[1] = 1
            t = GraphIdx.Tree.RootedTree(1, parent)
            @test mu[1] != 0.0
            @test length(parent) == n
            @test TreeLas.TreeDP.tree_dp(y, t, lam, mu) ≈ line["x"]
        end
    end
end


end
