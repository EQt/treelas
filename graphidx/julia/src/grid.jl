"""
Computations related to grid graphs
"""
module Grid
import SparseArrays
import LinearAlgebra

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

Setting the neighborhood degree (`dn`) to `i` will set all numbers less or equal `i` in the following diagram as neighbors of `[*]`.

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


# """
# `ds` species the directions explicitly (vector d is scaled by 1/norm(d))
# """
# img_graph(n1::Int, n2::Int, ds::Vector{Pixel}) =
#     img_graph(n1, n2, [(d, 1/norm(d)) for d in ds])


#=
function img_graph(n1::Int, n2::Int,
                   dir::Vector{Tuple{Pixel,Float64}}=[(Pixel(1,0), 1.0)])
    n = n1 * n2
    m = 0
    for d in dir
        e = d[1]
        m += (n1-e.x)*(n2-e.y) + (n1-e.y)*(n2-e.x)
    end
    @assert m > 0
    I = zeros(Int, 2m)
    J = zeros(Int, 2m)
    W = zeros(Float64, 2m)
    E = [IEdge(0,0,0) for e=1:m]
    lam = zeros(Float64, m)
    m = 0
    for d in dir
        e = d[1]
        @inbounds for j = 1:n2-e.y
            for i = 1:n1-e.x
                l = m + i + (j-1)*(n1-e.x)
                k = 2l - 1
                v1 = pix2ind(i,j, n1)
                v2 = pix2ind(i+e.x, j+e.y, n1)
                E[l] = IEdge(l, v1, v2)
                lam[l] = d[2]
                I[k] = l
                J[k] = v1
                W[k] = +d[2]
                k += 1
                I[k] = l # same edge
                J[k] = v2
                W[k] = -d[2]
            end
        end
        m += (n1-e.x)*(n2-e.y)
        @inbounds for j = 1:n2-e.x
            for i = 1+e.y:n1
                l = m + i - e.y + (j-1)*(n1-e.y)
                k = 2l -1
                v1 = pix2ind(i,j, n1)
                v2 = pix2ind(i-e.y, j+e.x, n1)
                E[l] = IEdge(l, v1, v2)
                lam[l] = d[2]
                I[k] = l
                J[k] = v1
                W[k] = +d[2]
                k += 1
                I[k] = l # same edge
                J[k] = v2
                W[k] = -d[2]
            end
        end
        m += (n1-e.y)*(n2-e.x)
    end
    @debug @val I
    @debug @val J
    D = sparse(I, J, W, m, n)
    G = simple_edgelist(n1*n2, E; is_directed=false)
    lmax = maximum(lam)
    Lip = lmax * 2 * 4 * sum([l for (d,l) in dir])
    ImgGraph(n1, n2, Lip, lam, G, D, dir)
end
=#
end
