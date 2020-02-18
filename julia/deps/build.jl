import Libdl

const build_dir = joinpath(dirname(@__FILE__), "build")
const root_dir = joinpath(dirname(@__FILE__), "..", "..")
const build_config = "Release"
const target = "treelas"
const cmake_opts = [
    (Sys.iswindows() ? ["-A", "x64"] : [])...,
    "-DCMAKE_BUILD_TYPE=$build_config",
    "-DBUILD_TESTS=OFF",
    "-DBUILD_PYEXT=OFF",
]

# rm(build_dir, recursive=true)
if !isdir(build_dir)
    mkdir(build_dir)
end

cd(build_dir) do
    run(`cmake $root_dir $cmake_opts`)
    run(`cmake --build . --target $target --config $build_config`)
    @static if Sys.iswindows()
        @assert isfile(joinpath(build_config, "treelas.$(Libdl.dlext)"))
    else
        @assert isfile("libtreelas.$(Libdl.dlext)")
    end
end
