"""
Need Boost::Python, Boost::Numpy.
Please install via `conda install boost`
"""
import sys
import os
from glob import glob
from setuptools import setup
from setuptools.extension import Extension
from setuptools.command.build_ext import build_ext


class BuildExt(build_ext):
    """Avoid GCC warnings"""
    def build_extensions(self):
        if hasattr(self.compiler, "compiler"):
            if self.compiler.compiler[0] == 'gcc':
                self.compiler.compiler_so.remove('-Wstrict-prototypes')
                self.compiler.compiler_so.append('-Wno-deprecated-declarations')
        super(BuildExt, self).build_extensions()


def find_boost():
    include_dir = sys.base_prefix
    if sys.platform == 'win32':
        include_dir = os.path.join(include_dir, 'Library')
    include_dir = os.path.join(include_dir, 'include')
    if not os.path.exists(os.path.join(include_dir, 'boost', 'python')):
        raise RuntimeError(f"Need boost python in '{include_dir}'" + \
                           "\n\n  conda install boost")
    if not os.path.exists(os.path.join(include_dir, 'boost', 'python', 'numpy')):
        raise RuntimeError(f"Need boost::numpy in '{include_dir}'" + \
                           "\n\n  conda install boost")
    boost_libs = ["boost_python", "boost_numpy"]
    extra = dict()
    if sys.platform == 'win32':
        libs = os.path.join(sys.base_prefix, 'Library', 'lib')
        abs_boost = [glob(os.path.join(libs,  'lib' + b) + '*')[0]
                     for b in boost_libs]
        # boost_libs = [os.path.basename(b).split('.lib')[0] for b in abs_boost]
        boost_libs = [os.path.basename(b).replace('.lib', '') for b in abs_boost]
        extra['library_dirs'] = [libs]
    args = dict(**extra,
                include_dirs=[include_dir],
                libraries=boost_libs)
    # raise RuntimeError(args)
    return args


treelas_ext = Extension("treelas._treelas",
                        ["py_ext.cpp",
                         "../cxx/dualtree.cpp",
                         "../cxx/gamma.cpp",
                         "../cxx/dp_tree.cpp",
                         "../cxx/postorder.cpp",
                         "../cxx/dp_line.cpp",
                         "../extern/glmgen/tf_dp.cpp"],
                        language='c++', **find_boost())

setup(name="treelas",
      version="0.5",
      author="Elias Kuthe",
      author_email="elias.kuthe@tu-dortmund.de",
      license="MIT",
      cmdclass={'build_ext': BuildExt},
      ext_modules=[treelas_ext],
      packages=['treelas']
)
