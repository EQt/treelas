module QueueUnion

import GraphIdx.Tree: ChildrenIndex, reset!, dfs_walk_rev


"""
    Range(start, stop)

A range in a `Vector`.
In our algorithm is is used for the nodes of [`Queues`](@ref) of [`Event`](@ref)s.

!!! warning

    unlike in Python, `start` and `stop` are inclusive!
"""
struct Range
    start::Int
    stop::Int
end

Base.length(q::Range) = q.stop - q.start + 1

Base.show(io::IO, q::Range) = print(io, q.start, ":", q.stop)


Base.convert(::Type{Range}, r::UnitRange{Int}) =
    Range(r.start, r.stop)


function merge(elements::Vector{E}, parent::Range, child::Range)::Range where {E}
    if parent.start <= parent.stop
        gap = child.start - parent.stop - 1
        old_stop = parent.stop
        res = Range(parent.start, child.stop - gap)
        if gap > 0
            for i in old_stop+1:res.stop
                elements[i] = elements[i+gap]
            end
        end
        sort!(elements, Int(res.start), Int(res.stop),
              PartialQuickSort(Int(res.start):Int(res.stop)),
              Base.Sort.Forward)
        return res
    end
    return child
end


function merge(elements::Vector{E}, pq::Vector{Range}, p::I, i::I) where {E,I}
    if pq[p].start <= pq[p].stop
        gap = pq[i].start - pq[p].stop - 1
        old_stop = pq[p].stop
        res = Range(pq[p].start, pq[i].stop - gap)
        if gap > 0
            for i in old_stop+1:res.stop
                elements[i] = elements[i+gap]
            end
        end
        sort!(elements, Int(res.start), Int(res.stop),
              PartialQuickSort(Int(res.start):Int(res.stop)),
              Base.Sort.Forward)
        pq[p] = res
    else
        pq[p] = pq[i]
    end
end



"""
Set of double ended queues (DeQue) in a tree.
Initially all leaf nodes have its own queue.
Children's queues will be merged into the parent node.
"""
struct Queues{E}
    events::Vector{E}
    pq::Vector{Range}
end


Queues{E}(n::Integer) where {E} =
    Queues{E}(Vector{E}(undef, 2n),
              Vector{Range}(undef,  n))


Base.getindex(q::Queues, i) = q.pq[i]


function merge!(qs::Queues{E}, i::I, j::I) where {I,E}
    qs.pq[i] = merge(qs.events, qs.pq[i], qs.pq[j])
end


function reset!(
    pq::Vector{Range},
    childs::ChildrenIndex,
    stack::Vector{I} = I[],
    proc_order::Vector{I} = I[],
) where {I}
    @assert length(pq) == length(childs)
    resize!(proc_order, length(childs))
    empty!(proc_order)
    @assert isempty(proc_order)
    empty!(stack)

    local t::Ref{Int} = Ref{Int}(0)
    dfs_walk_rev(childs, stack) do v::Int
        t[] += 1
        if v >= 0
            push!(proc_order, v)
            pq[v] = Range(t[], t[]-1)
        end
    end

    return proc_order
end

end
