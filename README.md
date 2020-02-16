TreeLas
=======
[![Travis](https://travis-ci.com/EQt/treelas.svg?token=WXPT4d6dD68rQ9ty7yDf&branch=master)](https://travis-ci.com/EQt/treelas)

![logo](https://eqt.github.io/treelas/imgs/treelas.png)

Fused Lasso solvers for tree graphs.
We exactly compute the minimizer of the convex function

![formula](https://eqt.github.io/treelas/imgs/formula.svg)

whereby _E_ are the edges of a tree graph.

![demo](docs/phantom_w200_0_lam2e-1_pad10.png)
**Demo** (from left to right)
1. [Original Image][phantom]
2. Gaussian noise added (std=0.25)
3. _E_ is grid graph (_λ_ = 0.2)
4. _E_ is random spanning tree (_λ_ = 0.2)


Python Interface
----------------

... is realized via
[`pybind11`](https://github.com/pybind/pybind11).
For compiling, add the Git submodule
```bash
git submodule update --init --recursive deps/graphidx
```

Afterwards build the python extension by calling [`setup.py`](python/setup.py)
```bash
cd python && python setup.py develop
```


Directory Structure
-------------------

```
├── ci          Continous integration
├── cxx         C++ source/header files
├── data        Some test data and benchmarks
├── deps        External dependencies/libraries
├── julia       Julia Bindings/Implementation
├── rust        Rust re-implementation
└── python      Python bindings
```

Line Graphs
-----------

There are several implementations to be able assess e.g. different memory layouts, traversing orders, etc.

We  also provide specialized algorithms for one dimensional **line graphs** (also called “trail graphs”). 
For the sake of benchmarks the implementation by [Laurent Condat][condat] and the [`glmgen` R package][glmgen] are included as well.


[condat]: https://lcondat.github.io/
[glmgen]: https://github.com/glmgen/glmgen
[phantom]: https://en.wikipedia.org/wiki/Shepp%E2%80%93Logan_phantom
