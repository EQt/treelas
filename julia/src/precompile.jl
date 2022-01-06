precompile(Dual.dual!, (Array{Float64,1}, Array{Int64,1}, Array{Int64,1}, Float64))
precompile(MGT.duality_check, (Array{Float64,1}, Array{Float64,1}))
precompile(
    MGT.update_tree!,
    (
        Array{Float64,1},
        Array{Float64,1},
        Array{Int64,1},
        GraphIdx.EdgeGraph,
        Array{Int64,1},
    ),
)
precompile(
    Pwl.QueueUnion.merge,
    (Array{Pwl.Event,1}, Array{Pwl.QueueUnion.Range,1}, Int64, Int64),
)
precompile(
    Pwl.clip,
    (
        Array{Pwl.Event,1},
        GraphIdx.Utils.MutRef{Pwl.QueueUnion.Range},
        Float64,
        Float64,
        Base.Val{false},
        Base.Val{true},
    ),
)
precompile(
    Pwl.clip,
    (
        Array{Pwl.Event,1},
        GraphIdx.Utils.MutRef{Pwl.QueueUnion.Range},
        Float64,
        Float64,
        Base.Val{true},
        Base.Val{true},
    ),
)
precompile(
    TreeDP.tree_dp!,
    (
        Array{Float64,2},
        Array{Float64,2},
        GraphIdx.Tree.RootedTree,
        GraphIdx.Vec{Float64},
        GraphIdx.Ones{Float64},
        TreeDP.TreeDPMem{Float64,Int64},
    ),
)
precompile(
    Utils.primal_objective,
    (Array{Float64,2}, Array{Float64,2}, GraphIdx.WeightedGraph, GraphIdx.Const{Float64}),
)
precompile(
    Utils.primal_objective,
    (Array{Float64,2}, Array{Float64,2}, GraphIdx.WeightedGraph),
)
