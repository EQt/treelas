import GraphIdx.Io: parse_uints
import GraphIdx: BiAdjacentIndex, num_edges, num_nodes
import HDF5: h5open, attrs
import CodecBzip2


function parse_dimacs10(fname::String, f = parse_dimacs10; as_process=false)
    if endswith(fname, ".bz2")
        if as_process
            open(`bzip2 -d -c $fname`) do io
                return f(io)
            end
        else
            open(fname) do bz2
                io = CodecBzip2.Bzip2DecompressorStream(bz2)
                return f(io)
            end
        end
    else
        if fname == "-"
            f(stdin)
        else
            @assert ispath(fname)
            open(f, fname, "r")
        end
    end
end


function parse_dimacs10_header(io::IO)
    line = ""
    while startswith(line, "%") || line == ""
        line = readline(io)
    end
    n, m = map(x-> parse(Int, x), split(line))
end


function parse_dimacs10(io::IO)::BiAdjacentIndex
    n, m = parse_dimacs10_header(io)
    pi = Vector{Int}()
    idx = Vector{Int}()
    sizehint!(pi, n+1)
    sizehint!(idx, 2m)
    parse_uints(io) do u::UInt, last::Bool
        push!(idx, u)
        if last
            push!(pi, length(idx) + 1)
        end
    end
    return BiAdjacentIndex(pi, idx)
end


function parse_dimacs10_ht(io::IO)
    n, m = parse_dimacs10_header(io)
    head = Vector{Int32}()
    tail = Vector{Int32}()
    sizehint!(head, m)
    sizehint!(tail, m)
    local i::Ref{Int32} = Ref{Int32}(0)
    parse_uints(io) do u::UInt, last::Bool
        if i[] < u
            push!(head, Int32(i[]))
            push!(tail, Int32(u) - 1)
        end
        if last
            i[] += 1
        end
    end
    return n, m, head, tail
end


function transform(fname::String)
    out = "out.h5"
    idx = parse_dimacs10(fname)
    m = num_edges(idx)
    n = num_nodes(idx)
    @show (n, m)
    head, tail = Vector{Int}(), Vector{Int}()
    sizehint!(head, m)
    sizehint!(tail, m)
    for i in 1:n
        for j in idx[i]
            if i < j
                push!(head, i)
                push!(tail, j)
            end
        end
    end
    n, m, head, tail
end


fname = get(ARGS, 1, "belgium.bz2")
out = get(ARGS, 2, "out.h5")
output_bin = false

@time n, m, head, tail = parse_dimacs10(fname, parse_dimacs10_ht)
isfile(out) && rm(out)

h5open(out, "w") do io
    attrs(io)["n"] = n
    attrs(io)["m"] = m
    write(io, "head", head)
    write(io, "tail", tail)
end

output_bin && open("bin", "w") do io
    write(io, head);
    write(io, tail)
end
