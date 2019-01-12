include("heap.jl")
include("neighbors.jl")

import .Heap: PriorityQueue, dequeue!


"""
    minimum_spantree_edges(g, weights, [root = 1])

[Prim's algorithm](https://en.wikipedia.org/wiki/Prim%27s_algorithm)
for minimum spanning tree.
Start from node `root` (will become the root node of the spanning
tree).  Return the parent vector and a Boolean vector indicating for
each edge whether the edge is part of the spanning tree.
"""
function minimum_spantree_edges(n, edges, weights, root = 1)
    selected = Vector{Int}(undef, n)
    finished, dist, parent, neighbors, pq = _init_spantree(edges, n)
    _minimum_spantree_edges(weights, finished,dist, parent,
                            neighbors, selected, pq, root)
    # swap root element to front
    selected[1], selected[root] = selected[root], selected[1]
    return parent, view(selected, 2:length(selected))
end


function _init_spantree(edges, n)
    m = length(edges)
    finished = Vector{Bool}(undef, n)
    dist = Vector{Float64}(undef, n)
    parent = Vector{Int}(undef, n)
    neighbors = NeighborIndex(n, edges)
    pq = PriorityQueue{Int, Float64}(n)
    return finished, dist, parent, neighbors, pq
end


function _minimum_spantree_edges(edge_weight, finished, dist,
                                 parent::Vector{Int},
                                 neighbors::NeighborIndex,
                                 selected::Vector{Int},
                                 pq::PriorityQueue{Int, Float64},
                                 root::Int = 1)::Vector{Int}
    n = length(parent)
    @assert length(selected) == n
    @assert isempty(pq)
    sizehint!(pq, n)

    # see CLRS page 572 (2nd edition)
    finished .= false
    finished[root] = true
    selected[root] = -1
    dist .= typemax(Float64)
    dist[root] = typemin(Float64)
    parent[root] = root
    pq[root] = dist[root]
    while !isempty(pq)
        u = dequeue!(pq)
        for (v, eidx) in neighbors[u]
            v == u && continue
            if !finished[v] && edge_weight[eidx] < dist[v]
                dist[v] = edge_weight[eidx]
                pq[v] = dist[v]    # decrease_key!(v)
                parent[v] = u
                selected[v] = eidx
            end
        end
        finished[u] = true
    end
    return parent
end


#=
"""
    minimum_spantree(n, edges, weights, [root = 1])

[Prim's algorithm](https://en.wikipedia.org/wiki/Prim%27s_algorithm)
for minimum spanning tree.
Start from node `root` (will become the root node of the spanning tree).
Return the parent vector of the tree.
"""
function minimum_spantree(n, edges, weights, root = 1)::Vector{Int}
    finished, dist, parent, neighbors, pq = _init_spantree(edges, n)
    return _minimum_spantree(weights, finished,
                             dist, parent, neighbors, pq, root)
end

function _minimum_spantree(weights, finished, dist, parent, neighbors,
                           pq::PriorityQueue{Int, Float64}, root = 1)
    @assert isempty(pq)
    sizehint!(pq, length(parent))
    finished .= false
    finished[root] = true
    dist .= Inf
    dist[root] = 0.0
    parent[root] = root
    pq[root] = dist[root]
    while !isempty(pq)
        u = dequeue!(pq)
        for (v, eidx) in neighbors[u]
            v == u && continue
            if !finished[v] && weights[eidx] < dist[v]
                dist[v] = weights[eidx]
                pq[v] = dist[v]    # decrease_key!(v)
                parent[v] = u
            end
        end
        finished[u] = true
    end
    return parent
end
=#
