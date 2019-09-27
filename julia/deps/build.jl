const build_dir = joinpath(dirname(@__FILE__), "build")
const root_dir = joinpath(dirname(@__FILE__), "..", "..")
const target = "treelas"
const cmake_opts = [
    "-DCMAKE_BUILD_TYPE=Release",
    "-DBUILD_TESTS=OFF",
    "-DBUILD_PYEXT=OFF",
]

# rm(build_dir, recursive=true)
if !isdir(build_dir)
    mkdir(build_dir)
end

cd(build_dir) do
    run(`cmake $root_dir $cmake_opts`)
    run(`cmake --build . --target $target`)
end
