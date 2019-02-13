# import GraphIdx.Tree: ChildrenIndex, root_node

function _hierarchy(
    io::IO,
    cidx::ChildrenIndex,
    v::Int,
    indent::String,
    last::Bool,
    id::F,
) where {F}
    println(io, indent, last ? "└─" : "├─", id(v))
    indent = indent * (last ? "  " : "│ ")
    last_i = length(cidx[v])
    for (i, c) in enumerate(cidx[v])
        _hierarchy(io, cidx, c, indent, i == last_i, id)
    end
end


"""
    hierarchy([io,] cidx, [indent])

Print a tree given by a `ChildrenIndex` hierarchically onto `io` (default `stdout`).

# Example

```jldoctest
julia> hierarchy(ChildrenIndex([1, 1, 1, 3]))
1
├─2
└─3
  └─4

```
"""
function hierarchy(io::IO, cidx::ChildrenIndex, indent::String = "",
                   id::F = identity) where {F}
    local r = root_node(cidx)
    println(io, indent, id(r))
    last_i = length(cidx[r])
    for (i, c) in enumerate(cidx[r])
        _hierarchy(io, cidx, c, indent, i == last_i, id)
    end
end


hierarchy(cidx::ChildrenIndex, id::F = identity) where {F} =
    hierarchy(stdout, cidx, "", id)


"""
    hierarchy_string(cidx)

Same as `hierarchy` but output as String.
More precisely, print to a buffer and return as String.
"""
function hierarchy_string(cidx::ChildrenIndex, id::F = identity)::String where {F}
    buf = IOBuffer()
    hierarchy(buf, cidx, "", id)
    return String(take!(buf))
end


"""
    parenthesis([io,] cidx [, stack])

Print the tree in parenthesis notation: recursively print `<node id>(subtree)`.
Per default output on `stdout`.

# Example

```jldoctest
julia> parenthesis(ChildrenIndex([1, 1, 1, 3]))
1(2()3(4()))

```
"""
function parenthesis(io::IO, cidx::ChildrenIndex, stack::Vector{Int} = Int[])
    dfs_walk(cidx, stack) do v::Int
        if v < 0    # just discovered
            print(io, -v, "(")
        else
            print(io, ")")
        end
    end
end


parenthesis(cidx::ChildrenIndex, stack::Vector{Int} = Int[]) =
    parenthesis(stdout, cidx, stack)


"""
    parenthesis_string(cidx [, stack])

Like [`parenthesis`](@ref) but return output as `String`.
"""
function parenthesis_string(cidx::ChildrenIndex, stack::Vector{Int} = Int[])::String
    buf = IOBuffer()
    parenthesis(buf, cidx, stack)
    return String(take!(buf))
end
