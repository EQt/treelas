import matplotlib.pyplot as plt
import numpy as np


def example_plot(y=[1, 0.1], lam=1):
    @np.vectorize
    def f(x):
        return (x - y[1]) ** 2 + min(lam, (x - y[0]) ** 2)

    xs = np.linspace(-1, +3, num=200)
    plt.plot(xs, f(xs))
