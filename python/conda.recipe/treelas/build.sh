#/bin/bash
set -e
set -x

build_dir=build.conda.$BUILD
config=Release

mkdir $build_dir
cd $build_dir
cmake -G "$CMAKE_GENERATOR" \
      -DBUILD_TESTS=OFF \
      -DCMAKE_BUILD_TYPE=config \
      -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
      -DPYBIND_CMAKE=~/.miniconda/share/cmake/pybind11 \
      "$SRC_DIR"

make -j $CPU_COUNT _treelas
$STRIP _treelas*
mkdir -p $SP_DIR/treelas
cp _treelas* $SP_DIR/treelas/
