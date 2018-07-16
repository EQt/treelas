"""
Generate random trees via Prüfer sequences.

CURRENT PROBLEMS
----------------

- [ ] The root node seems to be independent of the random seed
      (always 23)
- [x] The root node has always only one child.
   --> for seed=42 root as 4 children
"""
import numpy as np
from numba import njit


@njit(cache=True)
def num_nodes(children_spec):
    n = 0
    for j, d in enumerate(children_spec):
        n += d * (j+2)
    return n+2


def prufer_from_children_spec(children_spec, seed=2018, dtype=np.int32):
    """
    Generate a random Prüfer sequence which obeys the children_spec, e.g.

        children_spec = [2, 4, 1]

    means that there are
     - two nodes with 2 children,
     - four nodes with 3 children, and
     - one node with 4 children.
    """

    @njit(cache=True)
    def _unroll(prufer, perm, children_spec):
        i = 0
        for j, d in enumerate(children_spec):
            for _ in range(d):
                p = perm[i]
                for _ in range(j+2):
                    prufer[i] = p
                    i += 1

    n = num_nodes(children_spec)
    np.random.seed(seed)
    perm = np.random.permutation(n)
    prufer = -np.ones(n-2, dtype=dtype)
    _unroll(prufer, perm, children_spec)
    np.random.shuffle(prufer)
    return prufer
