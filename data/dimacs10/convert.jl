#!/usr/bin/env julia
using GraphIdx.Io.Dimacs10: open_dimacs10, parse_dimacs10_edges
using HDF5: h5open, attrs

fname = get(ARGS, 1, "belgium.bz2")
out = get(ARGS, 2, "out.h5")
output_bin = false

@time n, m, head, tail = open_dimacs10(parse_dimacs10_edges, fname)

isfile(out) && rm(out)
h5open(out, "w") do io
    attrs(io)["n"] = n
    attrs(io)["m"] = m
    write(io, "head", head .-1)
    write(io, "tail", tail .-1)
end

output_bin && open("bin", "w") do io
    write(io, head .-1);
    write(io, tail .-1)
end
