"""
http://www.benjack.io/2017/06/12/python-cpp-tests.html
"""
import sys
import os
from glob import glob
from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext


_treelas = Extension("treelas._treelas",
                     ["_treelas.cpp",
                     ],
                     language='c++',
                     include_dirs=[])

setup(name="treelas",
      version="0.5",
      author="Elias Kuthe",
      author_email="elias.kuthe@tu-dortmund.de",
      license="MIT",
      ext_modules=[_treelas],
      packages=['treelas']
)
