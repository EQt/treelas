module TestQueues

using Test
import TreeLas.Pwl.QueueUnion: Queues, Range, reset!
import TreeLas.TreeDP: Tree, TreeDPMem
import GraphIdx.Tree: hierarchy_string, ChildrenIndex, parenthesis_string, root_node
import GraphIdx.Tree: dfs_walk_rev


@testset "Queus: init 2 nodes            " begin
    tree = Tree([1, 1])
    n = length(tree)
    @test n == 2
    mem = TreeDPMem(n)
    cidx = ChildrenIndex(tree.parent)
    @test root_node(cidx) == 1
    @test collect(cidx[1]) == [2]
    @test collect(cidx[2]) == []
    vs = Int[]
    reset!(mem, tree)
    dfs_walk_rev(cidx) do v; push!(vs, v); end
    @test vs == [-1, -2, 2, 1]
    @test mem.queues.pq == Range[4:3, 3:2]
end


@testset "Queus: init 10 nodes           " begin
    """
    Compare to cxx/test/test_tree_dp.cpp: TEST(dptree, init_queues)
    """
    t = Tree(Int[1, 1, 2, 3, 4, 1, 8, 9, 4, 9])
    cidx = ChildrenIndex(t.parent, t.root)
    @test strip(hierarchy_string(cidx)) == strip("""
1
├─2
│ └─3
│   └─4
│     ├─5
│     └─9
│       ├─8
│       │ └─7
│       └─10
└─6""")
    @test parenthesis_string(cidx) == "1(2(3(4(5()9(8(7())10()))))6())"
    n = length(cidx)
    pq = Vector{Range}(undef, n)
    proc_order = reset!(pq, cidx, Int[])
    @test proc_order == [6, 10, 7, 8, 9, 5, 4, 3, 2, 1]
    @test n == 10
    rr = Range[20:19, 19:18, 18:17, 17:16, 16:15, 3:2, 12:11, 13:12, 14:13, 9:8]
    @test pq == rr
    """
      1
      ├─2
      │ └─3
      │   └─4
      │     ├─5
      │     └─9
      │       ├─8
      │       │ └─7
      │       └─10
      └─6
    """
end


end
