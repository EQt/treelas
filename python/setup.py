"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
import importlib
import subprocess as sp
from os import path
from setuptools import setup
from setuptools.extension import Extension
from gitversion import describe_tag

graphidx_dir = path.join(path.dirname(__file__), "..", "deps", "graphidx")

sources = [
    "_treelas.cpp",
    "line.cpp",
    "tree.cpp",
    "../cxx/tree_12x.cpp",
    "../deps/condat/condat_tv_v2.cpp",
    "../deps/glmgen/tf_dp.cpp",
    "../cxx/line_dp.cpp",
    "../cxx/line/line_para.cpp",
    "../cxx/line/line_c.cpp",
    "../cxx/line/line_c2.cpp",
    "../cxx/line/line_c3.cpp",
    "../cxx/tree_dp.cpp",
    "../cxx/tree_dual.cpp"
]

includes = [
    path.join(graphidx_dir, "cxx", "include"),
    path.join(graphidx_dir, "deps", "pybind11", "include"),
]


if __name__ == '__main__':
    graphidx_setup = path.join(graphidx_dir, "python", "setup.py")

    if not path.exists(graphidx_setup):
        cmd = f'git submodule update --init {graphidx_dir}'
        try:
            sp.check_call(cmd.split())
        except:
            print(cmd)
            raise

    spec = importlib.util.spec_from_file_location('gs', graphidx_setup)
    gs = spec.loader.load_module()

    _graphidx = Extension(
        "treelas.graphidx._graphidx",
        language='c++',
        sources=[path.join(path.dirname(graphidx_setup), s)
                 for s in gs.sources],
        include_dirs=gs.includes,
    )

    _treelas = Extension(
        "treelas._treelas",
        language='c++',
        sources=sources,
        include_dirs=includes,
    )

    setup(name="treelas",
          version=describe_tag(default="0.12.9"),
          author="Elias Kuthe",
          author_email="elias.kuthe@tu-dortmund.de",
          license="MIT",
          packages=['treelas', *('treelas.' + p for p in gs.packages)],
          install_requires=['pybind11>=2.2'],
          ext_modules=[_treelas, _graphidx],
          cmdclass={'build_ext': gs.BuildExt},
    )
