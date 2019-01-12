"""
    node_degrees(parent, [root = 1])

Return a vector telling the degree of each node.
"""
function node_degrees(parent::Vector{Int}, root = 1)
    @assert parent[root] == root
    idx = similar(parent)
    idx .= 0
    for p in parent
        idx[p] += 1
    end
    idx[root] -= 1      # root does not have itself as children
    return idx
end
