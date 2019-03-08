Minimum Spanning Trees
====================

Prim's Algorithm
---------------

```@docs
GraphIdx.prim_mst_edges
```
The runtime is ``\mathcal{O}(|E| \log |V|)`` whereby ``|E|`` are the number of edges and ``|V|`` the number of nodes.
We use an ordinary binary heap as priority queue:
```@autodocs
Modules = [GraphIdx.Utils.Heap]
```


Kruskal's Algorithm
------------------

```@docs
GraphIdx.kruskal_mst
```

As you know, Kruskal's algorithm is best implemented using the [`UnionFind`](@ref GraphIdx.Utils.UnionFind) data structure which is useful on its own:

```@docs
GraphIdx.Utils.UnionFind
```
