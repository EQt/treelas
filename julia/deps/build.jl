const build_dir = joinpath(dirname(@__FILE__), "build")
const root_dir = joinpath(dirname(@__FILE__), "..", "..")
const target = "treelas"
const cmake_opts = ["-DCMAKE_BUILD_TYPE=Release"]

isdir(build_dir) && rm(build_dir, recursive=true)
mkdir(build_dir)

cd(build_dir) do
    run(`cmake $root_dir $cmake_opts`)
    run(`cmake --build . --target $target`)
end
