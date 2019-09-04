"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
import sys
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
    "../graphidx/python/idx.cpp",
    "../graphidx/python/order.cpp",
    "../graphidx/python/spanning.cpp",
    "../cxx/tree_12x.cpp",
    "../deps/condat/condat_tv_v2.cpp",
    "../deps/glmgen/tf_dp.cpp",
    "../cxx/line/line.cpp",
    "../cxx/line/line_w.cpp",
    "../cxx/line/line_para.cpp",
    "../cxx/line/line_c.cpp",
    "../cxx/line/line_c2.cpp",
    "../cxx/line/line_c3.cpp",
    "../cxx/tree_dp.cpp",
    "../cxx/tree_dual.cpp"
]


includes = [
    path.join(path.dirname(__file__), "..", "graphidx", "cxx")
]

_treelas = Extension("treelas._treelas", sources, language='c++',
                     include_dirs=[GetPyBindInc(False), GetPyBindInc(True),
                                   *includes])


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


setup(name="treelas",
      version=describe_tag(default="0.12.9"),
      author="Elias Kuthe",
      author_email="elias.kuthe@tu-dortmund.de",
      license="MIT",
      packages=['treelas', 'treelas.graphidx'],
      install_requires=['pybind11>=2.2'],
      ext_modules=[_treelas],
      cmdclass={'build_ext': BuildExt})
