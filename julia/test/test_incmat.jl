module TestIncMat
using Test

include("../src/incmat.jl")

@testset "Create D: Square               " begin
    head = [1, 1, 2, 3]
    tail = [2, 3, 4, 4]
    n = length(head)
    edges = collect(zip(head, tail))::Edges
    lambda = fill(0.5, length(edges))
    D = create_D(edges, lambda, n)
end

end
