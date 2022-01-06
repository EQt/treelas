precompile(Dual.dual!, (Vector{Float64}, Vector{Int64}, Vector{Int64}, Float64))
precompile(MGT.duality_check, (Vector{Float64}, Vector{Float64}))
precompile(
    MGT.update_tree!,
    (
        Vector{Float64},
        Vector{Float64},
        Vector{Int64},
        GraphIdx.EdgeGraph,
        Vector{Int64},
    ),
)
precompile(
    Pwl.QueueUnion.merge,
    (Vector{Pwl.Event}, Vector{Pwl.QueueUnion.Range}, Int64, Int64),
)
precompile(
    Pwl.clip,
    (
        Vector{Pwl.Event},
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
        Vector{Pwl.Event},
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
        Matrix{Float64},
        Matrix{Float64},
        GraphIdx.Tree.RootedTree,
        GraphIdx.Vec{Float64},
        GraphIdx.Ones{Float64},
        TreeDP.TreeDPMem{Float64,Int64},
    ),
)
precompile(
    Utils.primal_objective,
    (Matrix{Float64}, Matrix{Float64}, GraphIdx.WeightedGraph, GraphIdx.Const{Float64}),
)
precompile(
    Utils.primal_objective,
    (Matrix{Float64}, Matrix{Float64}, GraphIdx.WeightedGraph),
)
