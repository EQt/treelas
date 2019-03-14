import .Grid


function neighbors_lambda(g::Grid.GridGraph)
    idx, pi, lam = Grid.neighboridx_lambda(g)
    return IncidenceIndex(idx, pi), lam
end
