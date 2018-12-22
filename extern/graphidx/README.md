Index-Based Graphs
==================

In most graph algorithms, it is convinient to have access to the neighboring nodes for a node.
The corresponding data structure, adajency lists, however is very inpractical to efficiently store in memory.
So instead of storing
```
[ TODO: create an example ]
```
we concatenate the adjacencies, resulting in a single vector of length `m`, and create an index containing the first element for each node (`n + 1` elements).

This data structure is usually found in sparse matrices like CSC or CSR format.
Here, we use it to store
- Children in a rooted tree.
- Partitions of numbers `0..n`
- Neighbors in a bidirectional graph
