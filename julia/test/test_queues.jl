using Test
import TreeLas.QueueUnion: Queues, Range, reset!
import TreeLas.TreeDP: Tree
import GraphIdx.Tree: hierarchy_string, ChildrenIndex, parenthesis_string


@testset "Queus: init 9 nodes            " begin
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
    @test pq == Range[21:20,
                      18:17,
                      17:16,
                      16:15,
                      7:6,
                      20:19,
                      11:10,
                      12:11,
                      15:14,
                      14:13]
end
