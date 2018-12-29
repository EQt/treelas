include("heap.jl")
include("neighbors.jl")
include("unionfind.jl")

const PriorityQueue = Heap.PriorityQueue
const dequeue! = Heap.dequeue!

# import Heap
# import Heap: PriorityQueue, enqueue!, dequeue!
# import DataStructures: PriorityQueue, enqueue!, dequeue!


# -- Kruskal's minimum spanning tree algorithm ----------------------------

function kruskal_mst(n::Int, edges, weight::Vector{Float64})
    m = length(edges)
    @assert length(weight) == m
    selected, order, uf = _init_kruskal_mst(m, n)
    return _kruskal_mst(m, weight, edges, selected, order, uf)
end


_init_kruskal_mst(m, n) =
    Vector{Int}(), Vector{Int}(undef, m), UnionFind(n)


function _kruskal_mst(m, weight, edges, selected, order, uf)
    empty!(selected)
    sizehint!(selected, m)
    sortperm!(order, weight, rev=true)
    init(uf)
    for ei in order
        e = edges[ei]
        u, v = e.source, e.target
        fu, fv = find(uf, u), find(uf, v)
        if fu != fv
            unite!(uf, fu, fv)
            push!(selected, ei)
        end
    end
    return selected
end

# -- Prim's minimum spanning tree algorithm --------------------------------

function decrease_key!(pq::PriorityQueue{K,V}, k::K, new_val::V) where {K, V}
    pq[k] = new_val
end


function minimum_spantree(g, weights, root = 1)
    n = length(g.graph.vertices)
    finished, dist, parent, neighbors, pq = _init_spantree(g.graph.edges, n)
    return _minimum_spantree(weights, finished,
                             dist, parent, neighbors, pq, root)
end


function minimum_spantree_e(g, weights, root = 1)
    n = length(g.graph.vertices)
    selected = Vector{Int}(n)
    finished, dist, parent, neighbors, pq = _init_spantree(g.graph.edges, n)
    _minimum_spantree_e(weights, finished,dist, parent, neighbors,
                        selected, pq, root)
    return parent, selected
end


function _init_spantree(edges, n)
    m = length(edges)
    finished = Vector{Bool}(n)
    dist = Vector{Float64}(n)
    parent = Vector{Int}(n)
    neighbors = compute_undirected_index(edges, n)
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


function _minimum_spantree_e(weights, finished, dist, parent,
                             neighbors::NeighborIndex, selected::Vector{Int},
                             pq::PriorityQueue{Int, Float64}, root::Int = 1)
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
