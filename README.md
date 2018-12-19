Treelas
=======
[![Travis](https://travis-ci.com/EQt/treelas.svg?token=WXPT4d6dD68rQ9ty7yDf&branch=master)](https://travis-ci.com/EQt/treelas)


Fused Lasso solver for tree graphs, implemented in C++ with Python bindings.
We exactly compute the minimizer of the convex function

_f_(_x_) := 
  ∑<sub>_v_ ∈ _V_</sub> _μ_<sub>_v_</sub> (_x_<sub>_v_</sub> - _y_<sub>_v_</sub>)<sup>2</sup>
+ ∑<sub>(_u_,_v_) ∈ _E_</sub>_λ_<sub>_uv_</sub> |_x_<sub>_u_</sub> - _x_<sub>_v_</sub>|

whereby _E_ are the edges of a tree.

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
├── extern      External libraries
├── python      Python bindings
```


ChangeLog
---------

### v0.10.0  2018-12-19
- `PartitionIndex`, `groupby`



[condat]: https://www.gipsa-lab.grenoble-inp.fr/~laurent.condat
[glmgen]: https://github.com/glmgen/glmgen

