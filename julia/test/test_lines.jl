module TestLines

include("data.jl")

using Test
import Pkg: TOML
import TreeLas
import GraphIdx


function instance(line; to_tree::Bool = false)
    y = Float64.(line["y"])
    n = length(y)
    mu = if haskey(line, "mu")
        GraphIdx.Weights(Float64.(line["mu"]))
    else
        GraphIdx.Weights(Float64)
    end
    lam = Float64.(line["lam"])
    if to_tree && lam isa Array
        prepend!(lam, NaN)
    end
    lam = GraphIdx.Weights(lam)
    return y, lam, mu
end


const lines_toml = data_dir("test", "lines.toml")
lines = TOML.parsefile(lines_toml)["test"]


@testset "TreeLas LineLas                " begin
    @test length(lines) >= 5
    for (i, line) in enumerate(lines)
        y, lam, mu = instance(line, to_tree=false)
        n = length(y)
        @testset "line_las[$i]: n = $n" begin
            # @info "\nLine[$i]"
            @test TreeLas.LineDP.line_las(y, lam, mu) ≈ line["x"]
        end
    end
end


@testset "TreeLas line by trees          " begin
    for (i, line) in enumerate(lines)
        y, lam, mu = instance(line, to_tree=true)
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
