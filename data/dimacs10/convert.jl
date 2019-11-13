import GraphIdx.Io: parse_uints
import GraphIdx: BiAdjacentIndex, num_edges, num_nodes
import HDF5: h5write


function parse_dimacs10(fname::String, f = parse_dimacs10)
    if endswith(fname, ".bz2")
        io = open(`bzip2 -d -c $fname`)
        f(io)
        close(io)
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
    head = Vector{Int}()
    tail = Vector{Int}()
    sizehint!(head, m)
    sizehint!(tail, m)
    local i::Ref{Int} = Ref{Int}(0)
    parse_uints(io) do u::UInt, last::Bool
        if i[] < u
            push!(head, i[])
            push!(tail, u)
        end
        if last
            i[] += 1
        end
    end
    return head, tail
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
    head, tail
end


fname = get(ARGS, 1, "belgium.bz2")
out = "out.h5"
head, tail = parse_dimacs10(fname, parse_dimacs10_ht)
h5write(out, "head", head)
h5write(out, "tail", tail)
open("bin", "w") do io
    write(io, head);
    write(io, tail)
end
