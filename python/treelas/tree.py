import h5py
import warnings
import numpy as np
from numba import njit

from . import _treelas as _tl
from .graphviz import show_tree
from .prufer import prufer_from_children_spec


class Tree:
    """Rooted tree, stored as a parent np.array"""
    def __init__(self, parent, root=0):
        assert parent.dtype == np.int32
        self.parent = parent
        self.root = np.int32(root)

    def show(self, wait=True):
        """Show the tree using graphviz' dot"""
        show_tree(self.parent, wait=wait)
        return self

    @property
    def n(self):
        """Number of nodes"""
        return len(self.parent)

    def __repr__(self):
        return f"""
Tree(n={self.n},
     root={self.root},
     parent={repr(self.parent)})""".strip()

    @staticmethod
    def from_prufer(prufer):
        """If choose root by the prufer sequence"""
        parent, root = _tl.prufer2parent(prufer.astype(np.int32, copy=False))
        return Tree(parent, root=root)

    @classmethod
    def generate(cls, degrees, seed=42):
        return cls.from_prufer(prufer_from_children_spec(degrees, seed=seed))
    

@njit
def find_root(parent):
    """Return the smallest i with parent[i] == i"""
    for i, p in enumerate(parent):
        if i == p:
            return i


class TreeInstance(Tree):
    """Fused lasso tree instance (including all weight-parameters)"""
    def __init__(self, y, parent, lam, mu, root=0):
        assert y.dtype == np.float64
        assert isinstance(lam, float) or lam.dtype == np.float64
        assert isinstance(mu, float) or mu.dtype == np.float64
        super().__init__(parent, root)
        self.y = y
        self.lam = lam
        self.mu = mu
        self.x = None
        self.z = None
        self.alpha = None
        self._gamma = None

    def __repr__(self):
        return f"""TreeInstance(root={self.root}):
     y = {repr(self.y)}
    mu = {repr(self.mu)}
   lam = {repr(self.lam)}
parent = {repr(self.parent)}"""

    def solve(self, verbose=False):
        if isinstance(self.lam, float) and isinstance(self.mu, float):
            assert self.mu > 0
            self.x = _tl.tree_dp(y=self.y,
                                 parent=self.parent,
                                 lam=self.lam/self.mu,
                                 root=self.root,
                                 verbose=verbose)
        else:
            self.x = _tl.tree_dp_w(y=self.y,
                                   parent=self.parent,
                                   lam=self.lam,
                                   mu=self.mu,
                                   root=self.root,
                                   verbose=verbose)
        self.y0 = self.y.copy()
        self.y0[np.isnan(self.y0)] = 0.0
        self.z = 2.0 * self.mu * (self.x - self.y0)
        return self

    def dsolve(self, max_iter=20):
        raise NotImplementedError()

    @property
    def dual(self):
        if self.alpha is None:
            if self.x is None:
                self.solve()
            self.alpha = _tl.tree_dual(self.parent, self.z, root=self.root,
                                       alpha=self.alpha)
        return self.alpha

    @property
    def gamma(self, root_val=0.0):
        """Compute the duality gap vector"""
        if self._gamma is None:
            self._gamma = np.empty(0)
        alpha = self.dual
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
            for n, v in to_write.items():
                if n in io:
                    if overwrite:
                        del io[n]
                    else:
                        raise RuntimeError(f"Allowed to delete {n} in {fname}?")
                if n in ["root"]:       # support compression?
                    io.create_dataset(n, data=v)
                else:
                    io.create_dataset(n, data=v, compression=compression)

    def objective(self, x):
        assert x.size == self.n
        raise NotImplementedError()

    # def show(self, wait=True):
    #     """Show the tree using graphviz' dot"""
    #     raise NotImplementedError()
    #     # from dilas._graphviz import graphviz_ladder as graphviz
    #     # lat = np.where(self.mu <= 0)[0]
    #     # r = ' fontsize=8'
    #     # graphviz(self.parent, range(self.n), out='neato', latent=lat, wait=wait,
    #     #          edge_label=lambda j: f'label="{j}: {self.lam[j]:.2f}"' + r)
    #     # return self

    def load(fname, group="/"):
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
                warnings.warn(f"No mu found in {fname}:{group}: setting mu == 0.5")
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
