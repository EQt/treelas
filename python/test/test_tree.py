import numpy as np
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


def test_tree5():
    parent = [0, 0, 1, 2, 3, 0, 7, 8, 3, 8]
    y = [8.2, 7.0, 9.5, 6.8, 5.8, 6.3, 4.3, 2.2, 1.2, 2.8]
    lam = 1.0
    t = Tree(parent)
    assert t.root == 0
    ti = TreeInstance(y, t.parent, lam=lam)
    ti.solve()
    diff = np.abs(ti.x*3 - [22.7, 22.7, 22.7, 18.9, 18.9, 21.9,  9.9,  8.2,  8.2,  8.2])
    assert diff.max() < 1e-14, f'{diff}, x={ti.x}'

    alpha = ti.dual
    assert (np.abs(alpha[1:]) <= lam).all()
    assert np.isnan(alpha[0])
    diff = alpha[1:] * 3 - [ 1.1,  2.8, -3. , -1.5, -3. , -3. , -1.4, -3. ,  0.2]
    assert np.abs(diff).max() < 1e-14

    gap = ti.gamma
