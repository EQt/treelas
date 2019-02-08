#!/usr/bin/env julia

push!(LOAD_PATH, "../src/")

using Documenter
import TreeLas


makedocs(
    modules=[TreeLas],
    sitename="TreeLas",
    authors="Elias Kuthe",
    build="html",
    clean=true,
    pages=[
        "Home" => "index.md",
    ],
    format = Documenter.HTML(
        prettyurls = get(ENV, "CI", nothing) == "true"
    )
)

