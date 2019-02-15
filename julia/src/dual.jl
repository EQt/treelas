function dual(
    x::Array{F,N},
    y::Array{F,N},
    post_order::Vector{I},
    parent::Vector{I},
) where {F,N,I}
    dual!(Vector{F}(undef, length(post_order)), x, y, post_order, parent)
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
) where {F,N,I}
    alpha .= vec(x) .- vec(y)
    dual!(alpha, post_order, parent)
end


function dual!(
    alpha::Vector{F},
    post_order::Vector{I},
    parent::Vector{I},
) where {F<:Real,I<:Integer}
    for v in @view post_order[1:end-1]
        alpha[parent[v]] += alpha[v]
    end
    alpha[post_order[end]] = NaN
    return alpha
end
