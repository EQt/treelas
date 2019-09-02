import numpy as np
import toml


class NumPyEncoder(toml.TomlEncoder):
    """Encode numpy arrays similar to lists, not as list of str"""
    def __init__(self):
        super().__init__()
        self.dump_funcs[np.int64] = self.dump_funcs[int]
        self.dump_funcs[np.int32] = self.dump_funcs[int]
        self.dump_funcs[np.uint64] = self.dump_funcs[int]
        self.dump_funcs[np.uint32] = self.dump_funcs[int]
        self.dump_funcs[np.float32] = self.dump_funcs[float]
        self.dump_funcs[np.float64] = self.dump_funcs[float]
        self.dump_funcs[np.ndarray] = self.dump_funcs[list]


def dumps(o):
    return toml.dumps(o, encoder=NumPyEncoder())


def dump(o, f):
    f.write(dumps(o))
