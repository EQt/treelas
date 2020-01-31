"""
Render an SVG image.
"""
import sys
import numpy as np
import subprocess as sp
from os import path


def phantom() -> str:
    return path.join(path.dirname(__file__), "phantom.svg")


def gm_render(fname: str = phantom(), width=400, depth=8, verbose=False,
              graphicsmagick=False) -> np.ndarray:
    """
    Render an image to a grayscale matrix with values in [0, 1].

    Make sure, that the SVG file does not contain css styles as the program
    `gm` (GraphicsMagick) cannot handle it.
    """
    assert path.exists(fname)
    assert depth in [8, 16, 32]
    img_cmd = ["gm"] if graphicsmagick else [] + \
        (f"convert -colorspace gray" +
         f" -depth {depth} +antialias -resize {width}").split(' ')
    img_cmd += [fname,  "gray:-"]
    if verbose:
        print(" ".join(img_cmd))
    buf = sp.check_output(img_cmd)
    dt = np.dtype(f"uint{depth}")
    a = np.frombuffer(buf, dtype=dt).reshape(-1, width)
    a = a.astype(float)
    a /= float(np.iinfo(dt).max)
    if verbose:
        print("a.shape", a.shape)
    return a


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('svg', type=str, nargs='?', default=phantom())
    p.add_argument('-w', '--width', type=int, default=500)
    p.add_argument('-b', '--binout', action='store_true')
    p.add_argument('-p', '--ppath', action='store_true',
                   help='Printout path and exit')
    args = p.parse_args()

    if args.ppath:
        print(phantom())
        exit(0)
    elif args.binout:
        a = gm_render(args.svg, width=args.width)
        # print("a.shape", a.shape, "a.dtype", a.dtype, file=sys.stderr)
        sys.stdout.buffer.write(a)
    else:
        import matplotlib.pyplot as plt

        a = gm_render(args.svg, width=args.width, verbose=True)
        plt.imshow(a, vmin=0, vmax=1, cmap='gray', interpolation='none')
        plt.show()
