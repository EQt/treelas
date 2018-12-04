
build_dir = joinpath(dirname(@__FILE__), "build")
root_dir = joinpath(dirname(@__FILE__), "..", "..")

isdir(build_dir) && rm(build_dir, recursive=true)
mkdir(build_dir)

@show root_dir

cd(build_dir) do
    @show pwd()
    run(`cmake $root_dir`)
end

