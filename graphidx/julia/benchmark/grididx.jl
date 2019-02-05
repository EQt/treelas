import GraphIdx: neighbors_lambda, Grid.GridGraph


a = neighbors_lambda(GridGraph(2, 2, 2))
@time a = neighbors_lambda(GridGraph(1000, 1000, 2))
