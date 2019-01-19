"""
Computations related to grid graphs
"""
module Grid
using SparseArrays
import LinearAlgebra
import LinearAlgebra: norm

"""
Position in a grid graph
"""
struct Pixel
    x::Int
    y::Int
end

Base.convert(::Type{Pixel}, p::Tuple{Int,Int}) = Pixel(p...)

Base.convert(::Type{Tuple{Int,Int}}, p::Pixel) = (p.x, p.y)

Base.getindex(p::Pixel, i::Int) =
    if i == 1
        p.x
    elseif i == 2
        p.y
    else
        throw(ArgumentErrow("i = $i"))
    end

LinearAlgebra.norm2(p::Pixel) = p.x^2 + p.y^2

LinearAlgebra.norm(x::Pixel) = sqrt(LinearAlgebra.norm2(x))


"""
    compute_dirs(dn)

Setting the neighborhood degree (`dn`) to `i` will set all numbers
less or equal `i` in the following diagram as neighbors of `[*]`.
```
[…] […] […] […] […] […] […]
[…] […] [4] [3] [4] […] […]
[…] [4] [2] [1] [2] [4] […]
[…] [3] [1] [*] [1] [3] […]
[…] [4] [2] [1] [2] [4] […]
[…] […] [4] [3] [4] […] […]
[…] […] […] […] […] […] […]
```
"""
function compute_dirs(dn::Int)::Vector{Pixel}
    if dn == 1
        Pixel[(1,0)]
    elseif dn == 2
        Pixel[(1,0), (1,1)]
    elseif dn == 3
        Pixel[(1,0), (1,1), (2,0)]
    elseif dn == 4
        Pixel[(1,0), (1,1), (2, 0), (2,1), (1,2)]
    else
        throw(ArgumentError("dn >= 4 not supported"))
    end
end


"""
    iter_edges(proc, n1, n2, dirs)

Call `proc(i1, j1, i2, j2, len)` for every grid edge
`(i1, j1) -- (i2, j2)` having `len`gth.
"""
function iter_edges(proc::Function, n1::Int, n2::Int, dirs::Vector{Pixel})
    for d in dirs
        len = 1/norm(d)
        for j = 1:(n2-d.y)
            for i = 1:(n1-d.x)
                proc(i, j, i+d.x, j+d.y, len)
            end
        end
        for j = 1:(n2-d.x)
            for i = 1+d.y:n1
                proc(i, j, i-d.y, j+d.x, len)
            end
        end
    end
end


"""
    num_edges(n1, n2, dirs)
    num_edges(n1, n2, dn::Int)

Number of edges in a grid graph `n1×n2` along directions `dirs`.
If called with last argument a number, first generate `dirs`.
"""
function num_edges(n1::Int, n2::Int, dirs::Vector{Pixel})::Int
    m = 0
    for d in dirs
        m += (n1-d.x)*(n2-d.y) + (n1-d.y)*(n2-d.x)
    end
    return m
end

num_edges(n1::Int, n2::Int, dn::Int = 1)::Int =
    num_edges(n1, n2, compute_dirs(dn))


function incmat(n1::Int, n2::Int, dn::Int = 1)
    """Fortran index of the matrix entry `(i,j)`"""
    pix2ind(i, j) = i + (j-1)*n1

    dirs = compute_dirs(dn)
    n = n1 * n2
    m = num_edges(n1, n2, dirs)

    I = Vector{Int}(undef, 2m)
    J = Vector{Int}(undef, 2m)
    W = zeros(Float64, 2m)

    k = Int(1)   # edge index
    iter_edges(n1, n2, dirs) do i, j, i2, j2, len
        I[2k-1] = k
        J[2k-1] = pix2ind(i, j)
        W[2k-1] = +len
        I[2k-0] = k
        J[2k-0] = pix2ind(i2, j2)
        W[2k-0] = -len
        k +=1
    end
    D = sparse(I, J, W, m, n)
end


function adjlist(n1::Int, n2::Int, dn::Int = 1)
    """Fortran index of the matrix entry `(i,j)`"""
    pix2ind(i, j) = i + (j-1)*n1

    dirs = compute_dirs(dn)
    m = num_edges(n1, n2, dirs)
    head = Vector{Int}(undef, m)
    tail = Vector{Int}(undef, m)
    lam =  Vector{Float64}(undef, m)

    k = Int(1)
    iter_edges(n1, n2, dirs) do i1, j1, i2, j2, len
        head[k] = pix2ind(i1, j1)
        tail[k] = pix2ind(i2, j2)
        lam[k] = len
        k += 1
    end
    head, tail, lam
end


"""
    line_D(n)

Return the incidence matrix for a line graph of length `n`.
Is equivalent to `incmat(1, n, 1)`
"""
function line_D(n)
    D = spzeros(n-1, n)
    for i = 1:n-1
        D[i, i+0] = +1
        D[i, i+1] = -1
    end
    return D
end


"""
    lipschitz(n1, n2, [dn | dirs])

Compute an upper bound for the Lipschitz-constant for ...
TODO: Be more precise
"""
function lipschitz(n1, n2, dirs)
    lmax = maximum(lam)
    Lip = lmax * 2 * 4 * sum([l for (d,l) in dirs])
    error("not implemented, yet")
end

lipschitz(n1::Int, n2::Int, dn::Int)::Float64 =
    lipschitz(n1, n2, compute_dirs(dn))


end
