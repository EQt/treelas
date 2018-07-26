"""
Treelas: a fused lasso solver for trees
---------------------------------------

Information on tree edges (e.g. lambda) are encoded as vector of length n
(albeit there are only n-1 edges).  We do so because than the root node can
be arbitrary and the i-th element lam[i] contains the information for edge
(i, parent[i]).


See https://github.com/eqt/treelas.

"""
import re
from .tree import Tree, TreeInstance
from .children import ChildrenIndex
from ._treelas import (
    __compiler__,
    __author__,
    line_condat,
    line_glmgen,
    line_las,
    line_para,
    line_lasc,
    line_las2,
    line_las3,
    find_root,
    tree_dp,
    tree_dp_w,
    tree_12,
    tree_dual,
    tree_dual_gap,
)
#     post_order,
#     dp_forward, dp_reverse,
#     dp_line_w,
# )
