include("heap.jl")
include("neighbors.jl")
include("unionfind.jl")

import .Heap: PriorityQueue, dequeue!


# -- Kruskal's minimum spanning tree algorithm ----------------------------

"""
    kruskal_mst(n, edges, weight)

Kruskal's minimum spanning tree algorithm.
Return a vector indicating for each edge whether it is part of the
spanning tree.
"""
function kruskal_mst(n::Int, edges, weight::Vector{Float64})::Vector{Bool}
    @assert length(weight) == length(edges)
    m = length(edges)
    selected, order, uf = Vector{Bool}(undef, m), Vector{Int}(undef, m), UnionFind(n)
    return _kruskal_mst(weight, edges, selected, order, uf)
end


function _kruskal_mst(weight, edges, selected, order, unfi)
    selected .= false
    sortperm!(order, weight, rev=true)
    init(uf)
    for ei in order
        (u, v) = edges[ei]
        fu, fv = find(unfi, u), find(unfi, v)
        if fu != fv
            unite!(unfi, fu, fv)
            selected[ei] = true
        end
    end
    return selected
end

# -- Prim's minimum spanning tree algorithm --------------------------------

function decrease_key!(pq::PriorityQueue{K,V}, k::K, new_val::V) where {K, V}
    pq[k] = new_val
end


"""
    minimum_spantree(n, edges, weights, [root = 1])

Prim's minimum spanning tree algorithm.
Start from node `root` (will become the root node of the spanning tree).
Return the parent vector of the tree.
"""
function minimum_spantree(n, edges, weights, root = 1)::Vector{Int}
    finished, dist, parent, neighbors, pq = _init_spantree(edges, n)
    return _minimum_spantree(weights, finished,
                             dist, parent, neighbors, pq, root)
end


"""
    minimum_spantree_edges(g, weights, [root = 1])

Like `minimum_spantree` but return the parent vector and a Boolean vector
indicating for each edge whether the edge is part of the spanning tree.
"""
function minimum_spantree_edges(n, edges, weights, root = 1)
    selected = Vector{Int}(undef, n)
    finished, dist, parent, neighbors, pq = _init_spantree(edges, n)
    _minimum_spantree_edges(weights, finished,dist, parent, neighbors,
                            selected, pq, root)
    return parent, selected
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
            if !finished[v] && weights[eidx] < dist[v]
                dist[v] = weights[eidx]
                decrease_key!(pq, v, dist[v])
                parent[v] = u
            end
        end
        finished[u] = true
    end
    return parent
end


function _minimum_spantree_edges(weights, finished, dist,
                                 parent::Vector{Int},
                                 neighbors::NeighborIndex,
                                 selected::Vector{Int},
                                 pq::PriorityQueue{Int, Float64},
                                 root::Int = 1)::Vector{Int}
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
        for ii in neighbors.idx[u]:neighbors.idx[u+1]-1
            v, eidx = neighbors.pi[ii]
            if !finished[v] && weights[eidx] < dist[v]
                dist[v] = weights[eidx]
                decrease_key!(pq, v, dist[v])
                parent[v] = u
                selected[v] = eidx
            end
        end
        finished[u] = true
    end
    return parent
end
