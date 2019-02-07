#!/usr/bin/env julia

using Documenter

# const JULIA_DIR = joinpath(@__DIR__, "..", "julia")
# include(joinpath(JULIA_DIR, "src", "GraphIdx.jl"))

import GraphIdx


makedocs(
    modules=[GraphIdx],
    sitename="GraphIdx",
    authors="Elias Kuthe",
    build="html",
    # clean=true,
    pages=[
        "Home" => "index.md",
        "Graph Indexes" => "idx.md",
        "Minimum Spanning Trees" => "mst.md",
        "Grid Graphs" => "grid.md",
        "Linear Algebra" => "lina.md",
    ],
    format = Documenter.HTML(
        prettyurls = get(ENV, "CI", nothing) == "true"
    )
)

