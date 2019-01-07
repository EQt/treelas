#!/usr/bin/env julia
using Test

include(joinpath("..", "..", "graphidx", "julia", "test", "runtests.jl"))
include("test_dp_tree.jl")
