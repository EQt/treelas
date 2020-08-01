module MinCut

import TreeLas.Cxx: lib

struct WeightedArc{I, F}
    head::I
    tail::I
    weight::F
end

WeightedArc{I, F}(t::Tuple{I2,I2,F2}) where {I, F, I2, F2} =
    WeightedArc{I,F}(I(t[1]), I(t[2]), F(t[3]))


function cxx_min_cut!(
    cut::Vector{Bool},
    n::Integer,
    warcs::Vector{WeightedArc{Cint, Cdouble}},
    source::Integer,
    target::Integer,
)
    resize!(cut, n)
    m = length(warcs)
    return ccall(
        (:min_cut_f64_i32, lib),
        Cdouble,
        (
            Ref{Bool},
            Csize_t,
            Csize_t,
            Ref{WeightedArc{Cint, Cdouble}},
            Cint,
            Cint,
        ),
        cut,
        n,
        m,
        warcs,
        source,
        target,
    )
end


end


module TestMinCut

import ..MinCut
using Test

@testset "MinCut" begin
    warcs = MinCut.WeightedArc{Cint, Cdouble}.([
        (0, 1, 5.0),
        (0, 3, 10),
        (1, 2, 10),
        (3, 1, 15),
        (3, 2, 5),
    ])
    source = 0
    target = 2
    n = 4
    cut = Vector{Bool}()
    MinCut.cxx_min_cut!(cut, n, warcs, source, target)
    @test Set(i for i=1:n if cut[i]) == Set(1)
end


end
