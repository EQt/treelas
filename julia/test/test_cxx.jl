module TestTreeLasCxx

using Test
import Libdl
import TreeLas

const cxx_jl = joinpath(dirname(pathof(TreeLas)), "cxx.jl")


@testset "TreeLas.Cxx                    " begin
    @test isfile(cxx_jl)
    include(cxx_jl)
    @test isfile(Cxx.lib)

    cxx_txt = read(open(cxx_jl), String)
    syms = Set((String(m.captures[1]) for m in eachmatch(r"(_Z\w+)", cxx_txt)))

    tl = Libdl.dlopen(Cxx.lib)
    @testset "Check DLL symbols" begin
        for s in syms
            @test Libdl.dlsym_e(tl, s) != Ptr{Nothing}()
        end
    end
end


end
