module TestCycle3x7


import GraphIdx
import GraphIdx: GraphViz, Const, Graph
import GraphIdx.Grid: GridGraph
import TreeLas: Cycle
import TreeLas: CycleGap


function show(y, lambda = Const(1.0))
    graph = GridGraph(size(y)...)
    cmem = CycleGap.CycMem(y, graph, lambda);
    mem = cmem.gap_mem;
    CycleGap.gap_vec!(mem.gamma, mem.x, mem.alpha, mem.wgraph, -1.0)
    CycleGap.prim_mst_edges(mem.gamma, mem.tree.root, mem.mst)
    cmem.cycles = Cycle.CycleBasis(graph, mem.mst.parent)
    tlam0, tlam = Cycle.extract_rotate(cmem.cycles, lambda)
    cycles = cmem.cycles

    GraphViz.dot(graph, cycles, i -> "[ label=\"$(Int(tlam[i]))\"]")
end


@info "compiled"
@sync for size in [(3, 7), (4, 6), (4, 6), (2, 30), (2, 30), (2, 30)]
    @async show(randn(size))
end


end#module
