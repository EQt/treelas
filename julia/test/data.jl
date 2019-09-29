import TreeLas


data_dir(paths...) =
    joinpath(dirname(pathof(TreeLas)), "..", "..", "data", paths...)
