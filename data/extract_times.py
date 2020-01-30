import re
import h5py


def extract_time_n(fname):
    last_line = open(fname).readlines()[-1]
    time_ms = float(re.match(r' --> +(\d+\.\d+)ms', last_line).group(1))
    base, _, alg = fname.rpartition('.')
    tl_fname = re.match('((.*/)*[a-z]+_\d+)*', base).group(1) + ".treelas"
    num_nodes = len(h5py.File(tl_fname)["parent"])
    time_per_node_ns = time_ms * 1e-3 /  num_nodes * 1e9
    return time_per_node_ns, num_nodes
