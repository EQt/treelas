import .Utils.Heap: PriorityQueue, dequeue!


"""
    prim_mst_edges(n, weights, [root = 1])

[Prim's algorithm](https://en.wikipedia.org/wiki/Prim%27s_algorithm)
for minimum spanning tree.
Start from node `root` (will become the root node of the spanning
tree).  Return the parent vector and a Boolean vector indicating for
each edge whether the edge is part of the spanning tree.
"""
function prim_mst_edges(
    n::Integer,
    edges::Vector{E},
    weights::Array{Float64},
    root::Int = 1,
) where {E}
    mem = PrimMstMem(edges, n)
    prim_mst_edges(weights, root, mem)
    # swap root element to front
    mem.selected[1], mem.selected[root] = mem.selected[root], mem.selected[1]
    return mem.parent, view(mem.selected, 2:length(mem.selected))
end


struct PrimMstMem
    finished::Vector{Bool}
    dist::Vector{Float64}
    parent::Vector{Int}
    neighbors::NeighborIndex
    selected::Vector{Int}
    pq::PriorityQueue{Int, Float64}

    PrimMstMem(edges::Vector{E}, n::Integer) where {E} =
        PrimMstMem(NeighborIndex(n, edges))

    PrimMstMem(head::Vector{I}, tail::Vector{I}, n::Integer) where {I} =
        PrimMstMem(NeighborIndex(n, head, tail))

    function PrimMstMem(neighbors::NeighborIndex)
        local n = num_nodes(neighbors)
        finished = Vector{Bool}(undef, n)
        dist = Vector{Float64}(undef, n)
        parent = Vector{Int}(undef, n)
        selected = Vector{Int}(undef, n)
        pq = PriorityQueue{Int, Float64}(n)
        return new(finished, dist, parent, neighbors, selected, pq)
    end
end


prim_mst_edges(
    weights::Vector{Float64},
    root::Int,
    edges::Vector{E},
    n::Integer,
) where {E} =
    prim_mst_edges(weights, root, PrimMstMem(edges, n))


prim_mst_edges(weights::Vector{Float64}, root::Int, mem::PrimMstMem) = 
    prim_mst_edges(weights,
                   mem.finished,
                   mem.dist,
                   mem.parent,
                   mem.neighbors,
                   mem.selected,
                   mem.pq,
                   root)


function prim_mst_edges(
    edge_weight::Vector{Float64},
    finished::Vector{Bool},
    dist::Vector{Float64},
    parent::Vector{Int},
    neighbors::NeighborIndex,
    selected::Vector{Int},
    pq::PriorityQueue{Int, Float64},
    root::Int = 1,
)::Vector{Int}
    @assert length(selected) == length(parent)
    @assert isempty(pq)
    sizehint!(pq, length(parent))

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
