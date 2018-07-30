import numpy as np
import treelas as tl


def test_postorder3():
    parent      = np.array([0, 0, 0], dtype=np.int32)
    post_expect = np.array([2, 1], dtype=np.int32)
    postord = tl.post_order(parent)
    assert postord.shape == post_expect.shape
    assert (postord == post_expect).all(), \
        f'postorder = {postord}, expected = {post_expect}'


def test_postorder():
    """Graph from ../test/test_children.cpp, i.e. generate5"""
    parent      = np.array([0, 0, 1, 2, 3, 0, 7, 8, 3, 8], dtype=np.int32)
    post_expect = np.array([5, 9, 6, 7, 8, 4, 3, 2, 1], dtype=np.int32)
    postord = tl.post_order(parent)
    assert postord.shape == post_expect.shape
    assert (postord == post_expect).all(), \
        f'postorder = {postord}, expected = {post_expect}'
