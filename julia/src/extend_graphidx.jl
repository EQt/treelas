import GraphIdx
import GraphIdx: EdgeGraph, Graph, Edge, num_edges, num_nodes, enumerate_edges
import GraphIdx: Vec, Const, Ones


function Base.collect(g::Graph)::EdgeGraph
    m = num_edges(g)
    n = num_nodes(g)
    edges = Vector{Edge{Int}}(undef, m)
    enumerate_edges(g) do i::Int, u::Int, v::Int
        edges[i] = Edge((u, v))
    end
    EdgeGraph(n, edges)
end

Base.collect(g::EdgeGraph)::EdgeGraph = g


Base.similar(::Vec{F}, n::Integer) where {F} =
    Vec(Vector{F}(undef, n))

Base.similar(w::Const{F}, n::Integer) where {F} =
    Const(w.w)

Base.similar(::Ones{F}, n::Integer) where {F} =
    Ones{F}()
