#/usr/bin/env python
"""
Show HDF5 as image
"""
import matplotlib.pyplot as plt
import h5py


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('fname', type=str)
    args = p.parse_args()

    with h5py.File(args.fname, 'r') as io:
        a = io['y'][:]
    plt.imshow(a, vmin=0, vmax=1, cmap='gray', interpolation='none')
    plt.show()
