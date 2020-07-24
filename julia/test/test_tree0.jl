module TestTree0

using Test
import Pkg: TOML
import TreeLas
import GraphIdx

include("data.jl")

function readtoml(fname::String; nan="-1.0")
    txt = read(open(fname), String)
    txt = replace(txt, "nan" => nan)
    TOML.parse(txt)
end


function instance(tree)
    y = Float64.(tree["y"])
    n = length(y)
    mu = GraphIdx.Weights(Float64.(get(tree, "mu", 1.0)))
    pi = Int.(tree["parent"]) .+ 1
    root::Int = get(tree, "root", -1) + 1
    if root <= 0
        root = GraphIdx.Tree.find_root(pi)
    end
    lam = Float64.(tree["lam"])
    if lam isa Array
        @assert findall(lam .< 0) == [root] "$(typeof(lam)): $lam, $root"
        lam[root] = NaN
    end
    lam = GraphIdx.Weights(lam)
    t = GraphIdx.Tree.RootedTree(root, pi)
    return y, t, lam, mu
end


@testset "tree0.toml                     " begin
    for (i, ti) in enumerate(readtoml(data_dir("test", "tree0.toml"))["tree"])
        @testset "tree0.toml[$i]" begin
            y, t, lam, mu = instance(ti)
            @test TreeLas.TreeDP.tree_dp(y, t, lam, mu) ≈ ti["x"]
        end
    end
end


@testset "tree0.1.toml                   " begin
    ti = readtoml(data_dir("test", "tree0.1.toml"))["tree"][1]
    y, t, lam, mu = instance(ti)
    x = TreeLas.TreeDP.tree_dp(y, t, lam, mu)
    @test all(isfinite.(x))
    @test minimum(y) <= minimum(x)
    @test maximum(x) <= maximum(y)
end


end
