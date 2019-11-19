import numpy as np
import h5py
from pandas import read_csv
from numba import njit


@njit(cache=True)
def transform(vals, head, tail, m):
    e = 0
    for i in range(vals.shape[0]):
        for j in range(vals.shape[1]):
            if vals[i,j] < 0:
                break
            if vals[i, j]-1 < i:
                continue
            else:
                if e >= m:
                    return i
                head[e] = i
                tail[e] = vals[i, j] -1
                e += 1
                return -1


def convert(bzfname, h5fname):
    n, m = read_csv(bzfname, sep=' ', skiprows=14, nrows=1, header=None).values[0]
    assert n < m, n
    assert m < np.iinfo(np.int32).max, f'{m:,}'

    ncols = 15
    cols = list(range(ncols))
    df = read_csv(bzfname, sep=' ', header=None, skiprows=15, names=cols,
                  engine='c', dtype={i: float for i in range(ncols)})
    df[np.isnan(df)] = -1
    df = df.astype(np.int32)

    assert len(df) == n
    assert (df >= 0).astype(int).sum().sum() == 2*m

    for i, j in enumerate(df.max()):
        if j < 0: break

    vals = df.values
    del df

    max_degree = j
    assert max_degree < ncols

    head = -np.ones(m, dtype=np.int32)
    tail = -np.ones(m, dtype=np.int32)
    assert transform(vals, head, tail, m) == -1
    del vals
    assert head.min() >= 0
    assert tail.min() >= 0

    with h5py.File(h5fname, 'w') as io:
        g = io
        g.create_dataset("head", data=head, compression=3)
        g.create_dataset("tail", data=tail, compression=3)
