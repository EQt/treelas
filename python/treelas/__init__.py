"""
TreeLas: a Fused Lasso Solver for Trees
=======================================

The software herein aims to **minimize** the following objective function

.. math::
   f(x) = \\frac{1}{2} \\sum_{i=1}^n \\mu_i (x_i - y_i)^2 + \\sum_{(i,j) \\in E} \\lambda_{ij} |x_i - x_j|

Hereby is
 * :math:`y_i` the input signal for node :math:`i`,
 * :math:`\\mu_i \\geq 0` is the node weight for node :math:`i`,
 * :math:`E` the edges, usually from a tree, and
 * :math:`\\lambda_{ij} > 0` is the edge weight for :math:`(i,j)`.


Tree Edges
----------

Information on tree edges (e.g. lambda) are encoded as vector of length n
(albeit there are only n-1 edges).  We do so because than the root node can
be arbitrary and the i-th element lam[i] contains the information for edge
(i, parent[i]).


Details
-------

See https://github.com/eqt/treelas
"""
from .tree import Tree, TreeInstance
from .segment import line_mean
from .children import PyChildrenIndex
from ._treelas import (
    __compiler__,
    __author__,
    line_condat,
    line_glmgen,
    line_las,
    line_dp,
    line_w,
    line_para,
    line_lasc,
    line_las2,
    line_las3,
    find_root,
    tree_dp,
    tree_12x,
    tree_dual,
    tree_dual_gap,
    random_spanning_tree,
    post_order,
    # dp_forward, dp_reverse,
)
