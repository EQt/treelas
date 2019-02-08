#!/usr/bin/env julia

push!(LOAD_PATH, "../src/")

using Documenter
import GraphIdx


makedocs(
    modules=[GraphIdx],
    sitename="GraphIdx",
    authors="Elias Kuthe",
    build=get(ENV, "HTML_DIR", "html"),
    clean=true,
    pages=[
        "GraphIdx" => "index.md",
        "Graph Indexes" => "idx.md",
        "Grid Graphs" => "grid.md",
        "Tree Graphs" => "tree.md",
        "Minimum Spanning Trees" => "mst.md",
        "Linear Algebra" => "lina.md",
    ],
    format = Documenter.HTML(
        prettyurls = get(ENV, "CI", nothing) == "true"
    )
)

