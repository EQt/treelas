#!/usr/bin/env julia
using Test

include(joinpath("..", "..", "graphidx", "julia", "test", "runtests.jl"))
include("test_queues.jl")
include("test_tree_dp.jl")
include("test_tree_dp2.jl")
include("test_mgt.jl")
include("test_gaplas.jl")
