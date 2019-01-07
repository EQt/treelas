import SparseArrays: SparseMatrixCSC, sparse


const IncMat = SparseMatrixCSC{Float64,Int}
const Edges = Vector{Tuple{Int, Int}}


function create_D(E::Edges,
                  lam::Vector{Float64}, n::Int,
                  T = Int)::IncMat
    m = length(E)
    I = Vector{T}(2m)
    J = Vector{T}(2m)
    V = Vector{eltype(lam)}(2m)
    for (i,(u,v)) in enumerate(E)
        I[2i-1] = i
        J[2i-1] = u
        V[2i-1] = lam[i]
        I[2i-0] = i
        J[2i-0] = v
        V[2i-0] = -lam[i]
    end
    sparse(I, J, V, m, n)
end
