GraphIdx
=======

Most of the code builds on top of a graph library called **GraphIdx**.

```@eval
out = joinpath(pwd(), "graphidx")
@info("Generating GraphIdx documentation", out)
cd("../../graphidx/docs") do
    run(`env HTML_DIR=$out $(Base.julia_cmd()) --color=yes make.jl`)
end
@info("GraphIdx DONE")
nothing
```

The GraphIdx documentation is a separate [subpage](graphidx/index.html).
