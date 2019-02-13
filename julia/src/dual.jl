function dual(
    x::Array{F,N},
    y::Array{F,N},
    proc_order::Vector{I},
    parent::Vector{I},
) where {F,N,I}
    dual!(Vector{F}(undef, length(proc_order)), x, y, proc_order, parent)
end


function dual!(
    alpha::Vector{F},
    x::Array{F,N},
    y::Array{F,N},
    proc_order::Vector{I},
    parent::Vector{I},
) where {F,N,I}
    alpha .= vec(x) .- vec(y)
    for v in @view proc_order[1:end-1]
        alpha[parent[v]] += alpha[v]
    end
end


