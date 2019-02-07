"""
PriorityQueue datastructure (based on a implicit binary heap).
Code copied from DataStructures.jl (long time ago)
"""
module Heap
import Base.Order: Ordering, Forward, lt

struct PriorityQueue{K,V,O<:Ordering}
    xs::Vector{Pair{K,V}}
    index::Vector{Int}    # Map elements to their index in xs
    o::O

    function PriorityQueue{K,V}(o::O=Forward) where {K,V, O<:Ordering}
        new{K,V,O}(Vector{Pair{K,V}}(), Vector{Int}(), o)
    end
    function PriorityQueue{K,V}(n::Integer, o::O=Forward) where {K,V, O<:Ordering}
        xs = Vector{Pair{K,V}}()
        sizehint!(xs, n)
        new{K,V,O}(xs, Vector{Int}(undef, n), o)
    end
end

heapleft(i::Integer) = 2i
heapright(i::Integer) = 2i + 1
heapparent(i::Integer) = div(i, 2)

Base.length(pq::PriorityQueue) = length(pq.xs)
Base.isempty(pq::PriorityQueue) = isempty(pq.xs)
Base.haskey(pq::PriorityQueue, key) = pq.index[key] > 0


function Base.sizehint!(pq::PriorityQueue, n::Integer)
    sizehint!(pq.xs, n)
    resize!(pq.index, n)
    pq.index .= 0
end


function percolate_down!(pq::PriorityQueue, i::Integer)
    x = pq.xs[i]
    @inbounds while (l = heapleft(i)) <= length(pq)
        r = heapright(i)
        j = r > length(pq) || lt(pq.o, pq.xs[l].second, pq.xs[r].second) ? l : r
        if lt(pq.o, pq.xs[j].second, x.second)
            pq.index[pq.xs[j].first] = i
            pq.xs[i] = pq.xs[j]
            i = j
        else
            break
        end
    end
    pq.index[x.first] = i
    pq.xs[i] = x
end


function percolate_up!(pq::PriorityQueue, i::Integer)
    x = pq.xs[i]
    @inbounds while i > 1
        j = heapparent(i)
        if lt(pq.o, x.second, pq.xs[j].second)
            pq.index[pq.xs[j].first] = i
            pq.xs[i] = pq.xs[j]
            i = j
        else
            break
        end
    end
    pq.index[x.first] = i
    pq.xs[i] = x
end


"""
Change the priority of an existing element, or equeue it if it isn't present.
"""
function Base.setindex!(pq::PriorityQueue{K, V}, value::V, key::K) where {K,V}
    if haskey(pq, key)
        i = pq.index[key]
        oldvalue = pq.xs[i].second
        pq.xs[i] = Pair{K,V}(key, value)
        if lt(pq.o, oldvalue, value)
            percolate_down!(pq, i)
        else
            percolate_up!(pq, i)
        end
    else
        enqueue!(pq, key, value)
    end
    value
end


enqueue!(pq::PriorityQueue, key, value) = enqueue!(pq, key=>value)

function enqueue!(pq::PriorityQueue{K,V}, pair::Pair{K,V}) where {K,V}
    key = pair.first
    if haskey(pq, key)
        throw(ArgumentError("PriorityQueue keys must be unique"))
    end
    push!(pq.xs, pair)
    pq.index[key] = length(pq)
    percolate_up!(pq, length(pq))

    return pq
end


function dequeue!(pq::PriorityQueue)
    x = pq.xs[1]
    y = pop!(pq.xs)
    if !isempty(pq)
        pq.xs[1] = y
        pq.index[y.first] = 1
        percolate_down!(pq, 1)
    end
    # delete!(pq.index, x.first)
    pq.index[x.first] = 0
    x.first
end


end
