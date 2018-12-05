"""
Complete rewrite of the old FLSA code for tree graphs 
=====================================================

GOAL: avoid reallocation of memory in the event queue.

Ideas
-----
Is it possible to omit the position x from the events?
The position is just the intersection of two neighboring event line segments.
However, the end position is complicated to compute.
For sorting the events, it is also necessary
"""
module FLSA2
using Printf: @sprintf
const MERGESORT = false
include("children.jl")


# Tree -------------------------------------------------------------------
struct Tree
    root::Int
    parent::Vector{Int}
    dfs::Vector{Int}
end


# Event ------------------------------------------------------------------
struct Event
    x::Float64      # position: redundant, but makes queue easier
    slope::Float64  # delta slope
    offset::Float64 # delta offset
end

Base.isless(e1::Event, e2::Event) = isless(e1.x, e2.x)
Base.zero(::Type{Event}) = Event(NaN, NaN, NaN)
Base.isnan(e::Event) = isnan(e.x)
Base.show(io::IO, e::Event) =
    if isnan(e)
        print(io, "Event(NaN)")
    else
        print(io,
              @sprintf("% .2f -> % .2ft + % .3f",
                       e.x, e.slope, e.offset))
    end

# Queue -----------------------------------------------------------------
struct Queue
    start::Int
    stop::Int
end

Base.length(q::Queue) = q.stop - q.start + 1
Base.show(io::IO, q::Queue) = print(io, q.start, ":", q.stop)

function _alloc_queues(n::Int)
    pq = Vector{Queue}(n)
    proc_order = Vector{Int}()
    stack = Vector{Int}()
    sizehint!(stack, n)
    sizehint!(proc_order, n)
    childs = ChildrenIndex(n)
    return pq, proc_order, stack, childs
end


function init_queues(parent::Vector{Int}, root::Int = 1)
    n = length(parent)
    pq, proc_order, stack, childs = _alloc_queues(n)
    return _init_queues(parent, root, pq, proc_order, stack, childs)
end


function _init_queues(parent, root, pq, proc_order, stack, childs)
    _compute_children(childs.pi, childs.idx, parent, root)
    n = length(parent)
    @assert length(childs.pi) == n
    @assert length(childs.idx) == n+1

    empty!(proc_order)
    sizehint!(proc_order, n)
    sizehint!(stack, n)

    @assert isempty(stack)
    @assert isempty(proc_order)
    push!(stack, root)
    t = 1
    while !isempty(stack)
        v = pop!(stack)
        t += 1
        if v >= 0
            push!(stack, -v)
            for i in childs.idx[v]:childs.idx[v+1]-1
                push!(stack, childs.pi[i])
            end
        else
            v = -v
            push!(proc_order, v)
            pq[v] = Queue(t, t-1)
        end
    end
    pop!(proc_order)    # remove root
    return pq, proc_order
end


@inline function merge(elements::Vector{Event},
                       parent::Queue, child::Queue)::Queue
    if parent.start <= parent.stop
        gap = child.start - parent.stop - 1
        old_stop = parent.stop
        res = Queue(parent.start, child.stop - gap)
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


@inline function merge2(buf::Vector{Event}, elements::Vector{Event},
                       parent::Queue, child::Queue)::Queue
    if parent.start <= parent.stop
        gap = child.start - parent.stop - 1
        old_stop = parent.stop
        res = Queue(parent.start, child.stop - gap)

        i = 1
        if length(buf) < length(parent)
            resize!(buf, length(parent))
        end
        for j in parent.start:parent.stop
            buf[i] = elements[j]
            i += 1
        end
        buf_end = i
        l = 1
        r = child.start
        el = buf[l]::Event
        er = elements[r]::Event
        for k in res.start:res.stop
            if l < buf_end && (r > child.stop || el.x < er.x)
                elements[k] = el
                l += 1
                el = buf[l]::Event
            else
                elements[k] = er
                r += 1
                er = elements[r]::Event
            end
        end

        return res
    end
    return child
end


