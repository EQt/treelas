"""
Plot the timing results.
"""
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

# https://www.pluralsight.com/guides/controlling-figure-aesthetics
sns.set_style('white', 
              {'axes.grid': True,
               'grid.color': '.5',
               'grid.linestyle': '--',
               'axes.spines.left': False,
               'axes.spines.right': False,
               'axes.spines.top': False,
              })


def plot_data(fname: str, suffix=None, figsize=(5.3, 3.7)):
    """
    Read the tab separated values (TSV) from `fname`
    and create a violin plot out of it.

    If `suffix` is a `str`, the figure is stored under the name
    of the dataset + "." + suffix.

    `figsize` specifies the size of the figure (in inches)
    """
    df = pd.read_csv(fname, sep='\t')
    for dataset, ddf in df.groupby("dataset"):
        fig = plt.figure(f"{dataset}", figsize=figsize)
        ax = sns.violinplot(
            x="lam",
            y="time_per_node",
            hue="algorithm",
            data=ddf,
            split=True,
        )

        ymin, ymax = ax.get_ylim()
        ax.set(ylabel='time per node [ns]', xlabel='$\\lambda$', ylim=(0, ymax))
        sns.despine(left=True)
        ax.legend_.remove()
        # plt.legend(loc='upper center', ncol=2, bbox_to_anchor=(0.5, 1.1))
        if suffix:
            fig.savefig(dataset + "." + suffix,
                        bbox_inches='tight', pad_inches=0.001)


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('fname', nargs='+', type=str)
    p.add_argument('-o', '--out', default=None, type=str)
    args = p.parse_args()

    for fn in args.fname:
        print(fn)
        plot_data(fn, suffix=args.out)
    if args.out is None:
        plt.show()
