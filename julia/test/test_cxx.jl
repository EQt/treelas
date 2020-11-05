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
