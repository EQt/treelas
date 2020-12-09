import Libdl
import CMakeWrapper:cmake_executable

const build_dir = joinpath(dirname(@__FILE__), "build")
const root_dir = joinpath(dirname(@__FILE__), "..", "..")
const build_config = "Release"
const target = "treelas"
const cmake_opts = [
    (@static Sys.iswindows() ? ["-A", "x64"] : [])...,
    "-DCMAKE_BUILD_TYPE=$build_config",
    "-DBUILD_TESTS=OFF",
    "-DBUILD_PYEXT=OFF",
    "-DWITH_LEMON=ON",
]

# rm(build_dir, recursive=true)
if !isdir(build_dir)
    mkdir(build_dir)
end

cd(build_dir) do
    run(`$cmake_executable $root_dir $cmake_opts`)
    run(`$cmake_executable --build . --target $target --config $build_config`)
    @static if Sys.iswindows()
        @assert isfile(joinpath(build_config, "treelas.$(Libdl.dlext)"))
    else
        @assert isfile("libtreelas.$(Libdl.dlext)")
    end
end
