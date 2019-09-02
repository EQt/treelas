from __future__ import annotations
import h5py
import warnings
import numpy as np
from numba import njit

from . import _treelas as _tl
from .graphviz import show_tree
from .prufer import prufer_from_children_spec
from .children import PyChildrenIndex as ChildrenIndex


@njit(cache=True)
def find_root(parent):
    """Return the smallest i with parent[i] == i"""
    for i, p in enumerate(parent):
        if i == p:
            return i


class Tree:
    """Rooted tree, stored as a parent np.array"""
    def __init__(self, parent, root=-1, n=None):
        self.parent = np.asarray(parent, dtype=np.int32)
        if isinstance(n, int):
            assert len(self.parent) == n
        if root < 0:
            root = _tl.find_root(parent)
            assert root >= 0
            assert root < len(parent)
        self.root = np.int32(root)
        self.childidx = None
        assert self.parent[self.root] == self.root

    @property
    def children(self):
        if self.childidx is None:
            self.childidx = ChildrenIndex.compute(self.parent, self.root)
        return self.childidx

    @property
    def degree(self):
        degs = np.diff(self.children.idx) + 1
        degs[-1] -= 1
        return degs

    @property
    def n(self):
        """Number of nodes"""
        return len(self.parent)

    def __repr__(self):
        return f"""
Tree(n={self.n},
     root={self.root},
     parent={repr(self.parent)})""".strip()

    def show(self, wait=True):
        """Show the tree using graphviz' dot"""
        show_tree(self.parent, wait=wait)
        return self

    @staticmethod
    def from_prufer(prufer):
        """If choose root by the Prüfer sequence"""
        parent, root = _tl.prufer2parent(np.asarray(prufer, dtype=np.int32))
        return Tree(parent, root=root)

    @classmethod
    def generate(cls, degrees, seed=42):
        return cls.from_prufer(prufer_from_children_spec(degrees, seed=seed))

    @staticmethod
    def random(n, seed=None):
        if seed is not None:
            np.random.seed(seed)
        t = Tree.from_prufer(np.random.randint(0, n, size=n-2, dtype=np.int32))
        return t

    @staticmethod
    def load(fname):
        with h5py.File(fname) as io:
            parent = io['parent'][:]
            root = io['root'][:] if 'root' in io else _tl.find_root(parent)
        return Tree(parent=parent, root=root)


class TreeInstance(Tree):
    """Fused lasso tree instance (including all weight-parameters)"""
    def __init__(self, y, parent, lam, mu=1.0, root=-1):
        assert isinstance(lam, float) or lam.dtype == np.float64
        assert isinstance(mu, float) or mu.dtype == np.float64
        super().__init__(parent, root)
        self.y = np.asarray(y, dtype=np.float64)
        self.lam = lam
        self.mu = mu
        self.x = None
        self.z = None
        self.alpha = None
        self._gamma = None
        self._postord = np.array([], dtype=np.int32)
        assert self.y.dtype == np.float64

    def __repr__(self):
        return f"""TreeInstance(root={self.root},
     y = {repr(self.y)},
    mu = {repr(self.mu)},
   lam = {repr(self.lam)},
parent = {repr(self.parent)})"""

    def solve(self, verbose=False, x=None, **args):
        """Compute a primal solution self.x (return self)"""
        if isinstance(self.lam, float) and isinstance(self.mu, float):
            assert self.mu > 0
            self.x = _tl.tree_dp(y=self.y,
                                 parent=self.parent,
                                 lam=self.lam/self.mu,
                                 root=self.root,
                                 x=x,
                                 verbose=verbose,
                                 **args)
        else:
            self.x = _tl.tree_dp_w(y=self.y,
                                   parent=self.parent,
                                   lam=self.lam,
                                   mu=self.mu,
                                   root=self.root,
                                   x=x,
                                   verbose=verbose,
                                   **args)
        return self

    def dsolve(self, max_iter=20):
        raise NotImplementedError()

    @property
    def y0(self):
        y0 = self.y.copy()
        y0[np.isnan(y0)] = 0.0
        return y0

    @property
    def dual(self):
        """
        Dual solution, corresponding to self.x.
        If called the first time, compute it.
        """
        if self.z is None:
            if self.x is None:
                self.solve()
            self.z = self.mu * (self.x - self.y0)

        if self.alpha is None or not np.isnan(self.alpha[self.root]):
            self.alpha = _tl.tree_dual(parent=self.parent,
                                       z=self.z.copy(),
                                       post_ord=self._postord,
                                       root=self.root,
                                       alpha=self.alpha)
        return self.alpha

    @property
    def gamma(self, root_val=0.0):
        """Compute the duality gap vector"""
        alpha = self.dual
        if self._gamma is None:
            self._gamma = np.empty_like(alpha)
        self._gamma = _tl.tree_dual_gap(self.x, alpha, self.lam, self.parent,
                                        root_val, gamma=self._gamma)
        return self._gamma

    @property
    def dual_diff(self):
        a = self.dual
        diff = self.lam - np.abs(a)
        diff[self.root] = 0.0
        return diff

    @property
    def n(self):
        """Number of nodes"""
        return len(self.parent)

    def save(self, fname, group="/", overwrite=True, compression=4):
        """Store in HDF5 file format"""
        with h5py.File(fname, 'a') as io:
            if group != "/":
                if group in io:
                    if not overwrite:
                        raise RuntimeError(
                            f"{group} already exists in {fname}. Overwrite?")
                    else:       # overwrite
                        del io[group]
                io = io.create_group(group)
            to_write = dict(
                parent=self.parent,
                y=self.y,
                lam=self.lam,
                mu=self.mu,
                root=self.root)
            for n, v in {'x': self.x, 'alpha': self.alpha}.items():
                if v is not None:
                    to_write[n] = v

            if self.mu == 1.0:
                del to_write['mu']
            if isinstance(self.lam, float):
                to_write['lam'] = [to_write['lam']]

            for n, v in to_write.items():
                if n in io:
                    if overwrite:
                        del io[n]
                    else:
                        raise RuntimeError(f"Allow to delete {n} in {fname}?")
                if n in ["root"]:       # support compression?
                    io.create_dataset(n, data=v)
                else:
                    io.create_dataset(n, data=v, compression=compression)

    def objective(self, x: np.ndarray) -> float:
        """Compute the primal objective value for `x`"""
        assert x.size == self.n
        raise NotImplementedError()

    @staticmethod
    def load(fname: str, group: str = "/") -> TreeInstance:
        """Load a tree instance from a HDF5 file"""
        with h5py.File(fname) as io:
            io = io[group]
            parent = io["parent"][:].astype(np.int32)
            assert parent.max() <= len(parent)
            y = io["y"][:].astype(np.float64).reshape(-1)
            if "lam" not in io:
                raise RuntimeError(f"no lam found in {fname}:{group}")
            lam = io["lam"].value
            if len(lam) == 1:
                lam = lam[0]
            mu = None
            if "mu" in io:
                mu = io["mu"].value
            else:
                warnings.warn(f"No µ found in {fname}:{group}: set µ := 0.5")
                mu = 0.5
            root = -1
            if "root" in io:
                root = io["root"].value
            else:
                root = find_root(parent)
            x = io["x"].value if "x" in io else None
            alpha = io["alpha"].value if "alpha" in io else None
        t = TreeInstance(y=y, parent=parent, lam=lam, mu=mu, root=root)
        t.x = x
        t.alpha = alpha
        return t
