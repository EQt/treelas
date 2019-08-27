"""
Test latent nodes (i.e. `mu[i] = 0`);
in particular that decreasing lambdas are handled correctly.
"""
import numpy as np
import pytest
from treelas import TreeInstance


def test_nan():
    """`y` contains `NaN`"""
    t = TreeInstance(y=np.array( [0., np.nan, np.nan]),
                     mu=np.array([1., 0., 0.]),
                     lam=np.array([1.0, 0.5, 0.3]),
                     parent=np.array([1, 2, 2], dtype=np.int32),
                     root=2)
    with pytest.raises(RuntimeError) as e:
        t.solve()
        assert 'y[1] = nan' in str(e.value)
    t.y = np.array([0., 0, 0])
    t.mu = np.array([1., 0., np.nan])
    with pytest.raises(RuntimeError) as e:
        t.solve()
        assert 'mu[2] = nan' in str(e.value)


def test_zero3():
    """Tree with 3 nodes and zero input"""
    t = TreeInstance(y=np.array( [0., 0., 0.]),
                     mu=np.array([1., 0., 0.]),
                     lam=np.array([1.0, 0.5, 0.3]),
                     parent=np.array([1, 2, 2], dtype=np.int32),
                     root=2)
    t.solve()
    assert np.allclose(t.x, 0)
    alpha = t.dual
    assert np.where(np.isnan(alpha))[0].tolist() == [t.root], \
        f'alpha={alpha}, root={t.root}'
    g = t.gamma
    assert (g > -1e-10).all(), f'gamma={g}'
    assert (g < +1e-10).all(), f'gamma={g}'
    v = t.dual_diff
    assert (v > -1e-10).all(), f'v={v}\nx={t.x}\nalpha={t.dual}\nlam={t.lam}'


def test_zero3b():
    """Tree with 3 nodes and zero input"""
    t = TreeInstance(y=np.array( [0., 0., 3.]),
                     mu=np.array([1., 0., 0.]),
                     lam=np.array([1.0, 0.5, 0.3]),
                     parent=np.array([1, 2, 2], dtype=np.int32),
                     root=2)
    t.solve()
    assert np.allclose(t.x, 0)
    alpha = t.dual
    assert np.where(np.isnan(alpha))[0].tolist() == [t.root], \
        f'alpha={alpha}, root={t.root}'
    g = t.gamma
    assert (g > -1e-10).all(), f'gamma={g}'
    assert (g < +1e-10).all(), f'gamma={g}'
    v = t.dual_diff
    assert (v > -1e-10).all(), f'v={v}\nx={t.x}\nalpha={t.dual}\nlam={t.lam}'


def test_zero4():
    """
    Tree with 4 nodes, actually a line graph where just the two end
    nodes contain information
    """
    t = TreeInstance(y=np.array( [0., 0., 0., 2.]),
                     mu=np.array([1., 0., 0., 1.]),
                     lam=np.array([1.0, 0.3, np.nan, 1.0]),
                     parent=np.array([1, 2, 2, 2], dtype=np.int32),
                     root=2)
    t.solve()
    assert np.allclose(t.x, [0.3, 0.3, 1.7, 1.7])
    if False:
        t.show(wait=False)
    alpha = t.dual
    assert np.where(np.isnan(alpha))[0].tolist() == [t.root], \
        f'alpha={alpha}, root={t.root}'
    g = t.gamma
    assert (g > -1e-10).all(), f'gamma={g}'
    assert (g < +1e-10).all(), f'gamma={g}'
    v = t.dual_diff
    assert (v > -1e-10).all(), f'v={v}\nx={t.x}\nalpha={t.dual}\nlam={t.lam}'


def test_nonzero():
    """
    Similar to test_zero4 but with default mu == 1
    """
    t = TreeInstance(y=np.array( [0., 0., 0., 2.]),
                     mu=np.array([1., 1., 1., 1.]),
                     lam=np.array([1.0, 0.3, np.nan, 1.0]),
                     parent=np.array([1, 2, 2, 2], dtype=np.int32),
                     root=2)
    t.solve()
    assert np.allclose(t.x, [0.15, 0.15, 0.7, 1.0])
