set CONFIGURATION=Release
set BUILDDIR=build.conda.%BUILD%
set CMAKEGENERATOR=Visual Studio 14 2015 Win64

mkdir %BUILDDIR%
cd %BUILDDIR%


cmake -E copy_directory "%SRC_DIR%\python\treelas" "%SP_DIR%\treelas"
cmake -E remove_directory "%SP_DIR%\treelas\__pycache__"

cmake -G "%CMAKEGENERATOR%" ^
      -DBUILD_TESTS=OFF ^
      -DPYTHON_PREFIX="%SP_DIR%" ^
      "%SRC_DIR%"
cmake --build . --target _treelas --config %CONFIGURATION%
cmake -E copy Release\_treelas "%SP_DIR%\treelas\_treelas.pyd"
