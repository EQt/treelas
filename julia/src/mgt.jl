"""
# Maximum Gap Tree

In the central function `max_gap_tree`, a tree having the largest gap
values (computed by `gap_vec!`) is selected.  The non-tree edge-flows
are forwarded into the input `y`.  Then the tree solver is used and
the tree-edges are updated.

## Refactoring

At the moment (2019-01-13), the code is pretty unorganized und hard to
understand.

- What could we do to better organize the code?
- What should be adaptable in the future?


## Input

At the moment, I need a lot of redundant information:
```
D::IncMat,
edges::Vector{E},
lambda::Vector{Float}
```

Which of those could be brought together?

Obviously, D can be constructed from `edges` and `lambda` (which is
already implemented).

Looking into the future, we might not have an explicit list of edges:
for a grid graph, e.g. it makes no sense to explicitly compute all
edges.  So what do we need?

Prim's MST algorithm needs an index telling for each node the
neighbors.  The same is needed for DFS traversal.  Furtheron, an edge
order need to be specified, i.e. we should provide an edge iterator.

"""
module MGT
include("tree_dp.jl")

import SparseArrays: mul!
import Printf: @sprintf
import .DPTree: _alloc_queues, _dp_tree, DPMem
import GraphIdx.Tree: ChildrenIndex
# , _init_spantree, _minimum_spantree_edgse
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx.LinA: IncMat, Edges


"""
    gap_vec!(...)

Compute the gap vector (modifies dif and x) and stores it in γ.
"""
function gap_vec!(γ::Vector{Float64},
                  dif::Vector{Float64},
                  x::Vector{Float64},
                  y::Vector{Float64},
                  D::IncMat,
                  Dt::IncMat,
                  α::Vector{Float64})
    mul!(x, Dt, α)
    x .-= y
    mul!(dif, D, x)
    γ .= (α .* dif) .+ abs.(dif)
    return γ
end


"""
    max_gap_tree(y, edges, λ)

Optimize in each iteration along a tree.
"""
max_gap_tree(y::Matrix{Float64}, edges::Edges, λ::Vector{Float64}; args...) =
    reshape(max_gap_tree(vec(y), edges, λ; args...), size(y)...)


max_gap_tree(y::Vector{Float64}, edges::Edges, λ::Vector{Float64}; args...) =
    max_gap_tree(y, IncMat(edges, λ, length(y)), edges, λ; args...)


function max_gap_tree(y::Vector{Float64},
                      D::IncMat,
                      edges::Vector{E},
                      lambda::Vector{Float64};
                      root_node::Int = 1,
                      mu::Float64 = 1.0,
                      max_iter::Int = 3,
                      verbose::Bool = true,
                      process::Function=x->nothing,
                      dprocess::Function=α->nothing,
                      tprocess::Function=(t,w)->nothing,
                      c0::Float64 = 0.0) where E
    m, n = size(D)
    Dt = copy(D')
    alpha = c0 * sign.(D*vec(y))
    dif = zeros(m)
    γ = zeros(m)
    x = copy(y)
    z = similar(y)
    tlam = Vector{Float64}(undef, n)
    xbuf = Vector{Float64}(undef, n)
    mem = DPMem(n)
    pq, proc_order, stack, childs = _alloc_queues(n)
    mst_mem = PrimMstMem(edges, n)
    selected = mst_mem.selected
    parent = mst_mem.parent

    for it in 0:max_iter
        dprocess(alpha)
        it >= max_iter && break
        gap_vec!(γ, dif, x, y, D, Dt, alpha)
        verbose && println(@sprintf("%4d %f", it, sum(γ)))
        γ .*= -1.0
        prim_mst_edges(γ, root_node, mst_mem)
        tprocess(γ, parent)
        z .= y
        for (i, e) in enumerate(edges)
            v, u = e
            if parent[v] == u
                tlam[v] = lambda[i]
            elseif parent[u] == v
                tlam[u] = lambda[i]
            else
                z[v] -= lambda[i] * alpha[i]
                z[u] += lambda[i] * alpha[i]
            end
        end

        x_old = x
        x = _dp_tree(z,
                     i -> tlam[i],
                     i -> mu,
                     root_node,
                     parent,
                     mem,
                     pq,
                     proc_order,
                     stack,
                     childs)
        process(x)
        ub = x_old
        begin # compute dual ==> update alpha
            local mu_i = mu
            xbuf .= x .- z
            for i in proc_order
                p = parent[i]
                eidx = selected[i]
                xbuf[p] += mu_i * xbuf[i]
                c = xbuf[i] * (i < p ? -mu_i : +mu_i)
                alpha[eidx] = c / lambda[eidx]
            end
        end
    end
    return x
end

end
