# Dynamic Programming Tree Solver

```@meta
DocTestSetup  = quote
    import TreeLas.TreeDP: Event
end
```
```@docs
TreeLas.TreeDP
TreeLas.TreeDP.tree_dp
TreeLas.TreeDP.tree_dp!
TreeLas.TreeDP.TreeDPMem
TreeLas.TreeDP.reset!
```

## Dual Solution

```@autodocs
Modules = [TreeLas.Dual]
```

## Queues-Structure

The main component is a set of double-ended, mergable priority queues (also called *heaps*).

```@autodocs
Modules = [TreeLas.TreeDP.QueueUnion]
```

