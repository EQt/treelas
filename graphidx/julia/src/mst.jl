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
    sortperm!(order, weight, rev=false)  # , alg=Base.Sort.DEFAULT_STABLE)
    init(unfi)
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


function _minimum_spantree_edges(weights, finished, dist,
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
            if !finished[v] && weights[eidx] < dist[v]
                dist[v] = weights[eidx]
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
