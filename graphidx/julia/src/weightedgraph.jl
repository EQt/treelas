import .Tree


struct WeightedGraph
    idx::NeighborIndex
    lam::Vector{Float64}
end


num_nodes(g::WeightedGraph) =
    num_nodes(g.idx)

num_edges(g::WeightedGraph) =
    num_edges(g.idx)


function enumerate_edges(f::Function, g::WeightedGraph)
    for u in 1:num_nodes(g)
        for (v, ei) in g.idx[u]
            if u < v
                f(ei, u, v, g.lam[ei])
            end
        end
    end
end


function enumerate_edges(f::Function, t::Tree.WeightedTree{W}) where {W}
    for (u, v) in enumerate(t.tree.parent)
        if u == t.tree.root
            continue
        end
        f(u, u, v, t.lam(u))
    end
end



include("weights.jl")
