#!/bin/bash
# -*- mode: julia -*-
#=
exec julia --color=yes --startup-file=no "${BASH_SOURCE[0]}" "$@"
=#
using Test


include("test_children.jl")
include("test_inorder.jl")
include("test_unionfind.jl")
include("test_root.jl")
include("test_cluster.jl")
include("test_incmat.jl")
include("test_grid.jl")
include("test_neighbors.jl")
include("test_lca.jl")
include("test_mst.jl")
include("test_hierarchy.jl")
