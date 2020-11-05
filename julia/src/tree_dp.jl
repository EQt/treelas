"""
Dynamic Programming Tree Solver
===============================

Merge the event queues within the tree by sorting arrays.

Compared to previous versions, all weighting (λ and μ) is templated;
thus we only need one implementation and the compiler will do the
rest.

"""
module TreeDP

import GraphIdx
import GraphIdx: Ones, Const, Vec, Weights, is_const
import GraphIdx.Utils: MutRef
import GraphIdx.Tree: ChildrenIndex, reset!, dfs_walk
import ..Pwl: clip, Range, Event, EPS, DEBUG
import ..Pwl: clip_front, clip_back
import ..Pwl.QueueUnion: Queues, merge, reset!

const Tree = GraphIdx.Tree.RootedTree


"""
Contains all memory needed for `tree_dp!`.
"""
struct TreeDPMem{F,I}
    lb::Vector{F}
    queues::Queues
    proc_order::Vector{I}
    kidz::ChildrenIndex
    stack::Vector{Int}
end


function TreeDPMem(n::Integer, F::Type = Float64, I::Type = Int)
    lb = Vector{F}(undef, n)
    proc_order = Vector{I}(undef, n)
    kidz = ChildrenIndex(n)
    stack = Vector{Int}(undef, n)
    TreeDPMem{F,I}(lb, Queues{Event}(n), proc_order, kidz, stack)
end


"""
    reset!(mem::TreeDPMem, t::Tree)

Re-initialize the memory given the new tree, i.e.
0. Assert that `mem` has the required size for `t`.
1. Compute processing order.
2. Initialize queues to fit `t`.
"""
function reset!(mem::TreeDPMem{F,I}, tree::Tree) where {F,I}
    reset!(mem.kidz, tree.parent, tree.root)
    sizehint!(mem.proc_order, length(tree))
    reset!(mem.queues.pq, mem.kidz, mem.stack, mem.proc_order)
    mem
end


"""
    tree_dp(y, t::Tree, λ [, μ = 1.0])

Perform the dynamic programming algorithm on `y` and return the optimized `x`.

The edge weighting λ should be either a constant (e.g. `Float64`) or a
callable such that `λ(i)` returns the weight of the edge `(i, t.parent[i])`;
for `λ(t.root)` it might return anything but not causean error.
"""
tree_dp(y::Array{F,N}, t::Tree, λ::F)  where {F,N} =
    tree_dp!(similar(y), y, t, Const{F}(λ), Ones{F}())

tree_dp(y::Array{F,N}, t::Tree, λ::F, µ::F)  where {F,N} =
    tree_dp!(similar(y), y, t, Const{F}(λ), Const{F}(µ))

tree_dp(y::Array{F,N}, t::Tree, λ::Lam, µ::Mu) where {F,N,Lam,Mu} =
    tree_dp!(similar(y), y, t, λ, µ)


"""
    tree_dp!(x, y, t, λ, μ [,mem])

Like `tree_dp` but do not allocate an output buffer for `x`.

If `mem::TreeDPMem` is provided, no additional allocations will be
needed.
"""
tree_dp!(x::Array{F,N}, y::Array{F,N}, t::Tree, λ::La, µ::Mu) where {F,N,La,Mu} =
    tree_dp!(x, y, t, λ, µ, TreeDPMem(length(y)))


function tree_dp!(
    x::Array{F,N},
    y::Array{F,N},
    t::Tree,
    λ::Lam,
    µ::Mu,
    mem::TreeDPMem{F,I},
)::Array{F,N} where {F, I, N, Lam<:Weights, Mu<:Weights}
    tree_dp!(x, y, t, λ, μ, mem, Val(!is_const(Mu)))
end

function tree_dp!(
    x::Array{F,N},
    y::Array{F,N},
    t::Tree,
    λ::Lam,
    µ::Mu,
    mem::TreeDPMem{F,I},
    C::Val{check},
)::Array{F,N} where {F, I, N, Lam<:Weights, Mu<:Weights, check}
    reset!(mem, t)
    local lb::Vector{F} = mem.lb
    local ub::Vector{F} = vec(x)
    local sig::Vector{F} = mem.lb
    local ev::Vector{Event} = mem.queues.events
    local pq::Vector{Range} = mem.queues.pq
    local ref::MutRef{Range} = MutRef(pq, 1)

    sig .= 0
    for i in @view mem.proc_order[1:end-1]
        local sig_i::F = sig[i]
        ref.i = i
        lb[i] = clip(ev, ref, +µ[i], -µ[i]*y[i] - sig_i + λ[i], Val(true), C)
        ub[i] = clip(ev, ref, -µ[i], +µ[i]*y[i] - sig_i + λ[i], Val(false), C)
        merge(mem.queues.events::Vector{Event},
              mem.queues.pq::Vector{Range},
              t.parent[i]::Int, i::Int)
        sig[t.parent[i]] += μ[i] > EPS ? λ[i] : min(sig_i, λ[i])
    end

    let r = t.root
        ref.i = r
        x[r] = clip(ev, ref,  +µ[r], -µ[r]*y[r] -sig[r] + 0.0, Val(true), C)
    end
    for v in @view mem.proc_order[end-1:-1:1]
        x[v] = clamp(x[t.parent[v]], lb[v], ub[v])
    end

    return x
end


end
