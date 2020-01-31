import re
import pandas as pd
from os import path


def extract_time_n(fname):
    last_line = open(fname).readlines()[-1]
    time_ms = float(re.match(r' --> +(\d+\.\d+)ms', last_line).group(1))
    base, _, alg = fname.rpartition('.')
    num_nodes = int(re.search(
        " *n = ([0-9_]+)", open(fname).read()).group(1))
    time_per_node_ns = time_ms * 1e-3 /  num_nodes * 1e9
    return time_per_node_ns, num_nodes


def tsv_concat(output, fnames, sep='\t'):
    pd.concat([pd.read_csv(f, sep=sep) for f in fnames])\
      .to_csv(output, sep=sep, index=None)
