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
Capture all we need to know about a grid graph.
"""
struct GridGraph
    n1::Int
    n2::Int
    dirs::Vector{Pixel}
end


GridGraph(n1, n2, dn::Int = Int(1)) =
    GridGraph(n1, n2, compute_dirs(dn))


num_edges(g::GridGraph) =
    num_edges(g.n1, g.n2, g.dirs)


num_nodes(g::GridGraph) = g.n1 * g.n2


"""
    iter_edges_pixel(proc, n1, n2, dirs)

Call `proc(i1, j1, i2, j2, len)` for every grid edge
`(i1, j1) -- (i2, j2)` having `len`gth.
"""
function iter_edges_pixel(proc::Function, n1::Int, n2::Int, dirs::Vector{Pixel})
    for d in dirs
        len = 1/norm(d)
        for i = 1:(n1-d.x)
            for j = 1:(n2-d.y)
                proc(i, j, i+d.x, j+d.y, len)
            end
        end
        for i = 1+d.y:n1
            for j = 1:(n2-d.x)
                proc(i, j, i-d.y, j+d.x, len)
            end
        end
    end
end


iter_edges_pixel(f::Function, g::GridGraph) = 
    iter_edges_pixel(f, g.n1, g.n2, g.dirs)


"""
    iter_edges(proc, n1, n2, dirs)

Call `proc(u::Int, v::Int, len::Float64)` for every grid
edge `u -- v` having `len`gth.
Hereby `u` and `v` are the index of the corresponding grid-nodes.
"""
function iter_edges(proc::Function, n1::Int, n2::Int, dirs::Vector{Pixel})
    """Fortran index of the matrix entry `(i,j)`"""
    pix2ind(i, j) = i + (j-1)*n1

    iter_edges_pixel(n1, n2, dirs) do i1, j1, i2, j2, len::Float64
        proc(pix2ind(i1, j1), pix2ind(i2, j2), len)
    end
end


iter_edges(f::Function, g::GridGraph) =
    iter_edges(f, g.n1, g.n2, g.dirs)


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
    dirs = compute_dirs(dn)
    n = n1 * n2
    m = num_edges(n1, n2, dirs)

    I = Vector{Int}(undef, 2m)
    J = Vector{Int}(undef, 2m)
    W = zeros(Float64, 2m)

    k = Int(1)   # edge index
    iter_edges(n1, n2, dirs) do u::Int, v::Int, len::Float64
        I[2k-1] = k
        J[2k-1] = u
        W[2k-1] = +len
        I[2k-0] = k
        J[2k-0] = v
        W[2k-0] = -len
        k +=1
    end
    D = sparse(I, J, W, m, n)
end


function adjlist(n1::Int, n2::Int, dn::Int = 1)
    dirs = compute_dirs(dn)
    m = num_edges(n1, n2, dirs)
    head = Vector{Int}(undef, m)
    tail = Vector{Int}(undef, m)
    lam =  Vector{Float64}(undef, m)

    k = Int(1)
    iter_edges(n1, n2, dirs) do u::Int, v::Int, len::Float64
        head[k] = u
        tail[k] = v
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
function lipschitz(n1, n2, dirs::Vector{Pixel})
    lmax = maximum(lam)
    Lip = lmax * 2 * 4 * sum([l for (d,l) in dirs])
    error("not implemented, yet")
end

lipschitz(n1::Int, n2::Int, dn::Int)::Float64 =
    lipschitz(n1, n2, compute_dirs(dn))


function neighbors_lambda(g::GridGraph)
    n = num_nodes(g)
    idx = zeros(Int, n+1)
    local m = 0
    iter_edges(g) do h::Int, t::Int, _::Float64
        m += 1
        idx[h] += 1
        idx[t] += 1
    end
    acc = 1                        # accumulate degrees ==> positions
    deg_i = 0
    deg_ii = idx[1]
    for i = 1:n
        idx[i] = acc
        acc += deg_i
        deg_i, deg_ii = deg_ii, idx[i+1]
    end
    idx[n+1] = acc
    @assert(idx[end] + deg_i == 2m + 1,
            "idx[$(length(idx))]: $(idx[end] + deg_i) != $(2m + 1)")
    pi = Vector{Tuple{Int,Int}}(undef, 2m)
    lam = Vector{Float64}(undef, m)
    local i = 0
    iter_edges(g) do u::Int, v::Int, lam_i::Float64
        i += 1
        lam[i] = lam_i
        pi[idx[u+1]] = (v, i)
        idx[u+1] += 1
        pi[idx[v+1]] = (u, i)
        idx[v+1] += 1
    end
    @assert(idx[end] == 2m + 1, "$(idx[end]) vs $(2m + 1)")
    return NeighborIndex(idx, pi), lam
end

end
