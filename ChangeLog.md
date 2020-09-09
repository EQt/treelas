ChangeLog
---------
### v0.15.5
Released 2020-09-09
- Deduce whether to check for zero nodes from type parameter
- Name-Mangling in libtreelas
- julia: Some experiments with new algorithms

### v0.15.4
Released 2020-07-17
Collect minor changes over time

### v0.14.1
Released 2019-11-25
- Fixed Bug in Julia: index shift

### v0.14.0
Released 2019-11-11
- Guiding ideas: Unify all kind of `clip_{front,back,fronw,backw}` type of methods
- New means to measure runtime more accurately
- Compare LineLas implementations in C++, Julia and Rust
- Testcases for weighted `line_las`.
- Several simplifications/unifications of old code.

### v0.13.0
Released 2019-09-13
GraphIdx becomes an independent Git-Module

### v0.12.9
Released 2019-09-04
- C++: Re-implement `line_dp` to support arbitrary weights.

### v0.12.8
Released 2019-09-02
- Python: More tree test instances; simplified `treelas.tree_dp`

### v0.12.7
Released 2019-09-02
- Python: `tree.py`: Test read/write of TreeInstance

### v0.12.6
Released 2019-08-27
- Python: `linelas.py`: Latent nodes and `lam` parameter work

### v0.12.5
Released 2019-08-27
- Python: Implement the unified `clip` as template for C++/Rust
- Include line test instances

### v0.12.4
Released 2019-05-21
- C++: Prepare to remove Event with 3 variables
- Python: PEP8, PyFlakes

### v0.12.3
Release 2019-05-20
- Julia: `MGT.gaplas` got a new `learn` parameter to control the portion to update the dual solutioon.
- Documentation extended.

### v0.12.2
- Julia: NeighborIndex --> IncidenceIndex
- Julia: generalized weighted median

### v0.12.1
- Julia: Fix duality gap computation (match C++ code)
- Julia: Several sanity checks in `gaplas`

### v0.12.0
- Julia: Refactored mgt.jl to reduce the input needed
- Julia: Make MGT callable on grid graphs

### v0.11.11
Released 2019-02-13
- Julia: Re-Factor/Re-Implement `tree_dp.jl`
- Julia/Rust: Doctests

### v0.11.10
Released 2019-02-08
- GraphIdx: C++ documentation via Sphinx + Doxygen + Breathe.

### v0.11.9
Released 2019-02-08
- Julia: Started documentation
- Julia: Refactored Code into submodules
- Julia: Unit-Tests on Travis-CI

### v0.11.8
Released 2019-02-06
- Julia: Grid graph features such as `enumerate_edges(::GridGraph)`

### v0.11.7
Released 2019-01-31
- ChildrenIndex provides access to root node and has same layout in C++ and Julia
- Julia: `lowest_common_ancestors`

### v0.11.6
Released 2019-01-31
- CXX: started to implement van Emde Boas layout
- CXX: copied sparse CSC matrix

### v0.11.5
Released 2019-01-28
- CXX: dissolve `treelas/utils`

### v0.11.4
Released 2019-01-28
- CXX: graphidx is header only library

### v0.11.3
Released 2019-01-27
- Several bug fixes, restructures, documentation
- CXX: `hyperfloor`

### v0.11.2
Released 2019-01-11
- Julia: Reimplement and test `kruskal_mst`

### v0.11.1
Released 2019-01-07
- Julia: Maximum Gap Tree seems to work

### v0.11.0
- CXX: moved most general graph code to `graphidx`


### v0.10.4
Released 2019-01-07
- Python: started on "lowest common ancestor" (`lca.py`)

### v0.10.3
Released 2018-12-29
- Julia: `dp_tree`
- CXX: Test `kruskal_mst`

### v0.10.2
Released 2018-12-20
- Iterator over `PartitionIndex`

### v0.10.1
Released 2018-12-20
- `cluster`

### v0.10.0 
Released 2018-12-19
- `PartitionIndex`, `groupby`
