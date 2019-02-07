"""
Linear Algebra related graph functions.
Especially, methods for handling incidence matrixes [`IncMat`](@ref).
"""
module LinA
import SparseArrays: SparseMatrixCSC, sparse

const IncMat = SparseMatrixCSC{Float64,Int64}
const Edges = Vector{Tuple{Int, Int}}


"""
    is_incmat(::IncMat)

Check whether a matrix is a graph (oriented) edge-vertex incidence matrix.
"""
function is_incmat(d::IncMat)::Bool
    dt = IncMat(d')
    any(diff(diff(dt.colptr)) .!= 0) && return false
    any(dt.nzval[1:2:end] + dt.nzval[2:2:end] .!= 0) && return false
    return true        
end


function edges_from_incmat(d::IncMat)
    m, n = size(d)
    head = Vector{Int}(undef, m)
    tail = Vector{Int}(undef, m)
    lam = Vector{Float64}(undef, m)
    k = Int(1)
    for i = 1:n
        for ci = d.colptr[i]:d.colptr[i+1]-1
            ei = d.rowval[ci]
            v = d.nzval[ci]
            lam[ei] = abs(v)
            if v > 0
                head[ei] = i
            else
                tail[ei] = i
            end
        end
    end
    return head, tail, lam
end


"""
    IncMat(edges, λ, n [, T = Int])

Create an incidence matrix from a list of edges, weights `λ` having `n` nodes.
The resulting matrix index will have type `T`.
"""
function IncMat(E::Edges,
                lam::Vector{Float64},
                n::Int,
                T = Int)::IncMat
    m = length(E)
    I = Vector{T}(undef, 2m)
    J = Vector{T}(undef, 2m)
    V = Vector{eltype(lam)}(undef, 2m)
    for (i, (u, v)) in enumerate(E)
        I[2i-1] = i
        J[2i-1] = u
        V[2i-1] = lam[i]
        I[2i-0] = i
        J[2i-0] = v
        V[2i-0] = -lam[i]
    end
    sparse(I, J, V, m, n)
end

end
