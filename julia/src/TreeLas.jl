"""
Fused Lasso Solver for tree graphs, i.e. we aim to **minimize** the following objective function

```math
f(x) = \\frac{1}{2} \\sum_{i=1}^n \\mu_i (x_i - y_i)^2 + \\sum_{(i,j) \\in E} \\lambda_{ij} |x_i - x_j|
```
Hereby is
- ``y_i`` the input signal for node ``i``,
- ``\\mu_i \\geq 0`` is the node weight for node ``i``,
- ``E`` the edges of a tree, and
- ``\\lambda_{ij} > 0`` is the edge weight for ``(i,j)``.
"""
module TreeLas
import GraphIdx

include("utils.jl")
include("dual.jl")
include("pwl.jl")
include("line_dp.jl")
include("tree_dp.jl")
# include("tree_dp.old.jl")
# include("cxx.jl")
include("mgt.jl")

include("precompile.jl")
_precompile_()

end
