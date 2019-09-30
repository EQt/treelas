module TestQueues

using Test
import TreeLas.Pwl.QueueUnion: Queues, Range, reset!
import TreeLas.TreeDP: Tree
import GraphIdx.Tree: hierarchy_string, ChildrenIndex, parenthesis_string


@testset "Queus: init 9 nodes            " begin
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
    rr = Range[21:20, 20:19, 19:18, 18:17, 17:16, 4:3, 13:12, 14:13, 15:14, 10:9]
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
