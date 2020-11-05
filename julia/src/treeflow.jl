module TreeFlow
import GraphIdx: Vec, Weights, Graph, EdgeGraph, enumerate_edges

"""
    extract_non_tree!(graph, y, λt, π, α, λ)

Iterate over all edges of `grapy` (weighted by `λ`) and do the following:
If the edge `e` is a tree edge, store the corresponding `λ[e]` in tree order.
If the edge `e` is not part of the tree, compute the flow `α[e]` into the
node input `y`.
"""
function extract_non_tree!(
    graph::Graph,
    parent::Vector{Int},
    y::Array,
    alpha::Array,
    tlam::W1,
    lambda::W2,
) where {W1, W2<:Weights}
    enumerate_typed_edges(graph, parent) do istree::Bool, i::Int, u::Int, v::Int
        if istree
            tlam[u] = lambda[i]
        else
            y[u] += alpha[i]
            y[v] -= alpha[i]
        end
    end
end

"""

For convinience: Transform `λ` from Array into a `GraphIdx.Vec`.
"""
extract_non_tree!(
    graph::Graph,
    parent::Vector{Int},
    y::Array,
    alpha::Array,
    tlam::W1,
    lambda::Array{T},
) where {W1, T} =
    extract_non_tree!(
        graph::Graph,
        parent::Vector{Int},
        y::Array,
        alpha::Array,
        tlam::W1,
        GraphIdx.Vec(lambda),
    )


"""
    enumerate_typed_edges(func, edges, π)

Call for every edge `(u, v)` enumerated by `i` the function
```julia
func(istree, i, u, v)
```

whereby `istree` tells whether the edge is a tree edge.
If it is a tree edge, `u` is the child of `v`.
"""
@inline function enumerate_typed_edges(
    func::Func, graph::Graph, parent::Vector{Int}
) where {Func<:Function}
    enumerate_edges(graph) do i::Int, u::Int, v::Int
        if parent[v] == u
            func(true, i, v, u)
        elseif parent[u] == v
            func(true, i, u, v)
        else
            func(false, i, u, v)
        end
    end
end



"""
    update_tree!(α, αt, selected, graph, parent)

Update the global dual `α` by a tree dual `αt`.
"""
function update_tree!(alpha, tree_alpha, selected, graph::EdgeGraph, parent)
    @assert selected[1] < 0
    for i in @view selected[2:end]
        local u::Int, v::Int = graph.edges[i]
        alpha[i] = if parent[v] == u
            -tree_alpha[v]
        elseif parent[u] == v
            +tree_alpha[u]
        else
            error("Should not happen")
        end
    end
end

end # module
