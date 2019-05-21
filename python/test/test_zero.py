import numpy as np
import pytest
from treelas import TreeInstance


def test_nan():
    """Test that decreasing lambdas are handled correctly"""
    t = TreeInstance(y=np.array( [0., np.nan, np.nan]),
                     mu=np.array([1., 0., 0.]),
                     lam=np.array([1.0, 0.5, 0.3]),
                     parent=np.array([1, 2, 2], dtype=np.int32),
                     root=2)
    with pytest.raises(RuntimeError) as e:
        t.solve()
    assert 'y[1] = nan' in str(e)
    t.y = np.array([0., 0, 0])
    t.mu = np.array([1., 0., np.nan])
    with pytest.raises(RuntimeError) as e:
        t.solve()
    assert 'mu[2] = nan' in str(e)


def test_zero3():
    """Test that decreasing lambdas are handled correctly"""
    t = TreeInstance(y=np.array( [0., 0., 0.]),
                     mu=np.array([1., 0., 0.]),
                     lam=np.array([1.0, 0.5, 0.3]),
                     parent=np.array([1, 2, 2], dtype=np.int32),
                     root=2)
    t.solve()
    alpha = t.dual
    assert np.where(np.isnan(alpha))[0].tolist() == [t.root], \
        f'alpha={alpha}, root={t.root}'
    g = t.gamma
    assert (g > -1e-10).all(), f'gamma={g}'
    assert (g < +1e-10).all(), f'gamma={g}'
    v = t.dual_diff
    assert (v > -1e-10).all(), f'v={v}\nx={t.x}\nalpha={t.dual}\nlam={t.lam}'


def test_zero4():
    """Test that decreasing lambdas are handled correctly"""
    t = TreeInstance(y=np.array( [0., 0., 0., 2.]),
                     mu=np.array([1., 0., 0., 1.]),
                     lam=np.array([1.0, 0.3, np.nan, 1.0]),
                     parent=np.array([1, 2, 2, 2], dtype=np.int32),
                     root=2)
    t.solve()
    alpha = t.dual
    assert np.where(np.isnan(alpha))[0].tolist() == [t.root], \
        f'alpha={alpha}, root={t.root}'
    g = t.gamma
    assert (g > -1e-10).all(), f'gamma={g}'
    assert (g < +1e-10).all(), f'gamma={g}'
    v = t.dual_diff
    assert (v > -1e-10).all(), f'v={v}\nx={t.x}\nalpha={t.dual}\nlam={t.lam}'
