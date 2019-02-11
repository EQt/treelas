![](/logo.png)

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
The transformation from above to below is done using a linear time integer sorting procedure (called [Counting Sort](https://en.wikipedia.org/wiki/Counting_sort)), i.e. it is quite fast.
However afterwards, we cannot add further new edges (unless we reserved extra space for that; not implemented at the moment).


This kind of data structure we use to store "adajacency information" is usually found in sparse matrices like CSC or CSR format (the Rust crate
[`petgraph`](https://github.com/bluss/petgraph)
calls this structure therefore 
[`Csr`](https://docs.rs/petgraph/0.4.13/petgraph/csr/struct.Csr.html)).
Here, we use it to store
- Children in a rooted tree.
- Partitions of numbers `0..n`
- Neighbors in a bidirectional graph

The advantage of storing the values and indices separately are
- only one allocation
- avoid the additional space for vector implementation (e.g. an empty vector consumes only one integer)
- contiguous memory, hence we may benefit from prefetched cache lines
- prefer pointers to array indexes

This library will never be as extensive as e.g. 
[C++'s Boost Graph Library](https://www.boost.org/doc/libs/1_69_0/libs/graph/doc/index.htlm) or
[Julia's LightGraphs](https://github.com/JuliaGraphs/LightGraphs.jl).


Integers
-------

All nodes have to be integer numbers.
Because often we store extra information in the first bit, we require signed integers (typically `int32_t` or `int64_t`).


Programming Languages
---------------------

This is a multilanguage project.
We support bindings to Python.
Parts are rewritten in Rust.

