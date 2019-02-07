Tree Graphs
==========

For computing a spanning tree from within a general graph, see [here](mst.md).


Data Structures
--------------

We provide some framework specialized for (rooted) tree graphs:
```@docs
GraphIdx.Tree
GraphIdx.find_root
GraphIdx.node_degrees
```

Often, we will need to access the children of arbitrary nodes; for this the [`GraphIdx.ChildrenIndex`](@ref) was developed.



Traversal
--------

```@docs
GraphIdx.dfs_walk
```
