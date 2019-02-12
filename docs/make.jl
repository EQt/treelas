#!/usr/bin/env julia

push!(LOAD_PATH, "../src/")

using Documenter
import TreeLas


makedocs(
    modules=[TreeLas],
    sitename="TreeLas",
    authors="Elias Kuthe",
    build=get(ENV, "HTML_DIR", "html"),
    clean=true,
    pages=[
        "TreeLas" => "index.md",
        "Piecewise-Linear" => "pwl.md",
        "Queues" => "queues.md",
        "GraphIdx" => "graphidx.index.md",
    ],
    format = Documenter.HTML(
        prettyurls = get(ENV, "CI", nothing) == "true"
    )
)
