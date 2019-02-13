struct WeightedGraph
    idx::NeighborIndex
    lam::Vector{Float64}
end


num_nodes(g::WeightedGraph) =
    num_nodes(g.idx)

num_edges(g::WeightedGraph) =
    num_edges(g.idx)


function enumerate_edges(f::Function, g::WeightedGraph)
    for v in 1:num_nodes(g)
        for (u, ei) in g.idx[v]
            f(ei, v, u, g.lam[ei])
        end
    end
end
