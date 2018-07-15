Treelas
=======
[![Travis](https://travis-ci.com/EQt/treelas.svg?token=WXPT4d6dD68rQ9ty7yDf&branch=master)](https://travis-ci.com/EQt/treelas)


Fused Lasso solver for tree graphs, implemented in C++ with Python bindings.
We exactly compute the minimizer of the convex function

_f_(_x_) := 
  ∑<sub>_v_ ∈ _V_</sub> _μ_<sub>_v_</sub> (_x_<sub>_v_</sub> - _y_<sub>_v_</sub>)<sup>2</sup>
+ ∑<sub>(_u_,_v_) ∈ _E_</sub>_λ_<sub>_uv_</sub> |_x_<sub>_u_</sub> - _x_<sub>_v_</sub>|

whereby _E_ are the edges of a tree.


Python Interface
---------------

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
------------------

```
├── cxx         C++ source/header files
├── data        Some test data (needed for `py.test`)
├── extern      External libraries
├── python      Python bindings
└── test        GTest cases
```
