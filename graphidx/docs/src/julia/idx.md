Indexes
=======

General Graphs
-------------

```@docs
GraphIdx.IncidenceIndex
GraphIdx.num_edges(::GraphIdx.IncidenceIndex)
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
