module Utils

import GraphIdx: ConstantWeights, enumerate_edges


"""
    sum2(x)

Sum of squares.

```jldoctest
julia> TreeLas.Utils.sum2([1, 11])
122

```
"""
function sum2(x::Array{F,N})::F where{F<:Real,N}
    local s::F = zero(F)
    for xi in x
        s += xi^2
    end
    s
end


"""
    primal_objective(x, y, graph [, mu])

Compute the objective function, i.e.
```math
\\frac{1}{2} \\sum_{i=1}^n μ_i (x_i - y_i)^2 + \\sum_{(i,j) \\in E} λ_{ij} |x_i - x_j|
```
Hereby the edges (and edge weights ``λ_{ij}``) are obtained via `enumerate_edges(graph)`.
"""
function primal_objective(
    x::Array{F,N},
    y::Array{F,N},
    graph::G,
    mu::F2 = ConstantWeights(1.0),
)::F where {F,N,G,F1,F2}
    @assert length(y) == length(x)
    local n = length(x)
    local obj::Ref{F} = Ref{F}(0.5 * sum(mu(i) * (x[i] - y[i])^2 for i = 1:n))
    enumerate_edges(graph) do ei::Int, u::Int, v::Int, lam::Float64
        obj[] += lam * abs(x[u] - x[v])
    end
    return obj[]
end


end
