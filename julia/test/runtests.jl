#!/usr/bin/env julia
using Test

include(joinpath("..", "..", "graphidx", "julia", "test", "runtests.jl"))
include("test_tree_dp.jl")
