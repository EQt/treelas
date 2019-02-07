Indexes
=======

General Graphs
-------------

```@docs
GraphIdx.NeighborIndex
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
```
Once, memory has been allocated we can re-use it to store different tree (of same size!) in it
```@docs
GraphIdx.Tree.reset!(::GraphIdx.Tree.ChildrenIndex, ::Vector{Int})
```
