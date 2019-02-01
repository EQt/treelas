"""
    UnionFind(n)


Classical Union-Find data structure with path compression as described
eg in CLRS page XY
"""
struct UnionFind
    p::Vector{Int}
    rank::Vector{Int}
    UnionFind(n::Int) = new(collect(1:n), zeros(Int, n))
end


function Base.show(io::IO, u::UnionFind)
    println(io, typeof(u), ": n=", length(u.p))
    println(io, "p:    ", u.p)
    println(io, "rank: ", u.rank)
end


function init(u::UnionFind)
    for i in 1:length(u.p)
        u.p[i] = i
    end
    u.rank .= 0
end


Base.getindex(u::UnionFind, x::Int) =
    find(u, x)


function find(u::UnionFind, x::Int)
    if u.p[x] != x
        u.p[x] = find(u, u.p[x])
    end
    return u.p[x]
end


function unite!(u::UnionFind, fx::Int, fy::Int)
    if u.rank[fx] > u.rank[fy]
        u.p[fy] = fx
    else
        u.p[fx] = fy
        if u.rank[fx] == u.rank[fy]
           u.rank[fy] += 1
        end
    end
    return u
end
