#=
Implement broadcast functionality for Sol
=#

Base.similar(sol::Sol{N}) where {N} =
    Sol(similar(sol.x), similar(sol.α))


Broadcast.broadcasted(::typeof(identity), s::Sol{N}) where {N} = s


function Broadcast.materialize!(dest::Sol{N}, src::Sol{N}) where {N}
    dest.x .= src.x
    dest.α .= src.α
end