@inline function clip_front(elements::Vector{Event}, pqs::Vector{Queue}, i::Int,
                            slope::Float64, offset::Float64, t::Float64)
    begin
        pq = pqs[i]::Queue
        start = pq.start
        stop = pq.stop
        e = elements[start]::Event
        while start <= stop && slope * e.x + offset < t
            offset += e.offset
            slope  += e.slope
            start += 1
            e = elements[start]::Event
        end
        x = (t - offset)/slope
        start -= 1
        elements[start] = Event(x, slope, offset - t)
        pqs[i] = Queue(start, stop)
        return x
    end
end


@inline function clip_back(elements::Vector{Event}, pqs::Vector{Queue}, i::Int,
                           slope::Float64, offset::Float64, t::Float64)
    begin
        pq = pqs[i]::Queue
        start = pq.start
        stop = pq.stop
        e = elements[stop]::Event
        while start <= stop && slope * e.x + offset > t
            offset -= e.offset
            slope  -= e.slope
            stop -= 1
            e = elements[stop]::Event
        end
        x = (t - offset)/slope
        stop += 1
        elements[stop] = Event(x, -slope, -offset + t)
        pqs[i] = Queue(start, stop)
        return x
    end
end

# Actual Implementation -----------------------------------------------
dp_tree(y::Vector{Float64}, λ::Float64, t::Tree) =
    dp_tree(y, i->λ, i->1.0, t)

dp_tree(y::Matrix{Float64}, λ::Float64, t::Tree) =
    reshape(dp_tree(vec(y), λ, t), size(y))


function dp_tree(y, λ, µ, t::Tree)
    n = length(y)
    lb, ub, elements = _init_dp_tree(n)
    pq, proc_order, stack, childs = _alloc_queues(n)
    return _dp_tree(y, λ, μ, t.root, t.parent, lb, ub, elements,
                    pq, proc_order, stack, childs)
end


_init_dp_tree(n) =
    Vector{Float64}(n), Vector{Float64}(n), Vector{Event}(2n)


function _dp_tree(y::Vector{Float64},
                  λ::Function,
                  μ::Function,
                  root::Int,
                  parent::Vector{Int},
                  lb::Vector{Float64},
                  ub::Vector{Float64},
                  elements::Vector{Event},
                  pq::Vector{Queue},
                  proc_order::Vector{Int},
                  stack::Vector{Int},
                  childs::ChildrenIndex)
    sig = lb
    @static if MERGESORT
        buf = Vector{Event}(10)
    end
    _init_queues(parent, root, pq, proc_order, stack, childs)
    sig .= 0.0
    for i in proc_order
        sig[parent[i]] += λ(i)
        sig_i = sig[i]
        lb[i] = clip_front(elements, pq, i, µ(i), -µ(i)*y[i] -sig_i, -λ(i))
        # println("lb[$(i-1)] = ", @sprintf("% 4.2f", lb[i]))
        # _print_elments(elements)
        ub[i] = clip_back( elements, pq, i, µ(i), -µ(i)*y[i] +sig_i, +λ(i))
        # println("ub[$(i-1)] = ", @sprintf("% 4.2f", ub[i]))
        # _print_elments(elements)
        @static if MERGESORT
            pq[parent[i]] = merge2(buf, elements, pq[parent[i]], pq[i])
        else
            pq[parent[i]] = merge(elements, pq[parent[i]], pq[i])
            if length(parent) <= 10
                println("pq[$(parent[i])] = ", pq)
            end
        end
    end

    x = ub
    x[root] = clip_front(elements, pq, root,
                         µ(root), -µ(root)*y[root] -sig[root], 0.0)
    for i in length(proc_order):-1:1
        v = proc_order[i]
        x[v] = clamp(x[parent[v]], lb[v], ub[v])
    end
    return x
end


# Debug ---------------------------------------------------------------
function _print_elments(elements)
    println("[", join([@sprintf("Event(% 4.2f, % 4.2f % 4.2f)\n",
                                e.x, e.slope, e.offset)
                       for e in elements], ", "), "]")

end


end
