import re
import h5py
import pandas as pd


def extract_time_n(fname):
    first_line = open(fname).readlines()[0]
    last_line = open(fname).readlines()[-1]
    time_ms = float(re.match(r' --> +(\d+\.\d+)ms', last_line).group(1))
    base, _, alg = fname.rpartition('.')
    tl_fname = first_line
    with h5py.File(tl_fname, 'r') as io:
        num_nodes = len(io["parent"])
    time_per_node_ns = time_ms * 1e-3 /  num_nodes * 1e9
    return time_per_node_ns, num_nodes


def tsv_concat(output, fnames, sep='\t'):
    pd.concat([pd.read_csv(f, sep=sep) for f in fnames])\
      .to_csv(output, sep=sep, index=None)
