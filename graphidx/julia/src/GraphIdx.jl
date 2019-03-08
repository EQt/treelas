"""
Index-based Graphs
==================

Nodes
-----

A node (sometimes also *vertex* called) in a graph is always represented as an integer.
To be able to use a node as an index, the nodes should be within `1:n` whereby `n` is the number of nodes.
Furtheron, we always use **signed** integers because we might include extra information within the first (sign) bit.


Edges
-----

By convention, a graph is considered *undirected* (unless specified else).
Hence we only need to store one direction of every edge `(u, v)`; by default we assume `u < v`.

TODO:
- differences
- incidence matrix

If any *flow* `α``for an edge `(u, v)` has to be computed, the orientation is defined as going from `v` to `u` (big to small), i.e. we compute

```julia
x[u] += α
x[v] -= α
```

Submodules
----------

The most important submodules are
- [`Tree`](@ref): Rooted Trees (including minimum spanning trees).
- [`Grid`](@ref): Specialized code for grid graphs.
- [`LinA`](@ref): Linear algebra (e.g. incidence matrix)
- [`Utils`](@ref): Priority queues (e.g. binary heap)
"""
module GraphIdx

module Utils

include("utils/unionfind.jl")
include("utils/heap.jl")

end


include("cluster.jl")
include("neighbors.jl")
include("tree.jl")
include("weightedgraph.jl")
include("prim_mst.jl")
include("kruskal_mst.jl")
include("lina.jl")
include("grid.jl")
include("grididx.jl")
include("bits.jl")

end
