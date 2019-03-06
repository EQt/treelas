Indexes
=======

General Graphs
-------------

```@docs
GraphIdx.NeighborIndex
GraphIdx.num_edges(::GraphIdx.NeighborIndex)
```


Graph Partition
--------------

```@autodocs
Modules = [GraphIdx.Cluster]
```


Tree Graphs
----------

```@docs
GraphIdx.Tree.ChildrenIndex
GraphIdx.Tree.root_node(::GraphIdx.Tree.ChildrenIndex)
```
Once, memory has been allocated we can re-use it to store different tree (of same size!) in it.
```@docs
GraphIdx.Tree.reset!(::GraphIdx.Tree.ChildrenIndex, ::Vector{Int})
```
