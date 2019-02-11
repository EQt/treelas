struct WeightedGraph
    idx::NeighborIndex
    lam::Vector{Float64}
end


num_nodes(g::WeightedGraph) =
    num_nodes(g.idx)

num_edges(g::WeightedGraph) =
    num_edges(g.idx)


function enumerate_edges(f::Function, g::WeightedGraph)
    local ei::Int = 1
    for v = 1:num_nodes(g)
        for u in g.idx[v]
            f(ei, v, u, g.lam[ei])
            ei += 1
        end
    end
end
