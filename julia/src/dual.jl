module Dual

include("gap.jl")

import GraphIdx.Tree: ChildrenIndex, dfs_walk


function dual(
    x::Array{F,N},
    y::Array{F,N},
    post_order::Vector{I},
    parent::Vector{I},
    alpha_root::F = F(0.0),
) where {F,N,I}
    dual!(Vector{F}(undef, length(post_order)), x, y, post_order, parent,
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
    for i in 1:length(parent)
        let v = parent[i]
            alpha[i] = i > v ? -alpha[i] : +alpha[i]
        end
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
        let root = root_node(cidx)
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
primal_from_dual(y::Array{Float64,N}, alpha::Array{Float64}, graph::G)::Array{Float64,N} where {N,G} =
    primal_from_dual!(copy(y), alpha, graph)


"""

    primal_from_dual!(y, α, graph)

Similar to [`primal_from_dual`](@ref) but store the result in y.
"""
function primal_from_dual!(x::Array{Float64}, alpha::Array{Float64}, graph::G)::Float64 where G
    enumerate_edges(graph) do ei::Int, u::Int, v::Int, _::Float64
        x[u] += alpha[ei]
        x[v] -= alpha[ei]
    end
    return x
end


end
