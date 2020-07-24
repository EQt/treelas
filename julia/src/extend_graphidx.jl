import GraphIdx
import GraphIdx: EdgeGraph, Graph, Edge


function Base.collect(g::Graph)::EdgeGraph
    m = GraphIdx.num_edges(g)
    n = GraphIdx.num_nodes(g)
    edges = Vector{Edge{Int}}(undef, m)
    GraphIdx.enumerate_edges(g) do i::Int, u::Int, v::Int
        edges[i] = Edge((u, v))
    end
    EdgeGraph(n, edges)
end

Base.collect(g::EdgeGraph)::EdgeGraph = g


Base.similar(::GraphIdx.Vec{F}, n::Integer) where {F} =
    GraphIdx.Vec(Vector{F}(undef, n))

Base.similar(w::GraphIdx.Const{F}, n::Integer) where {F} =
    GraphIdx.Const(w.w)

Base.similar(::GraphIdx.Ones{F}, n::Integer) where {F} =
    GraphIdx.Ones{F}()
