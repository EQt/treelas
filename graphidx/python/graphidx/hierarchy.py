import sys
import io
from treelas import Tree
from treelas.idx import ChildrenIndex


def _hierarchy(cidx: ChildrenIndex, v: int, indent: str, last: bool, out: io.IOBase):
    """Print a tree as a hierarchy to `out`.

    Copied and adpated from https://stackoverflow.com/a/27141402
    """
    labl = str(v)
    print(indent + ('└─' if last else '├─') + labl, file=out)

    # Space or U+2502 followed by space
    indent = indent + ('  ' if last else '│ ')
    last_i = len(cidx[v]) -1
    for i, c in enumerate(cidx[v]):
        _hierarchy(cidx, c, indent, i == last_i, out=out)


def hierarchy(cidx: ChildrenIndex, out=sys.stdout, indent=""):
    print(indent, cidx.root, file=out, sep="")
    last_i = len(cidx[cidx.root]) -1
    for i, c in enumerate(cidx[cidx.root]):
        _hierarchy(cidx, c, indent, i == last_i, out=out)
