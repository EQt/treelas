module TestRotate

import GraphIdx
using Test

g = GraphIdx.Grid.GridGraph(2, 3)
GraphIdx.enumerate_edges(g) do i, u, v, lam println(v, " ", u) end

pi = [1, 1, 1, 1, 3, 3, 5]
n = GraphIdx.num_nodes(g)
m = GraphIdx.num_edges(g)
non_tree = Vector{Tuple{Int,Int}}(undef, m - n + 1)
@test length(non_tree) == 2

end
