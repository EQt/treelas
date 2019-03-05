Tree Fused Lasso
===============

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

```@meta
DocTestSetup  = quote
    import TreeLas.Utils: sum2, primal_objective
end
```
```@autodocs
Modules = [TreeLas.Utils]
```
