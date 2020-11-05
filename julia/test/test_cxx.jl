module TestTreeLasCxx

using Test
import Libdl
import TreeLas

const cxx_jl = joinpath(dirname(pathof(TreeLas)), "cxx.jl")

@static if !Sys.iswindows()

@testset "TreeLas.Cxx                    " begin
    @assert isfile(cxx_jl)
    include(cxx_jl)
    @assert isfile(Cxx.lib) "Call `Pkg.build(\"TreeLas\")`"

    syms = Set((String(m.captures[1])
                for m in eachmatch(r"(_Z\w+)", read(open(cxx_jl), String))))
    @test syms == Set()
    syms = Set([
        "tree_dp_f64_i32",
        "tree_dual_f64_i32",
    ])
    Libdl.dlopen(Cxx.lib) do tl
        @testset "Check DLL symbols" begin
            for s in syms
                @testset "Check symbol $s" begin
                    @test Libdl.dlsym_e(tl, s) != Ptr{Nothing}()
                end
            end
        end
    end
end
end

end
