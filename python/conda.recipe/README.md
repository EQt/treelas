Conda Build
==========

1. Make sure that you have the conda build utilities installed (`conda install conda-build`).
2. Run the build: `conda-build treelas --prefix-length 0`

In case of an error, look into the build directory; on Linux, it is located at
`$CONDAROOT/conda-bld/treelas_*/work`.

Finally, you can install your freshly build conda package:
```bash
conda create -n test.treelas
source activate test.treelas
conda install --use-local treelas
```


Windows
-------

needs a special compiler.
You could try a rather old `mingw-64` version packaged as `conda install m2w64-gcc`.
Or you can install Visual Studio 2017 together with the package `conda install vs2017_win-64`

Further information: [conda-doc][1]

[1]: https://github.com/conda/conda-docs/blob/master/docs/source/user-guide/tasks/build-packages/compiler-tools.rst
