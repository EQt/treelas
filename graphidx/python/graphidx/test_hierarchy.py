from hierarchy import hierarchy
from treelas.idx import ChildrenIndex
from io import StringIO


def unindent(s):
    indent = s[s.rfind("\n"):]
    return s.replace(indent, "\n").lstrip()


def test_unindent():
    """
    a
      b
    c
    """
    assert unindent(test_unindent.__doc__) == "a\n  b\nc\n"


def hierarchy_str(parent):
    cidx = ChildrenIndex(parent)
    buf = StringIO()
    hierarchy(cidx, out=buf)
    return buf.getvalue()


def test_3():
    """
    0
    └─1
      └─2
    """
    assert hierarchy_str([0, 0, 1]) == unindent(test_3.__doc__)


def test_10():
    """
    0
    ├─1
    │ └─2
    │   └─3
    │     ├─4
    │     └─8
    │       ├─7
    │       │ └─6
    │       └─9
    └─5
    """
    pi = [0, 0, 1, 2, 3, 0, 7, 8, 3, 8]
    h = hierarchy_str(pi)
    assert h == unindent(test_10.__doc__), "\n" + str(h)
