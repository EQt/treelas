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


def test_3():
    """
    0
    └╴1
      └╴2
    """
    cidx = ChildrenIndex([0, 0, 1])
    buf = StringIO()
    hierarchy(cidx, out=buf)
    assert buf.getvalue() == unindent(test_3.__doc__)
    
