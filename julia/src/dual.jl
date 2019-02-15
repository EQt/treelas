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
    for v in @view post_order[1:end-1]
        alpha[parent[v]] += alpha[v]
    end
    if alpha_root != F(0.0)
        let root = post_order[end], acc = eps() * length(parent)
            @assert abs(alpha[root]) <= acc "$(abs(alpha[root])) < = $acc"
            alpha[root] = alpha_root
        end
    end
    return alpha
end
