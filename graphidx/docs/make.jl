#!/usr/bin/env julia

using Documenter

const JULIA_DIR = joinpath(@__DIR__, "..", "julia")

include(joinpath(JULIA_DIR, "src", "GraphIdx.jl"))


makedocs(
    modules=[GraphIdx],
    sitename="GraphIdx",
    authors="Elias Kuthe",
    build="html",
    # clean=true,
    # pages=[
    #     "Home" => "index.html",
    # ],
)

