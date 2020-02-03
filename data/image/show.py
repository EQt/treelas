#/usr/bin/env python
"""
Show HDF5 as image
"""
import matplotlib.pyplot as plt
import h5py
import numpy as np


def plot_figs(fnames, out, cmap, prefix=""):

    def imshow(a, cmap=cmap, vmin=0, vmax=1):
        plt.axis('off')
        plt.tight_layout()
        plt.imshow(a, vmin=vmin, vmax=vmax, cmap=cmap, interpolation='none')

    figs = dict()
    shape = None
    for fn in fnames:
        with h5py.File(fn, 'r') as io:
            for n in ['orig', 'y', 'x++', 'xgrid']:
                if n in io:
                    a = io[n][:]
                    f = plt.figure(n)
                    if n == 'orig':
                        shape = a.shape
                    if len(a.shape) < 2:
                        a = a.reshape(shape, order='C')
                    if n in figs:
                        assert np.abs(a - figs[n][0]).max() < 1e-10
                    else:
                        figs[n] = (a, f)
                    imshow(a)

    if out is None:
        plt.show()
    else:
        for n, (a, f) in figs.items():
            outn = prefix + n + "." + out
            print(outn)
            plt.imsave(outn, a, cmap=cmap, vmin=0, vmax=1)


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('fname', nargs='+')
    p.add_argument('-o', '--out', default=None, type=str)
    p.add_argument('-c', '--cmap', default=None, type=str)
    args = p.parse_args()

    plot_figs(args.fname, args.out, args.cmap)

# Local Variables:
# compile-command: "python show.py phantom_w300.img"
# End:
