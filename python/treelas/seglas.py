"""
L0-Segmentation.
"""
import numpy as np


def example_plot(y=[1, 0.1], lam=1):
    import matplotlib.pyplot as plt

    @np.vectorize
    def f(x):
        return (x - y[1]) ** 2 + min(lam, (x - y[0]) ** 2)

    xs = np.linspace(-1, +3, num=10000)
    plt.plot(xs, f(xs))
    plt.plot(xs, np.gradient(f(xs) / (xs[1] - xs[0])))
