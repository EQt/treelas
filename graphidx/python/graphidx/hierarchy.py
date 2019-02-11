import sys
import io
from treelas import Tree
from treelas.idx import ChildrenIndex


def _hierarchy(cidx: ChildrenIndex, v: int, indent: str, last: bool, out: io.IOBase):
    """Print a tree as a hierarchy to `out`.

    Copied and adpated from https://stackoverflow.com/a/27141402
    """
    labl = str(v)
    print(indent + ('└╴' if last else '├╴') + labl, file=out)

    # Space or U+2502 followed by space
    indent = indent + ('  ' if last else '│ ')
    kidz = cidx[v]
    for (i, c) in enumerate(kidz):
        _hierarchy(cidx, c, indent, i+1 == len(kidz), out=out)


def hierarchy(cidx: ChildrenIndex, out=sys.stdout, indent=""):
    print(indent, cidx.root, file=out, sep="")
    kidz = cidx[cidx.root]

    for (i, c) in enumerate(kidz):
        _hierarchy(cidx, c, indent, i+1 == len(kidz), out=out)
    
    
