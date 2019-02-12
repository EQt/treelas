"""
Dynamic Programming Tree Solver
===============================

Merge the event queues within the tree by sorting arrays.

Compared to previous versions, all weighting (λ and μ) is templated;
thus we only need one implementation and the compiler will do the
rest.

"""
module TreeDP

include("queues.jl")
include("weights.jl")

import GraphIdx
import GraphIdx.Tree: ChildrenIndex, reset!, dfs_walk

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
    TreeDPMem{F,I}(lb, Queues(n), proc_order, kidz, stack)
end


"""
    reset!(mem::TreeDPMem, t::Tree)

Re-initialize the memory given the new tree, i.e.
0. Assert that `mem` has the required size for `t`.
1. Compute processing order.
2. Initialize queues to fit `t`.
"""
function reset!(mem::TreeDPMem{F,I}, tree::Tree) where {F,I}
    local proc_order::Vector{I} = mem.proc_order
    local childs::ChildrenIndex = mem.kidz
    local pq::Vector{Range} = mem.queues.pq

    reset!(childs, tree.parent, tree.root)
    sizehint!(proc_order, length(tree))
    empty!(proc_order)
    @assert isempty(proc_order)
    empty!(mem.stack)

    local t::Ref{Int} = Ref{Int}(1)
    dfs_walk(childs, mem.stack) do v::Int
        t[] += 1
        if v >= 0
            push!(proc_order, v)
            pq[v] = Range(t[], t[]-1)
        end
    end

    pop!(proc_order)    # remove root
    return mem
end


"""
    tree_dp(y, t::Tree, λ [, μ = 1.0])

Perform the dynamic programming algorithm on `y` and return the optimized `x`.

The edge weighting λ should be either a constant (e.g. `Float64`) or a
callable such that `λ(i)` returns the weight of the edge `(i, t.parent[i])`;
for `λ(t.root)` it might return anything but not causean error.
"""
tree_dp(y::Array{F,N}, t::Tree, λ::F, µ::F = F(1.0))  where {F,N} =
    tree_dp!(similar(y), y, t, ConstantWeights{F}(λ), ConstantWeights{F}(µ))


tree_dp(y::Array{F,N}, t::Tree, λ::Lam, µ::Mu) where {F,N,Lam,Mu} =
    tree_dp!(similar(y), y, t, λ, µ)


"""
    tree_dp!(x, y, t, λ, μ [,mem])

Like `tree_dp` but do not allocate an output buffer for `x`.

If `mem::TreeDPMem` is provided, no additional allocations will be
needed.
"""
tree_dp!(x::Array{F,N}, y::Vector{F}, t::Tree, λ::Lam, µ::Mu) where {F,N,Lam,Mu} =
    tree_dp!(x, y, t, λ, µ, TreeDPMem(length(y)))


function tree_dp!(x::Array{F,N}, y::Array{F,N}, t::Tree, λ::Lam,
                  µ::Mu, mem::TreeDPMem{F,I})::Array{F,N} where {F,I,N,Lam,Mu}
    reset!(mem, t)
    local lb::Vector{F} = mem.lb
    local ub::Vector{F} = x
    local sig::Vector{F} = mem.lb

    sig .= 0
    for i in mem.proc_order
        local sig_i::F = sig[i]
        lb[i] = clip_front(mem.queues, i, µ(i), -µ(i)*y[i] -sig_i, -λ(i))
        ub[i] = clip_back( mem.queues, i, µ(i), -µ(i)*y[i] +sig_i, +λ(i))
        merge!(mem.queues, i, t.parent[i])
    end

    x = ub
    let r = t.root
        x[r] = clip_front(mem.queues, r,  µ(r), -µ(r)*y[r] -sig[r], 0.0)
    end
    for i in length(mem.proc_order):-1:1
        v = mem.proc_order[i]
        x[v] = clamp(x[parent[v]], lb[v], ub[v])
    end
    return x

    return x
end


end
