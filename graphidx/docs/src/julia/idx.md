Indexes
=======

General Graphs
-------------

```@docs
GraphIdx.IncidenceIndex
GraphIdx.num_edges(::GraphIdx.IncidenceIndex)
```

Weights
-------

Often, nodes or edges might have an associated weight.
To give the optimizer the possibility to distinguish constant weights (all weights the same) from individual weights we provide the following structs:

```@meta
DocTestSetup  = quote
    import GraphIdx
end
```
```@docs
GraphIdx.ConstantWeights
GraphIdx.ArrayWeights
```

Graph Partition
--------------

```@autodocs
Modules = [GraphIdx.Cluster]
```


Tree Graphs
----------

```@meta
DocTestSetup  = quote
    import GraphIdx.Tree: ChildrenIndex
end
```
```@docs
GraphIdx.Tree.ChildrenIndex
GraphIdx.Tree.root_node(::GraphIdx.Tree.ChildrenIndex)
```
Once, memory has been allocated we can re-use it to store different tree (of same size!) in it.
```@docs
GraphIdx.Tree.reset!(::GraphIdx.Tree.ChildrenIndex, ::Vector{Int})
```
