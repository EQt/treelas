import .Utils: UnionFind, unite!, init, find


"""
    kruskal_mst(n, edges, weight)

Kruskal's minimum spanning tree algorithm.
Return a vector indicating for each edge whether it is part of the
spanning tree.
"""
function kruskal_mst(n::Int, edges, weight::Vector{Float64})::Vector{Bool}
    @assert length(weight) == length(edges)
    m = length(edges)
    selected, order, uf = Vector{Bool}(undef, m), Vector{Int}(undef, m), UnionFind(n)
    return _kruskal_mst(weight, edges, selected, order, uf)
end


function _kruskal_mst(weight, edges, selected, order, unfi)
    selected .= false
    sortperm!(order, weight, rev=false)  # , alg=Base.Sort.DEFAULT_STABLE)
    init(unfi)
    for ei in order
        (u, v) = edges[ei]
        fu, fv = find(unfi, u), find(unfi, v)
        if fu != fv
            unite!(unfi, fu, fv)
            selected[ei] = true
        end
    end
    return selected
end

