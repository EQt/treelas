"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
import re
import sys
import subprocess as sp
from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext


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
        try:
            import pybind11
            return pybind11.get_include(self.user)
        except ImportError:
            from os import path
            pybind11_include = path.join(path.dirname(__file__), '..',
                                         'extern', 'pybind11', 'include')
            pybind11_h = path.join(pybind11_include, 'pybind11', 'pybind11.h')

            if path.exists(pybind11_h):
                return pybind11_include
            else:
                raise RuntimeError(f'Please install pybind11')


sources = [
    "_treelas.cpp",
    "line.cpp",
    "order.cpp",
    "tree.cpp",
    "idx.cpp",
    "spanning.cpp",
    "../cxx/tree_12x.cpp",
    "../extern/condat/condat_tv_v2.cpp",
    "../extern/glmgen/tf_dp.cpp",
    "../cxx/line/line.cpp",
    "../cxx/line/line_w.cpp",
    "../cxx/line/line_para.cpp",
    "../cxx/line/line_c.cpp",
    "../cxx/line/line_c2.cpp",
    "../cxx/line/line_c3.cpp",
    "../cxx/tree_dp.cpp",
    "../cxx/tree_dual.cpp",
    "../cxx/postorder.cpp",
    "../cxx/cluster.cpp"
]

_treelas = Extension("treelas._treelas", sources, language='c++',
                     include_dirs=[GetPyBindInc(False), GetPyBindInc(True)])


def describe_tag(default="0.9.9"):
    try:
        ver = sp.check_output('git describe --tag'.split())
        ver = ver.decode().strip()
        m = re.match(r'v(?P<ver>\d+(\.\d+)*)((-(?P<dev>\d+))(-.*))?', ver)
        if m.group('dev') is not None:
            return f"{m.group('ver')}.dev{m.group('dev')}"
        return m.group('ver')
    except sp.CalledProcessError:
        return default


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
            # opts.append('-g')
            opts.append('-O3')
            opts.append('-Wall')
            opts.append('-fvisibility=hidden')
            if hasattr(self.compiler, "compiler"):
                if self.compiler.compiler[0] in ['gcc', 'g++', 'c++']:
                    cso = self.compiler.compiler_so
                    if '-Wstrict-prototypes' in cso:
                        cso.remove('-Wstrict-prototypes')
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)


setup(name="treelas",
      version=describe_tag(),
      author="Elias Kuthe",
      author_email="elias.kuthe@tu-dortmund.de",
      license="MIT",
      packages=['treelas'],
      install_requires=['pybind11>=2.2'],
      ext_modules=[_treelas],
      cmdclass={'build_ext': BuildExt})
