# import GraphIdx.Tree: ChildrenIndex, root_node

function _hierarchy(
    io::IO,
    cidx::ChildrenIndex,
    v::Int,
    indent::String,
    last::Bool
)
    println(io, indent, last ? "└╴" : "├╴", v)
    indent = indent * (last ? "  " : "│ ")
    last_i = length(cidx[v])
    for (i, c) in enumerate(cidx[v])
        _hierarchy(io, cidx, c, indent, i == last_i)
    end
end


"""
    hierarchy([io,] cidx, [indent])

Print a tree given by a `ChildrenIndex` hierarchically onto `io` (default `stdout`).

# Example
```jldoctest
julia> hierarchy(ChildrenIndex([1, 1, 1, 3]))
1
├╴2
└╴3
  └╴4

```
"""
function hierarchy(io::IO, cidx::ChildrenIndex, indent::String = "")
    local r = root_node(cidx)
    println(io, indent, r)
    last_i = length(cidx[r])
    for (i, c) in enumerate(cidx[r])
        _hierarchy(io, cidx, c, indent, i == last_i)
    end
end


hierarchy(cidx::ChildrenIndex) =
    hierarchy(stdout, cidx)


function hierarchy_string(cidx::ChildrenIndex)::String
    buf = IOBuffer()
    hierarchy(buf, cidx)
    return String(take!(buf))
end

    
