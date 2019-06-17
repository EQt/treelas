#!/usr/bin/env python
"""
Load edges/{head,tail} and show the graph structure
using the `dot` (https://www.graphviz.org/) program 
"""
import sys
import subprocess as sp
from io import StringIO


def print_tree(parent, root_loop=False, indent="  ", out=sys.stdout, o=0):
    """Print a tree given by the `parent` array in dot format to `out`"""
    print("digraph tree {", file=out)
    print(indent, "rankdir = BT", file=out)
    for i, p in enumerate(parent):
        if not root_loop and i == p:
            continue
        print(indent, i + o, "->", p + o, file=out)
    print("}", file=out)


def print_dot(head, tail=None, indent="   ", out=sys.stdout,
              edge_attr=None, header=None, footer=None):
    """
    Print the graph given by the edges (`head` --> `tail`) in
    dot graph format
    """
    print("graph g {", file=out)
    edges = head if tail is None else zip(head, tail)
    if header is not None:
        print(header, file=out)
    for h, t in edges:
        if edge_attr is not None:
            print(indent, h, "--", t, '[%s];' % edge_attr, file=out)
        else:
            print(indent, h, "--", t, ';', file=out)
    if footer is not None:
        print(footer, file=out)
    print("}", file=out)


def show_dot(graph, prg="dot -Tx11", wait=True, out=sp.DEVNULL):
    """Start dot program on string `graph`"""
    dot = sp.Popen(prg.split(), stdin=sp.PIPE,
                   stdout=out, stderr=sp.DEVNULL)
    dot.stdin.write(graph.encode())
    dot.stdin.close()
    if wait:
        dot.wait()


def show_tree(parent, prg="dot -Tx11", wait=True, out=sp.DEVNULL):
    buf = StringIO()
    print_tree(parent, out=buf)
    show_dot(buf.getvalue(), prg=prg, wait=wait, out=out)


def main(args=None):
    import h5py
    from .graphio import load_edges

    buf = StringIO()
    with h5py.File(args.inh5, 'r') as io:
        if 'edges' in io:
            head, tail = load_edges(io)
            if args.index1:
                head = [h + 1 for h in head]
                tail += 1
            print_dot(head, tail, out=buf)
        elif 'parent' in io:
            print_tree(io['parent'].value, out=buf, o=int(args.index1))
        else:
            raise RuntimeError('No edges found')
    g = buf.getvalue()

    if args.verbose:
        print(g)
    else:
        show_dot(g)


def ascii_dot(args):
    import numpy as np
    import re

    parent = args.inh5
    [parent] = re.findall(r'.*{([\d,\s]+)}.*', parent)
    parent = np.fromstring(parent, sep=',', dtype=int)
    show_tree(parent, prg="graph-easy --as boxart -",
              wait=False, out=sys.stdout)


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('inh5', type=str, help='inh5 file to show')
    p.add_argument('-v', '--verbose', action='store_true',
                   help='print dot to stdout')
    p.add_argument('-1', '--index1', action='store_true',
                   help='start counting by 1')
    args = p.parse_args()
    if args.inh5.startswith("{"):
        ascii_dot(args)
    else:
        main(args)


def networkx_graphviz():
    """
    Just a small test example
    """
    import networkx as nx
    import matplotlib.pyplot as plt

    G = nx.DiGraph()

    G.add_edge('x', 'a', capacity=3.0)
    G.add_edge('x', 'b', capacity=1.0)
    G.add_edge('a', 'c', capacity=3.0)
    G.add_edge('b', 'c', capacity=5.0)
    G.add_edge('b', 'd', capacity=4.0)
    G.add_edge('d', 'e', capacity=2.0)
    G.add_edge('c', 'y', capacity=2.0)
    G.add_edge('e', 'y', capacity=3.0)

    flow_value, flow_dict = nx.maximum_flow(G, 'x', 'y')

    nx.draw(G)

    plt.show()

    def draw_graphviz(G):
        plt.clf()
        pos = nx.drawing.nx_agraph.graphviz_layout(G, prog='dot')
        nx.draw(G, pos)
