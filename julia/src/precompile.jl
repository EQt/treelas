function _precompile_()
    ccall(:jl_generating_output, Cint, ()) == 1 || return nothing
    isdefined(TreeLas, Symbol("##gaplas#19")) && precompile(Tuple{getfield(TreeLas.MGT, Symbol("##gaplas#19")), Base.Iterators.Pairs{Symbol, Real, Tuple{Symbol, Symbol}, NamedTuple{(:max_iter, :learn), Tuple{Int64, Float64}}}, typeof(TreeLas.MGT.gaplas), Array{Float64, 2}, Array{Tuple{Int64, Int64}, 1}, Array{Float64, 1}})
    isdefined(TreeLas, Symbol("#gaplas##kw")) && precompile(Tuple{getfield(TreeLas.MGT, Symbol("#gaplas##kw")), NamedTuple{(:max_iter, :learn), Tuple{Int64, Float64}}, typeof(TreeLas.MGT.gaplas), Array{Float64, 2}, Array{Tuple{Int64, Int64}, 1}, Array{Float64, 1}})
    precompile(Tuple{typeof(TreeLas.Dual.dual!), Array{Float64, 1}, Array{Int64, 1}, Array{Int64, 1}, Float64})
    precompile(Tuple{typeof(TreeLas.MGT.duality_check), Array{Float64, 1}, Array{Float64, 1}})
    precompile(Tuple{typeof(TreeLas.MGT.update_tree!), Array{Float64, 1}, Array{Float64, 1}, Array{Int64, 1}, GraphIdx.EdgeGraph, Array{Int64, 1}})
    precompile(Tuple{typeof(TreeLas.Pwl.QueueUnion.merge), Array{TreeLas.Pwl.Event, 1}, Array{TreeLas.Pwl.QueueUnion.Range, 1}, Int64, Int64})
    precompile(Tuple{typeof(TreeLas.Pwl.clip), Array{TreeLas.Pwl.Event, 1}, GraphIdx.Utils.MutRef{TreeLas.Pwl.QueueUnion.Range}, Float64, Float64, Base.Val{false}, Base.Val{true}})
    precompile(Tuple{typeof(TreeLas.Pwl.clip), Array{TreeLas.Pwl.Event, 1}, GraphIdx.Utils.MutRef{TreeLas.Pwl.QueueUnion.Range}, Float64, Float64, Base.Val{true}, Base.Val{true}})
    precompile(Tuple{typeof(TreeLas.TreeDP.tree_dp!), Array{Float64, 2}, Array{Float64, 2}, GraphIdx.Tree.RootedTree, GraphIdx.Vec{Float64}, GraphIdx.Ones{Float64}, TreeLas.TreeDP.TreeDPMem{Float64, Int64}})
    precompile(Tuple{typeof(TreeLas.Utils.primal_objective), Array{Float64, 2}, Array{Float64, 2}, GraphIdx.WeightedGraph, GraphIdx.Const{Float64}})
    precompile(Tuple{typeof(TreeLas.Utils.primal_objective), Array{Float64, 2}, Array{Float64, 2}, GraphIdx.WeightedGraph})
end
