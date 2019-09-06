 """
    UnionFind(n)


Classical Union-Find data structure with path compression as described
e.g. in CLRS in Chapter 21 "Disjooint Sets" (3rd edition).

## Example

```jldoctest
julia> import GraphIdx.Utils: UnionFind, find, unite!

julia> u = UnionFind(2);

julia> find(u, 1) == find(u, 2)
false

julia> unite!(u, find(u, 1), find(u, 2));

julia> find(u, 1) == find(u, 2)
true

```
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


function reset!(u::UnionFind)::UnionFind
    for i in 1:length(u.p)
        u.p[i] = i
    end
    u.rank .= 0
    u
end


"""
Representant of a partition in UnionFind.
"""
struct Rep
    i::Int
end


# make it possible to a[find(u, 4)], where a::Array
Base.to_index(r::Rep) = r.i


Base.getindex(u::UnionFind, x::Int)::Rep =  find(u, x)


function find(u::UnionFind, x::Int)::Rep
    if u.p[x] != x
        u.p[x] = find(u, u.p[x]).i
    end
    return Rep(u.p[x])
end


function unite!(u::UnionFind, fx::Rep, fy::Rep)::UnionFind
    let fx::Int = fx.i, fy::Int = fy.i
        if u.rank[fx] > u.rank[fy]
            u.p[fy] = fx
        else
            u.p[fx] = fy
            if u.rank[fx] == u.rank[fy]
                u.rank[fy] += 1
            end
        end
    end
    return u
end
