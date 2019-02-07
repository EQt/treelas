#!/usr/bin/env julia

push!(LOAD_PATH, "../src/")

using Documenter
import GraphIdx


makedocs(
    modules=[GraphIdx],
    sitename="GraphIdx",
    authors="Elias Kuthe",
    build="html",
    clean=true,
    pages=[
        "Home" => "index.md",
        "Tree Graphs" => "tree.md",
        "Graph Indexes" => "idx.md",
        "Minimum Spanning Trees" => "mst.md",
        "Grid Graphs" => "grid.md",
        "Linear Algebra" => "lina.md",
    ],
    format = Documenter.HTML(
        prettyurls = get(ENV, "CI", nothing) == "true"
    )
)

