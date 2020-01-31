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
    p.add_argument('-o', '--out', default=None, type=str)
    args = p.parse_args()

    fname = args.fname
    with h5py.File(fname, 'r') as io:
        y = io['y'][:]
        orig = io.get('orig', [None])[:]
    nfigs = 1 if orig is None else 2
    fig, axis = plt.subplots(1, nfigs)
    axis[0].imshow(y, vmin=0, vmax=1, cmap='gray', interpolation='none')
    axis[0].axis('off')
    if orig is not None:
        axis[1].imshow(orig, vmin=0, vmax=1, cmap='gray', interpolation='none')
        axis[1].axis('off')

    if args.out is None:
        plt.show()
    else:
        plt.savefig(fname + "." + args.out, bbox_inches='tight', pad_inches=0.001)


# Local Variables:
# compile-command: "python show.py phantom_w300.img"
# End:
