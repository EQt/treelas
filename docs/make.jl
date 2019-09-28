#!/usr/bin/env julia

push!(LOAD_PATH, "../src/")

using Documenter
import TreeLas

DocMeta.setdocmeta!(TreeLas, :DocTestSetup, :(import TreeLas);
                    recursive=true, warn=true)

makedocs(
    modules=[TreeLas],
    sitename="TreeLas",
    authors="Elias Kuthe",
    build=get(ENV, "HTML_DIR", "html"),
    clean=true,
    pages=[
        "TreeLas" => "index.md",
        "Tree Solver" => "tree_dp.md",
        "Piecewise-Linear" => "pwl.md",
        "GraphIdx" => "graphidx.index.md",
    ],
    format = Documenter.HTML(
        prettyurls = get(ENV, "CI", nothing) == "true"
    )
)
