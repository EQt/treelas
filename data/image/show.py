#/usr/bin/env python
"""
Show HDF5 as image
"""
import matplotlib.pyplot as plt
import h5py


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
                    nt = n + "2" if n in figs else n
                    a = io[n][:]
                    f = plt.figure(nt)
                    if n == 'orig':
                        shape = a.shape
                    if len(a.shape) < 2:
                        a = a.reshape(shape, order='C')
                    figs[nt] = (a, f)
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
