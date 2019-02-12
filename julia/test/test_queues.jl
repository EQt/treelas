using Test
import TreeLas.QueueUnion: Queues, Range, reset!
import TreeLas.TreeDP: Tree
import GraphIdx.Tree: hierarchy_string, ChildrenIndex


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
end

