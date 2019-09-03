"""
Line test instances
"""
import numpy as np
from os import path
from data import test_dir


class Instance:
    """
    Fused Lasso line instance.
    """
    _nr = 0

    @staticmethod
    def from_toml(fname):
        import toml

        doc = toml.load(open(fname))
        Instance._nr = 1
        return [Instance(i, fname=fname) for i in doc["test"]]

    def __init__(self, data, fname=None):
        type(self)._nr += 1
        assert isinstance(data, dict)
        self.fname = fname
        for d in ['lam', 'mu', 'x', 'y']:
            if d in data and isinstance(data[d], list):
                data[d] = np.array(data[d])
        self.y = np.array(data['y'], dtype=float)
        self.lam = data['lam']
        self.mu = data.get('mu', 1.0)
        self.nr = type(self)._nr
        self.name = str(data['name']) if 'name' in data else None
        if 'x' in data:
            self.x = np.array(data['x'], dtype=float)

    def __repr__(self):
        fname = path.basename(self.fname) if self.fname else ''
        return f'Instance("{fname}"[{self.nr}])'

    def __len__(self):
        return len(self.y)


insts = Instance.from_toml(test_dir("lines.toml"))
