#/usr/bin/env python
"""
Show HDF5 as image
"""
import matplotlib.pyplot as plt
import h5py


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('fname', nargs='+')
    p.add_argument('-o', '--out', default=None, type=str)
    p.add_argument('-c', '--cmap', default='gray', type=str)
    args = p.parse_args()

    cmap = args.cmap
    def imshow(a, cmap=cmap, vmin=0, vmax=1):
        plt.axis('off')
        plt.tight_layout()
        plt.imshow(a, vmin=vmin, vmax=vmax, cmap=cmap, interpolation='none')

    figs = dict()
    shape = None
    for fn in args.fname:
        with h5py.File(fn, 'r') as io:
            for n in ['orig', 'y', 'x++']:
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

    if args.out is None:
        plt.show()
    else:
        for n, (a, f) in figs.items():
            outn = n + "." + args.out
            print(outn)
            plt.imsave(outn, a, cmap=cmap)


# Local Variables:
# compile-command: "python show.py phantom_w300.img"
# End:
