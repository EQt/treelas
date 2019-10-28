module TestRotate

import GraphIdx
import GraphIdx.Tree: ChildrenIndex, hierarchy_string, lowest_common_ancestors
using Test

@testset "Rotate Grid(2, 3)               " begin
g = GraphIdx.Grid.GridGraph(2, 3)
if false
    GraphIdx.enumerate_edges(g) do _, u, v, _
        println(u, " ", v)
    end
end

pi = [1, 1, 1, 3, 3, 5]
cidx = ChildrenIndex(pi)
@test hierarchy_string(cidx) == """
1
├─2
└─3
  ├─4
  └─5
    └─6
"""
n = GraphIdx.num_nodes(g)
m = GraphIdx.num_edges(g)
non_tree = Vector{Pair{Int,Int}}()
sizehint!(non_tree, m - n + 1)
GraphIdx.enumerate_edges(g) do _, u, v, _
    if !(pi[u] == v || pi[v] == u)
        push!(non_tree, u => v)
    end
end
@test non_tree == [2 => 4, 4 => 6]
lca = lowest_common_ancestors(cidx, pi, non_tree)
@test lca == [1, 3]
lam = Float64.(collect(1:m))

end

end
