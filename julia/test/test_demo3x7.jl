module TestTreeDemo3x7

using Test
import Pkg: TOML
import GraphIdx


include("data.jl")

const fname = data_dir("test", "tree_demo3x7.toml")

@testset "Demo 3x7                       " begin
    tree = TOML.parsefile(fname)["tree"][1]
    pi = tree["parent"] .+ 1
    cidx = GraphIdx.Tree.ChildrenIndex(pi)
    @test GraphIdx.Tree.hierarchy_string(cidx) == tree["str"]
end


end
