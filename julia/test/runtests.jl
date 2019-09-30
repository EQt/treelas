#!/usr/bin/env julia
using Test

include("test_lines.jl")
include("test_cxx.jl")
include("test_queues.jl")
include("test_tree_dp.jl")
include("test_tree0.jl")
# include("test_tree_dp.old.jl")
include("test_mgt.jl")
include("test_dual.jl")
include("test_gaplax.jl")
