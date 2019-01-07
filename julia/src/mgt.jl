module MGT
include("tree_dp.jl")
include(joinpath(@__DIR__(), "..", "..", "graphidx", "julia", "src", "mst.jl"))
include(joinpath(@__DIR__(), "..", "..", "graphidx", "julia", "src", "GraphIdx.jl"))

import Printf: @sprintf
import SparseArrays: SparseMatrixCSC
import .DPTree: _alloc_queues, _dp_tree, _init_dp_tree
import .GraphIdx: ChildrenIndex

const IncMat = SparseMatrixCSC{Float64,Int}


"""
Compute the gap vector (modifies dif and x) and stores it in γ.
"""
function gap_vec!(γ::Vector{Float64},
                  dif::Vector{Float64},
                  x::Vector{Float64},
                  y::Vector{Float64},
                  D::IncMat,
                  Dt::IncMat,
                  α::Vector{Float64})
    A_mul_B!(x, Dt, α)
    x .-= y
    A_mul_B!(dif, D, x)
    γ .= (α .* dif) .+ abs.(dif)
    return γ
end


# max_gap_tree(y::Matrix{Float64}, g::FLSA.ImgGraph; args...) =
#     reshape(max_gap_tree(vec(y), g; args...), size(y)...)


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
    r = Int(root_node)
    m, n = size(D)
    Dt = copy(D')
    alpha = c0 * sign.(D*vec(y))
    dif = zeros(m)
    γ = zeros(m)
    x = copy(y)
    xbuf = Vector{Float64}(undef, n)
    z = similar(y)
    selected = Vector{Int}(undef, n)
    selected[root_node] = 0
    lb, ub, elements = _init_dp_tree(n)
    pq, proc_order, stack, childs = _alloc_queues(n)
    tlam = Vector{Float64}(undef, n)
    finished, dist, parent, neighbors, mst_pq = _init_spantree(edges, n)

    total = 0.0
    for it in 0:max_iter
        dprocess(alpha)
        it >= max_iter && break
        gap_vec!(γ, dif, x, y, D, Dt, alpha)
        gap_value = sum(γ)
        if verbose
            println(@sprintf("%4d %f",
                             it, gap_value))
        end
        γ .*= -1.0
        _minimum_spantree_e(γ, finished, dist, parent,
                            neighbors, selected, mst_pq, r)
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
                     lb,
                     ub,
                     elements,
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
                # e = edges[eidx]
                xbuf[p] += mu_i * xbuf[i]
                c = xbuf[i] * (i < p ? -mu_i : +mu_i)
                alpha[eidx] = c / lambda[eidx]
            end
        end
    end
    return x
end

end
