from treelas import Tree


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
