"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
import sys
import imp
from os import path
from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext
from gitversion import describe_tag


class GetPyBindInc():
    """
    Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked.

    Copied from github.com:pybind/pybind11_example/setup.py
    """
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        pybind11_include = path.join(path.dirname(__file__), '..',
                                     'deps', 'pybind11', 'include')
        pybind11_h = path.join(pybind11_include, 'pybind11', 'pybind11.h')

        if path.exists(pybind11_h):
            return pybind11_include
        else:
            try:
                import pybind11
                return pybind11.get_include(self.user)
            except ImportError:
                raise RuntimeError(f'Please install pybind11')


sources = [
    "_treelas.cpp",
    "line.cpp",
    "tree.cpp",
    "../cxx/tree_12x.cpp",
    "../deps/condat/condat_tv_v2.cpp",
    "../deps/glmgen/tf_dp.cpp",
    "../cxx/line/line.cpp",
    "../cxx/line_dp.cpp",
    "../cxx/line/line_para.cpp",
    "../cxx/line/line_c.cpp",
    "../cxx/line/line_c2.cpp",
    "../cxx/line/line_c3.cpp",
    "../cxx/tree_dp.cpp",
    "../cxx/tree_dual.cpp"
]


includes = [
    path.join(path.dirname(__file__), "..", "deps", "graphidx", "cxx"),
    GetPyBindInc(False),
    GetPyBindInc(True),
]


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {'msvc': ['/EHsc'],
              'unix': []}

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-std=c++14')
            opts.append('-O3')
            opts.append('-Wall')
            opts.append('-fvisibility=hidden')
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)


if __name__ == '__main__':
    graphidx_setup = path.join(
        path.dirname(__file__),
        "..", "deps", "graphidx", "python", "setup.py"
    )
    gs = imp.load_source('gs', graphidx_setup)
    graphidx_sources = [path.join(path.dirname(graphidx_setup), s) for s in gs.sources]
    _graphidx = Extension(
        "treelas.graphidx._graphidx",
        language='c++',
        sources=graphidx_sources,
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
          packages=['treelas', 'treelas.graphidx'],
          install_requires=['pybind11>=2.2'],
          ext_modules=[_graphidx, _treelas],
          cmdclass={'build_ext': BuildExt}
    )
