Tree Graphs
==========

```@docs
GraphIdx.Tree
```

For computing a spanning tree from within a general graph, see [here](mst.md).
For debugging, outputting the tree as a hierarchy was usefule:
```@meta
DocTestSetup  = quote
    import GraphIdx.Tree: ChildrenIndex, hierarchy, dfs_walk, dfs_walk_rev, parenthesis, find_root, dfs_finish
end
```
```@docs
GraphIdx.Tree.hierarchy
GraphIdx.Tree.hierarchy_string
GraphIdx.Tree.parenthesis
GraphIdx.Tree.parenthesis_string
```

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
GraphIdx.Tree.dfs_walk_rev
GraphIdx.Tree.dfs_finish
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
