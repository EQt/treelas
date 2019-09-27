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
    path.join(path.dirname(__file__), "..", "deps", "graphidx", "deps", "pybind11", "include"),
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
    graphidx_dir = path.join(path.dirname(__file__), "..", "deps", "graphidx")
    graphidx_setup = path.join(graphidx_dir, "python", "setup.py")

    if not path.exists(graphidx_setup):
        import subprocess as sp

        cmd = f'git submodule update --init {graphidx_dir}'
        try:
            sp.check_call(cmd.split(), cwd=deps)
        except:
            print(cmd)
            raise
    
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
          ext_modules=[_treelas, _graphidx],
          cmdclass={'build_ext': BuildExt}
    )
