import sys
from os import path
from treelas import Tree


def print_deg_stats(fname="dimacs10/europe_0.tree", out=sys.stdout):
    """
    Parse HDF5 file containing "parent" array and
    printout a line of a TeX table containing some statistics about
    the degree distribution.
    """
    t = Tree.load(fname)
    n = len(t)
    deg = t.degree
    nleaf = len(deg[deg == 1])
    deg0 = deg[deg > 1]
    name = path.basename(fname).partition("_")[0]
    name = name.capitalize()
    print(f"\\textsc{{{name}}}",
          f"\\num{{{n}}}",
          f"\\SI{{{nleaf/n*100:.1f}}}{{\\%}}",
          f"$\\num{{{deg0.mean():.3f}}} \\pm \\num{{{deg0.std():.3f}}}$",
          sep=" & ",
          end="\\\\\n",
          file=out)
