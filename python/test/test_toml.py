"""
There are some issues to be taken into account when dumping TOML from numpy arrays
"""
import toml
import numpy as np
from treelas import _toml as noml


def test_dump_np():
    l = [5]
    o = np.array(l)
    assert toml.dumps({'o': l}) == 'o = [ 5,]\n'
    assert noml.dumps({'o': o}) == 'o = [ 5,]\n'
    enc = toml.TomlEncoder()
    enc.dump_funcs[np.int64] = enc.dump_funcs[int]
    assert toml.dumps({'o': o}, encoder=enc) == 'o = [ 5,]\n'


def test_dump_ndarray():
    l = [1, 2]
    o = np.array(l)
    assert toml.dumps({'a': {'o': l}}) == '[a]\no = [ 1, 2,]\n'
    assert noml.dumps({'a': {'o': o}}) == '[a]\no = [ 1, 2,]\n'
    enc = toml.TomlEncoder()
    enc.dump_funcs[np.int64] = enc.dump_funcs[int]
    enc.dump_funcs[np.ndarray] = enc.dump_funcs[list]
    assert toml.dumps({'a': {'o': o}}, encoder=enc) == '[a]\no = [ 1, 2,]\n'


def test_dump_ndarray_nested():
    l = [1.2, 2.2]
    o = np.array(l)
    assert toml.dumps({'a': [{'o': l}]}) == '[[a]]\no = [ 1.2, 2.2,]\n\n'
    assert noml.dumps({'a': [{'o': o}]}) == '[[a]]\no = [ 1.2, 2.2,]\n\n'
    enc = toml.TomlEncoder()
    enc.dump_funcs[np.float64] = enc.dump_funcs[int]
    enc.dump_funcs[np.ndarray] = enc.dump_funcs[list]
    assert toml.dumps({'a': [{'o': o, 'b': [1,2]}]},
                      encoder=enc) == '[[a]]\no = [ 1.2, 2.2,]\nb = [ 1, 2,]\n\n'
