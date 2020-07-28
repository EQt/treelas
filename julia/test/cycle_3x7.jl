module TestCycle3x7


import GraphIdx
import GraphIdx: GraphViz, Const, Graph
import GraphIdx.Grid: GridGraph
import TreeLas: Cycle
import TreeLas: CycleGap

include("demo3x7.jl")

lam = Const(1.0)
graph = GridGraph(size(y)...)
cmem = CycleGap.CycMem(y, graph, lam);
mem = cmem.gap_mem;
CycleGap.gap_vec!(mem.gamma, mem.x, mem.alpha, mem.wgraph, -1.0)
CycleGap.prim_mst_edges(mem.gamma, mem.tree.root, mem.mst)


function GraphViz.dot(graph::GridGraph, tree::GraphIdx.Tree.RootedTree)
    open(`dot -Tx11 -Kneato`, "w") do io::IO
        GraphViz.dot(io, graph) do io::IO
            println(io, "{ edge [color=1]")
            for (i, v) in enumerate(tree.parent)
                if i != v
                    println(io, i, " -> ", v)
                end
            end
            println(io, "}")
        end
    end
end
    

end#module
