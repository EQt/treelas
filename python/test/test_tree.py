import numpy as np
import h5py
import pytest
import os
from treelas import Tree, TreeInstance


def test_children3():
    t = Tree.from_prufer([1, 2])
    assert t.root == 3
    assert (t.parent == [1, 2, 3, 3]).all()
    (t.children[0] == []).all()
    for i in range(1, 4):
        assert (t.children[i] == [i-1]).all()


def test_degree3():
    t = Tree.from_prufer([1, 2])
    assert (t.degree == [1, 2, 2, 1]).all()


@pytest.fixture
def tree5(request):
    #         0  1  2  3  4  5  6  7  8  9
    parent = [0, 0, 1, 2, 3, 0, 7, 8, 3, 8]
    y = [8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0
    t = Tree(parent)
    assert t.root == 0

    def cleaner():
        if os.path.exists('tree5.h5'):
            os.remove('tree5.h5')

    request.addfinalizer(cleaner)
    return TreeInstance(y, t.parent, lam=lam)


def test_tree5_solve(tree5):
    ti = tree5
    ti.solve()
    diff = np.abs(ti.x*3 -
                  [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2])
    assert diff.max() < 1e-14, f'{diff}, x={ti.x}'
    assert ti.x.mean() == ti.y.mean()

    alpha = ti.dual
    assert (np.abs(alpha[1:]) <= ti.lam).all()
    assert np.isnan(alpha[0])
    diff = alpha[1:] * 3 - [ -1.1,  -2.8, +3. , +1.5, +3. , -3. , -1.4, +3. ,  -0.2]
    assert np.abs(diff).max() < 1e-14, f'{diff.max()}:\n{alpha*3}\n{diff.round(1)}'

    gap = ti.gamma
    assert (gap >= 0).all()
    assert gap.max() <= 1e-13, f'{gap}'


def test_tree5_write_simple(tree5):
    ti = tree5
    ti.save('tree5.h5')
    with h5py.File('tree5.h5') as io:
        assert 'lam' in io
        assert 'y' in io
        assert 'parent' in io


def test_tree5_write_mu(tree5):
    ti = tree5
    n = 10
    assert len(ti.parent) == n
    ti.mu = np.linspace(0, 1, num=n)
    ti.save('tree5.h5')
    with h5py.File('tree5.h5') as io:
        assert 'lam' in io
        assert 'y' in io
        assert 'parent' in io
        assert 'mu' in io


def test_tree5_write_read(tree5):
    ti = tree5
    ti.save('tree5.h5')
    t2 = TreeInstance.load('tree5.h5')
    assert repr(ti) == repr(t2), f"\n\n{ti}\n\n{t2}\n"


def test_rtree(n=5, seed=2018, eps=1e-14):
    t = Tree.random(n, seed=seed)
    assert t.n == n
    y = np.array([ 0.1,  1.7, -0.1,  1. ,  1.1])
    if n != 5 or seed != 2015:
        np.random.seed(seed)
        y = np.random.normal(size=n).round(1)
    lam = 0.2
    ti = TreeInstance(y, t.parent, lam=lam)
    ti.solve()

    assert abs(ti.x.mean() - ti.y.mean()) < eps
    if n == 5 and seed == 2015:
        assert (ti.x == [0.1 , 1.5 , 0.1 , 1.05, 1.05]).all()
    assert ti.gamma.min() >= -eps, ti.gamma.min()
    assert ti.gamma.max() <= +eps, f'seed = {seed}: gamma = {ti.gamma.max()}\n{ti}'
