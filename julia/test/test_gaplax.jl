"""
Test the eXtension of the GapLas/MGT idea
"""
module TestGapLaX

using Test
import GraphIdx
import GraphIdx: num_edges, NeighborIndex
import TreeLas


function sort_edges(nidx::NeighborIndex)
    local m = num_edges(nidx)
    edge_rank = zeros(Int, 2m)
    for (i, (v, ei)) in enumerate(nidx.pi)
        if edge_rank[2ei-1] == 0
            edge_rank[2ei-1] = i
        else
            edge_rank[2ei-0] = i
        end
    end
    return edge_rank
end


function non_tree_idx(nidx::NeighborIndex, selected)
    m = num_edges(nidx)
    nonidx = deepcopy(nidx)
    edge_rank = sort_edges(nidx)

    for (i, s) in enumerate(selected)
        s <= 0 && continue
        ri, rj = edge_rank[2s-1], edge_rank[2s-0]
        (u, ei) = nonidx.pi[ri]
        (v, ej) = nonidx.pi[rj]
        @assert ei == ej
        nonidx.pi[ri] = (u, 0)
        nonidx.pi[rj] = (v, 0)
    end
    error("not finished")
    return nonidx
end


function non_tree_idx(n::Int, edges::Vector{E}, selected) where {E}
    edges = copy(edges)
    for s in selected
        if s > 0
            edges[s] = (0, 0)
        end
    end
    return NeighborIndex(n, edges)
end


@testset "GapLax: demo 3x7               " begin

    grid = GraphIdx.Grid.GridGraph(3, 7)
    edges, lam = GraphIdx.Grid.collect_edges(grid)
    y = [0.62 0.73 0.71 1.5 1.17 0.43 1.08
         0.62 1.73 0.95 1.46 1.6 1.16 0.38
         0.9 0.32 -0.48 0.95 1.08 0.02 0.4]
    n, m = length(y), length(edges)
    root_node = 1
    α = zeros(Float64, m)
    γ = Vector{Float64}(undef, m)
    TreeLas.Dual.gap_vec!(γ, y, α, grid, -1.0)
    nidx = GraphIdx.NeighborIndex(n, edges)
    pmem = GraphIdx.PrimMstMem(nidx)
    pi = GraphIdx.prim_mst_edges(γ, root_node, pmem)
    selected = pmem.selected
    cidx = GraphIdx.Tree.ChildrenIndex(pi)

    @assert selected[1] == -1
    non_tree_edges = edges[selected[2:end]]
    lcas = GraphIdx.Tree.lowest_common_ancestors(cidx, pi, non_tree_edges)

    @test sortperm(nidx.pi, by=e -> e[2]) == sort_edges(nidx)

    nonidx = non_tree_idx(n, edges, selected)
    @test num_edges(nonidx) == m - n + 1
end


end
