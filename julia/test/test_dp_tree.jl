module TestDPTree
include("../src/dp_tree.jl")

using Test
import .DPTree

@testset "dp_tree: 1                     " begin
    root = 1
    pi = Int[0, 0, 1, 2, 3, 0, 7, 8, 3, 8] .+ 1
    n = length(pi)
    t = DPTree.Tree(root, pi)
end

end
