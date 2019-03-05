Tree Fused Lasso
===============

```@autodocs
Modules = [TreeLas]
```

Exact Tree Solver
----------------

The exact solver is found in [`TreeLas.TreeDP`](@ref).
```@autodocs
Modules = [TreeLas.TreeDP]
```


General Graphs
--------------

Furtheron, we presend a general graph solver that iteratively picks a subtree and block-optimizes it.
```@autodocs
Modules = [TreeLas.GapLas]
```


Utility Functions
----------------

```@meta
DocTestSetup  = quote
    import TreeLas.Utils: sum2
end
```
```@autodocs
Modules = [TreeLas.Utils]
```
