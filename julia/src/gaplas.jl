"""
# Gap Tree Lasso

Re-implement the MGT module for general Graphs having only a method to
access all edges.  One crucial part question is how to provide access
to a `NeighborIndex` or alike.
"""
module GapLas

include("gap.jl")

import GraphIdx.Grid: enumerate_edges
import GraphIdx: NeighborIndex, neighbors_lambda
import GraphIdx: PrimMstMem, prim_mst_edges
import ..TreeDP: TreeDPMem, tree_dp!, reset!



end
