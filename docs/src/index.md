Fused Lasso on Trees
==================

This is the documentation of the Julia part.
For other languages, have a look at
- [C++](../cxx/index.html)
- [Python](../python/index.html)
- [Rust](../rust/graphidx/index.html)


```@autodocs
Modules = [TreeLas]
```

Exact Tree Solver
----------------

The exact solver is found in the module [`TreeLas.TreeDP`](@ref).


General Graphs
--------------

Furtheron, we presend a general graph solver that iteratively picks a subtree and block-optimizes it.
```@autodocs
Modules = [TreeLas.MGT]
```


Utility Functions
----------------

```@autodocs
Modules = [TreeLas.Utils]
```
