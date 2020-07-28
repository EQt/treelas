module TestCycle3x7


import GraphIdx
import GraphIdx: GraphViz, Const, Graph
import GraphIdx.Grid: GridGraph
import TreeLas: Cycle
import TreeLas: CycleGap

include("demo3x7.jl")

lambda = Const(1.0)
graph = GridGraph(size(y)...)
cmem = CycleGap.CycMem(y, graph, lambda);
mem = cmem.gap_mem;
CycleGap.gap_vec!(mem.gamma, mem.x, mem.alpha, mem.wgraph, -1.0)
CycleGap.prim_mst_edges(mem.gamma, mem.tree.root, mem.mst)
cmem.cycles = Cycle.CycleBasis(graph, mem.mst.parent)
tlam0, tlam = Cycle.extract_rotate(cmem.cycles, lambda)
cycles = cmem.cycles


function GraphViz.dot(graph::GridGraph, tree::GraphIdx.Tree.RootedTree)
    open(`dot -Tx11 -Kneato`, "w") do io::IO
        GraphViz.dot(io, graph) do io::IO
            for (i, v) in enumerate(tree.parent)
                if i != v
                    println(io, i, " -> ", v)
                end
            end
        end
    end
end
    

function GraphViz.dot(
    graph::GridGraph, cycles::Cycle.CycleBasis, tree_label = i -> ""
)
    open(`dot -Tx11 -Kneato`, "w") do io::IO
        GraphViz.dot(io, graph) do io::IO
            println(io, "{ edge [color=black]")
            for (i, v) in enumerate(cycles.pi)
                if i != v
                    println(io, i, " -> ", v, tree_label(i))
                end
            end
            println(io, "}")
            Cycle.enumerate_non_tree(cycles) do ei::Int, u::Int, v::Int, r::Int
                if u < v
                    local col = 1 + (ei % 8)
                    tstyle = " [ color=$(col) ]"
                    println(io, u, " -> ", v , " [style=dotted, color=", col, "]")
                    for i in [u, v]
                        while i != r
                            local pi = cycles.pi[i]
                            println(io, i, " -> ", pi, tstyle)
                            i = pi
                        end
                    end
                end
            end
        end
    end
end

GraphViz.dot(graph, cycles, i -> "[ label=\"$(Int(tlam[i]))\"]")



end#module
