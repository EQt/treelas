Tree Graphs
==========

```@docs
GraphIdx.Tree
```

For computing a spanning tree from within a general graph, see [here](mst.md).


Data Structures
--------------

We provide some framework specialized for (rooted) tree graphs:
```@docs
GraphIdx.Tree.RootedTree
GraphIdx.Tree.find_root
GraphIdx.Tree.node_degrees
```

Often, we will need to access the children of arbitrary nodes; for this the [`GraphIdx.Tree.ChildrenIndex`](@ref) was developed.



Traversal
--------

Classical *depth first search*
```@docs
GraphIdx.Tree.dfs_walk
```

For binary trees, there is also the *inorder*:
```@docs
GraphIdx.Tree.binary_inorder
```


Lowest Common Ancestors
---------------------

```@docs
GraphIdx.Tree.lowest_common_ancestors
```