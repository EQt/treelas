"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
import sys
from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext
from subprocess import check_output


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
        import pybind11
        return pybind11.get_include(self.user)


_treelas = Extension("treelas._treelas",
                     ["_treelas.cpp",
                      "../extern/condat/condat_tv_v2.cpp",
                      "../extern/glmgen/tf_dp.cpp",
                      "../cxx/line.cpp"],
                     language='c++',
                     include_dirs=[GetPyBindInc(False),
                                   GetPyBindInc(True)]
)


def describe_tag(default):
    try:
        ver = check_output('git describe --tag'.split())
        return ver.decode().strip()
    except:
        return default


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {'msvc': ['/EHsc'],'unix': []}

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-std=c++11')
            opts.append('-g')
            opts.append('-O3')
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)


setup(name="treelas",
      version=describe_tag("v0.7"),
      author="Elias Kuthe",
      author_email="elias.kuthe@tu-dortmund.de",
      license="MIT",
      packages=['treelas'],
      install_requires=['pybind11>=2.2'],
      ext_modules=[_treelas],
      cmdclass={'build_ext': BuildExt}
)
