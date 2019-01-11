Index-Based Graphs
==================

In most graph algorithms, it is convinient to have access to the neighboring nodes for a node.
The corresponding data structure, adajency lists, however is very inpractical to efficiently store in memory.
So instead of storing
```
[ TODO: create an example ]
```
we concatenate the adjacencies, resulting in a single vector of length `m`, and create an index containing the first element for each node (`n + 1` elements):
```
[ TODO: transformed graph ]
```

This data structure is usually found in sparse matrices like CSC or CSR format (the Rust crate
[`petgraph`](https://github.com/bluss/petgraph)
calls this structure therefore 
[`Csr`](https://docs.rs/petgraph/0.4.13/petgraph/csr/struct.Csr.html)).
Here, we use it to store
- Children in a rooted tree.
- Partitions of numbers `0..n`
- Neighbors in a bidirectional graph


Programming Languages
---------------------

This is a multilanguage project.
We support bindings to Python.
Parts are rewritten in Rust.

