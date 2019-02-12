"""
Dynamic Programming Tree Solver
===============================

GOAL: avoid reallocation of memory as much as possible.
"""
module DPTree
# include("graphidx.jl")
include("common.jl")

import GraphIdx.Tree: RootedTree
import GraphIdx.Tree: ChildrenIndex, reset!, dfs_walk


const Tree = RootedTree


function _alloc_queues(n::Int)
    pq = Vector{Range}(undef, n)
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
    reset!(childs, parent, root)

    n = length(parent)
    @assert length(childs) == n
    @assert length(childs.idx) == n+1

    empty!(proc_order)
    sizehint!(proc_order, n)
    @assert isempty(proc_order)

    local t::Ref{Int} = Ref{Int}(1)
    dfs_walk(childs, stack) do v::Int
        t[] += 1
        if v >= 0
            push!(proc_order, v)
            pq[v] = Range(t[], t[]-1)
        end
    end

    pop!(proc_order)    # remove root
    return pq, proc_order
end


@inline function merge(elements::Vector{Event},
                       parent::Range, child::Range)::Range
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


@inline function merge2(buf::Vector{Event}, elements::Vector{Event},
                        parent::Range, child::Range)::Range
    if parent.start <= parent.stop
        gap = child.start - parent.stop - 1
        old_stop = parent.stop
        res = Range(parent.start, child.stop - gap)

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


# Actual Implementation -----------------------------------------------
dp_tree(y::Vector{Float64}, λ::Float64, t::Tree) =
    dp_tree(y, i->λ, i->1.0, t)

dp_tree(y::Matrix{Float64}, λ::Float64, t::Tree) =
    reshape(dp_tree(vec(y), λ, t), size(y))


function dp_tree(y, λ, µ, t::Tree)
    n = length(y)
    mem = DPMem(n)
    pq, proc_order, stack, childs = _alloc_queues(n)
    return _dp_tree(y, λ, μ, t.root, t.parent,
                    mem.lb, mem.ub, mem.elements,
                    pq, proc_order, stack, childs)
end


struct DPMem
    lb::Vector{Float64}
    ub::Vector{Float64}
    elements::Vector{Event}
    DPMem(n) =
        new(Vector{Float64}(undef, n),
            Vector{Float64}(undef, n),
            Vector{Event}(undef, 2n))
end


_dp_tree(
    y::Vector{Float64},
    λ::Function,
    μ::Function,
    root::Int,
    parent::Vector{Int},
    mem::DPMem,
    pq::Vector{Range},
    proc_order::Vector{Int},
    stack::Vector{Int},
    childs::ChildrenIndex) =
        _dp_tree(y, λ, μ, root, parent,
                 mem.lb, mem.ub, mem.elements, pq,
                 proc_order, stack, childs)


function _dp_tree(y::Vector{Float64},
                  λ::Function,
                  μ::Function,
                  root::Int,
                  parent::Vector{Int},
                  lb::Vector{Float64},
                  ub::Vector{Float64},
                  elements::Vector{Event},
                  pq::Vector{Range},
                  proc_order::Vector{Int},
                  stack::Vector{Int},
                  childs::ChildrenIndex)
    sig = lb
    _init_queues(parent, root, pq, proc_order, stack, childs)
    sig .= 0.0
    for i in proc_order
        sig[parent[i]] += λ(i)
        sig_i = sig[i]
        lb[i] = clip_front(elements, pq, i, µ(i), -µ(i)*y[i] -sig_i, -λ(i))
        ub[i] = clip_back( elements, pq, i, µ(i), -µ(i)*y[i] +sig_i, +λ(i))
        pq[parent[i]] = merge(elements, pq[parent[i]], pq[i])
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
#=
function _print_elments(elements)
    println("[", join([@sprintf("Event(% 4.2f, % 4.2f % 4.2f)\n",
                                e.x, e.slope, intercept(e))
                       for e in elements], ", "), "]")

end
=#

end
