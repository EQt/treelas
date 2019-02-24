function square_edges()
    head = [1, 1, 2, 3]
    tail = [2, 3, 4, 4]
    n = max(maximum(tail), maximum(head))
    edges = collect(zip(head, tail))
    return edges, n
end


mean(x) = sum(x)/length(x)
