#/bin/bash

build_dir=build.conda.$BUILD

mkdir $build_dir
cd $build_dir
cmake -G "$CMAKE_GENERATOR" \
      -DBUILD_TESTS=OFF \
      -DPYTHON_PREFIX="$SP_DIR" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
      "$SRC_DIR"
#      -DPYTHON_EXECUTABLE=$PYTHON \
# cmake --build . --target pyinstall
make -j $CPU_COUNT pyinstall
$STRIP $SP_DIR/treelas/_treelas*
