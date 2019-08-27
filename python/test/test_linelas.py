import numpy as np
import pytest
from os import path
from treelas import linelas


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
        self.y = np.array(data['y'], dtype=float)
        self.lam = float(data['lam'])
        self.nr = type(self)._nr
        self.name = str(data['name']) if 'name' in data else None
        if 'x' in data:
            self.x = np.array(data['x'], dtype=float)

    def __repr__(self):
        fname = path.basename(self.fname) if self.fname else ''
        return f'Instance("{fname}"[{self.nr}])'

    def __len__(self):
        return len(self.y)


def data_dir(*args):
    """Return a path relative to the `data` directory"""
    return path.join(path.dirname(__file__), "..", "..", "data", *args)


insts = Instance.from_toml(data_dir("test", "lines.toml"))


@pytest.mark.parametrize("i", insts)
def test_lines(i):
    x = linelas.line_lasso(y=i.y, lam=i.lam)
    assert np.allclose(x, i.x)