"""
Linear Algebra related graph functions
"""
module LinA
using SparseArrays

const IncMat = SparseMatrixCSC{Float64,Int64}

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


end
