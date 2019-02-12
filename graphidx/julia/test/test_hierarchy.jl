module TestHierarchy

using Test
import GraphIdx.Tree: ChildrenIndex, hierarchy_string


@testset "Tree.hierarchy                 " begin
    cidx = ChildrenIndex([1, 1, 2, 3, 4, 1, 8, 9, 4, 9])
    expect = 
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

end
