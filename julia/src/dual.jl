module Dual

include("gap.jl")

import GraphIdx.Tree: ChildrenIndex, dfs_walk, root_node

function dual(
    x::Array{F,N},
    y::Array{F,N},
    post_order::Vector{I},
    parent::Vector{I},
    alpha_root::F = F(0.0),
) where {F,N,I}
    dual!(Vector{F}(undef, length(post_order)),
          x,
          y,
          post_order,
          parent,
          alpha_root)
end


"""
    dual!(α, [, x, y], post_order, parent)

Compute the tree dual and store it in `α`.
"""
function dual!(
    alpha::Vector{F},
    x::Array{F,N},
    y::Array{F,N},
    post_order::Vector{I},
    parent::Vector{I},
    alpha_root::F = F(0.0),
) where {F,N,I}
    alpha .= vec(x) .- vec(y)
    dual!(alpha, post_order, parent, alpha_root)
end


function dual!(
    alpha::Vector{F},
    post_order::Vector{I},
    parent::Vector{I},
    alpha_root::F = F(0.0),
) where {F<:Real,I<:Integer}
    for c in @view post_order[1:end-1]
        let v = parent[c]
            alpha[v] += alpha[c]
        end
    end
    if alpha_root != F(0.0)
        let root = post_order[end], acc = eps() * length(parent)
            @assert abs(alpha[root]) <= acc "$(abs(alpha[root])) < = $acc"
            alpha[root] = alpha_root
        end
    end
    return alpha
end


dual(z::Vector{F}, parent::Vector{I},
     cidx::ChildrenIndex, alpha_root::F = F(0.0)) where {F,I} =
    dual!(copy(z), parent, cidx, alpha_root)


function dual!(
    alpha::Vector{F},
    parent::Vector{I},
    cidx::ChildrenIndex,
    alpha_root::F = F(0.0),
) where {F,I}
    for (i, v) in enumerate(parent)
        alpha[i] = i > v ? -alpha[i] : +alpha[i]
    end
    dfs_walk(cidx) do v::Int
        if v >= 0
            let p = parent[v]
                if p != v
                    alpha[p] += alpha[v]
                end
            end
        end
    end
    if alpha_root != F(0.0)
        let root = root_node(cidx), acc = eps() * length(parent)
            @assert abs(alpha[root]) <= acc "$(abs(alpha[root])) < = $acc"
            alpha[root] = alpha_root
        end
    end
    return alpha
end


"""
    primal_from_dual(y, α, graph)

If ``D`` is the oriented incidence matrix of `graph`,
return ``y + D'*α``.
"""
primal_from_dual(y::Array{F,N}, alpha::Vector{F}, graph::G) where {F,N,G} =
    primal_from_dual!(copy(y), alpha, graph)


"""

    primal_from_dual!(y, α, graph)

Similar to [`primal_from_dual`](@ref) but store the result in y.
"""
function primal_from_dual!(
    x::Array{F,N},
    alpha::Vector{F},
    graph::G,
)::Array{F,N} where {F<:Real,N,G}
    enumerate_edges(graph) do ei::Int, u::Int, v::Int, lam::Float64
        x[u] += alpha[ei]
        x[v] -= alpha[ei]
    end
    return x
end


end
