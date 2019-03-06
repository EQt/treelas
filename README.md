TreeLas
=======
[![Travis](https://travis-ci.com/EQt/treelas.svg?token=WXPT4d6dD68rQ9ty7yDf&branch=master)](https://travis-ci.com/EQt/treelas)


Fused Lasso solver for tree graphs, implemented in C++ with Python bindings.
We exactly compute the minimizer of the convex function

```math
f(x) = \\frac{1}{2} \\sum_{i=1}^n \\mu_i (x_i - y_i)^2 + \\sum_{(i,j) \\in E} \\lambda_{ij} |x_i - x_j|
```
whereby _E_ are the edges of a tree graph.

There are several implementations to be able assess e.g. different memory layouts, traversing orders, etc.

We  also provide specialized algorithms for one dimensional **line graphs** (also called “trail graphs”). 
For the sake of benchmarks the implementation by [Laurent Condat][condat] and the [`glmgen` R package][glmgen] are included as well.



Python Interface
----------------

... is realized via
[`pybind11`](https://github.com/pybind/pybind11).
For compiling, add the Git submodule
```bash
git submodule update --init extern/pybind11
```
Afterwards, you can build the library using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build . --target pysetup
```
or using `setup.py`:
```bash
cd python
python setup.py develop
```


Directory Structure
-------------------

```
├── ci          Continous integration
├── cxx         C++ source/header files
├── data        Some test data (needed for `py.test`)
├── deps        External dependencies/libraries
├── graphidx    (Almost) independent graph library
├── julia       Julia Bindings/Implementation
└── python      Python bindings
```


ChangeLog
---------


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


[condat]: https://www.gipsa-lab.grenoble-inp.fr/~laurent.condat
[glmgen]: https://github.com/glmgen/glmgen
