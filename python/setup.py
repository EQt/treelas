"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
from setuptools import setup
from setuptools.extension import Extension
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
                     ],
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


setup(name="treelas",
      version=describe_tag("v0.7"),
      author="Elias Kuthe",
      author_email="elias.kuthe@tu-dortmund.de",
      license="MIT",
      install_requires=['pybind11>=2.2',
                        'h5py'],
      ext_modules=[_treelas],
      packages=['treelas']
)
