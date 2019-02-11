var documenterSearchIndex = {"docs": [

{
    "location": "index.html#",
    "page": "GraphIdx",
    "title": "GraphIdx",
    "category": "page",
    "text": ""
},

{
    "location": "index.html#GraphIdx",
    "page": "GraphIdx",
    "title": "GraphIdx",
    "category": "module",
    "text": "GraphIdx\n\nMost important submodules are\n\nLinA: Linear algebra (e.g. incidence matrix)\nGrid: Specialized code for grid graphs.\nUtils.Heap: Priority queues (e.g. binary heap)\n\n\n\n\n\n"
},

{
    "location": "index.html#GraphIdx.jl-1",
    "page": "GraphIdx",
    "title": "GraphIdx.jl",
    "category": "section",
    "text": "This is the Julia part of GraphIdx, a framework for index-based graphs. Some parts are also implemented inC++ headers, or\na Rust Crate.The main module isGraphIdx"
},

{
    "location": "index.html#Example-1",
    "page": "GraphIdx",
    "title": "Example",
    "category": "section",
    "text": "Generate a random spanning tree on a 20×32 grid graph:import GraphIdx.Grid: GridGraph\nimport GraphIdx.NeighborIndex\n\nTODO: Extend"
},

{
    "location": "index.html#API-1",
    "page": "GraphIdx",
    "title": "API",
    "category": "section",
    "text": ""
},

{
    "location": "idx.html#",
    "page": "Graph Indexes",
    "title": "Graph Indexes",
    "category": "page",
    "text": ""
},

{
    "location": "idx.html#Indexes-1",
    "page": "Graph Indexes",
    "title": "Indexes",
    "category": "section",
    "text": ""
},

{
    "location": "idx.html#GraphIdx.NeighborIndex",
    "page": "Graph Indexes",
    "title": "GraphIdx.NeighborIndex",
    "category": "type",
    "text": "NeighborIndex(n, head, tail)\n\nConstruct an index for n nodes of the (undirected) neighbors given by the edges zip(head, tail). If head[i] <= 0 then edge i is excluded.\n\n\n\n\n\nNeighborIndex(n, edges)\n\nSame but for edges like [(1, 2), (2, 3)]\n\n\n\n\n\nNeighborIndex(n, m, iter::Function)\n\nProvide an iterator over the edges\n\n\n\n\n\n"
},

{
    "location": "idx.html#General-Graphs-1",
    "page": "Graph Indexes",
    "title": "General Graphs",
    "category": "section",
    "text": "GraphIdx.NeighborIndex"
},

{
    "location": "idx.html#GraphIdx.Cluster.cluster-Union{Tuple{N}, Tuple{Any,N}} where N",
    "page": "Graph Indexes",
    "title": "GraphIdx.Cluster.cluster",
    "category": "method",
    "text": "cluster(x, neigh, eps=1e-5, seed=42)\n\nFind a partition of x such that abs(x[i] - x[j]) < eps for all edges j in neighidx[i] where i and j are in the same partition.\n\n\n\n\n\n"
},

{
    "location": "idx.html#Graph-Partition-1",
    "page": "Graph Indexes",
    "title": "Graph Partition",
    "category": "section",
    "text": "Modules = [GraphIdx.Cluster]"
},

{
    "location": "idx.html#GraphIdx.Tree.ChildrenIndex",
    "page": "Graph Indexes",
    "title": "GraphIdx.Tree.ChildrenIndex",
    "category": "type",
    "text": "Provide constant time access to children of a node by index operator. By convention, the root node is stored at ChildrenIndex.idx[1].\n\n\n\n\n\n"
},

{
    "location": "idx.html#GraphIdx.Tree.reset!-Tuple{GraphIdx.Tree.ChildrenIndex,Array{Int64,1}}",
    "page": "Graph Indexes",
    "title": "GraphIdx.Tree.reset!",
    "category": "method",
    "text": "reset!(cidx, parent [,root])\n\nActually compute the index according to parent.\n\n\n\n\n\n"
},

{
    "location": "idx.html#Tree-Graphs-1",
    "page": "Graph Indexes",
    "title": "Tree Graphs",
    "category": "section",
    "text": "GraphIdx.Tree.ChildrenIndexOnce, memory has been allocated we can re-use it to store different tree (of same size!) in itGraphIdx.Tree.reset!(::GraphIdx.Tree.ChildrenIndex, ::Vector{Int})"
},

{
    "location": "grid.html#",
    "page": "Grid Graphs",
    "title": "Grid Graphs",
    "category": "page",
    "text": ""
},

{
    "location": "grid.html#GraphIdx.Grid",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid",
    "category": "module",
    "text": "Computations related to grid graphs\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.GridGraph",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.GridGraph",
    "category": "type",
    "text": "Capture all we need to know about a grid graph.\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.ImplicitGridGraph",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.ImplicitGridGraph",
    "category": "type",
    "text": "Idea: Do not store the graph explicitly but compute the neighbors as needed. For avoiding too many allocations, return just a view to the buffer\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.Pixel",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.Pixel",
    "category": "type",
    "text": "Position in a grid graph\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.compute_dirs-Tuple{Int64}",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.compute_dirs",
    "category": "method",
    "text": "compute_dirs(dn)\n\nSetting the neighborhood degree (dn) to i will set all numbers less or equal i in the following diagram as neighbors of [*].\n\n[…] […] […] […] […] […] […]\n[…] […] [4] [3] [4] […] […]\n[…] [4] [2] [1] [2] [4] […]\n[…] [3] [1] [*] [1] [3] […]\n[…] [4] [2] [1] [2] [4] […]\n[…] […] [4] [3] [4] […] […]\n[…] […] […] […] […] […] […]\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.iter_edges-Tuple{Function,Int64,Int64,Array{GraphIdx.Grid.Pixel,1}}",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.iter_edges",
    "category": "method",
    "text": "iter_edges(proc, n1, n2, dirs)\n\nCall proc(u::Int, v::Int, len::Float64) for every grid edge u -- v having length. Hereby u and v are the index of the corresponding grid-nodes.\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.iter_edges_pixel-Tuple{Function,Int64,Int64,Array{GraphIdx.Grid.Pixel,1}}",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.iter_edges_pixel",
    "category": "method",
    "text": "iter_edges_pixel(proc, n1, n2, dirs)\n\nCall proc(i1, j1, i2, j2, len) for every grid edge (i1, j1) -- (i2, j2) having length.\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.line_D-Tuple{Any}",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.line_D",
    "category": "method",
    "text": "line_D(n)\n\nReturn the incidence matrix for a line graph of length n. Is equivalent to incmat(1, n, 1)\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.lipschitz-Tuple{Any,Any,Array{GraphIdx.Grid.Pixel,1}}",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.lipschitz",
    "category": "method",
    "text": "lipschitz(n1, n2, [dn | dirs])\n\nCompute an upper bound for the Lipschitz-constant for ... TODO: Be more precise\n\n\n\n\n\n"
},

{
    "location": "grid.html#GraphIdx.Grid.num_edges-Tuple{Int64,Int64,Array{GraphIdx.Grid.Pixel,1}}",
    "page": "Grid Graphs",
    "title": "GraphIdx.Grid.num_edges",
    "category": "method",
    "text": "num_edges(n1, n2, dirs)\nnum_edges(n1, n2, dn::Int)\n\nNumber of edges in a grid graph n1×n2 along directions dirs. If called with last argument a number, first generate dirs.\n\n\n\n\n\n"
},

{
    "location": "grid.html#Grid-Graphs-1",
    "page": "Grid Graphs",
    "title": "Grid Graphs",
    "category": "section",
    "text": "Modules = [GraphIdx.Grid]<!– ### Under Development –> <!– @docs --> <!-- GraphIdx.Grid.ImplicitGridGraph -->"
},

{
    "location": "tree.html#",
    "page": "Tree Graphs",
    "title": "Tree Graphs",
    "category": "page",
    "text": ""
},

{
    "location": "tree.html#GraphIdx.Tree",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree",
    "category": "module",
    "text": "Bundle functions regarding tree graphs.\n\n\n\n\n\n"
},

{
    "location": "tree.html#Tree-Graphs-1",
    "page": "Tree Graphs",
    "title": "Tree Graphs",
    "category": "section",
    "text": "GraphIdx.TreeFor computing a spanning tree from within a general graph, see here."
},

{
    "location": "tree.html#GraphIdx.Tree.RootedTree",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree.RootedTree",
    "category": "type",
    "text": "A rooted tree. Often you might want to have a ChildrenIndex.\n\n\n\n\n\n"
},

{
    "location": "tree.html#GraphIdx.Tree.find_root",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree.find_root",
    "category": "function",
    "text": "find_root(pi)\n\nFind the first index i with pi[i] == i. Report an error if non such exist.\n\n\n\n\n\n"
},

{
    "location": "tree.html#GraphIdx.Tree.node_degrees",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree.node_degrees",
    "category": "function",
    "text": "node_degrees(parent, [root = 1])\n\nReturn a vector telling the degree of each node.\n\n\n\n\n\n"
},

{
    "location": "tree.html#Data-Structures-1",
    "page": "Tree Graphs",
    "title": "Data Structures",
    "category": "section",
    "text": "We provide some framework specialized for (rooted) tree graphs:GraphIdx.Tree.RootedTree\nGraphIdx.Tree.find_root\nGraphIdx.Tree.node_degreesOften, we will need to access the children of arbitrary nodes; for this the GraphIdx.Tree.ChildrenIndex was developed."
},

{
    "location": "tree.html#GraphIdx.Tree.dfs_walk",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree.dfs_walk",
    "category": "function",
    "text": "dfs_walk(f, tree [,stack])\n\nCall f on each node of tree in depth-first search (DFS) order. Hereby the node v is negative v < 0 if the node is discovered the first time and non-negative (v >= 0) if the node has been finished.\n\nThe tree can be given as ChildrenIndex or parent Vector{Int}; in the latter case ChildrenIndex will be constructed. To avoid allocation, you can pass a stack Vector.\n\n\n\n\n\n"
},

{
    "location": "tree.html#GraphIdx.Tree.binary_inorder",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree.binary_inorder",
    "category": "function",
    "text": "Return the Inorder traversal of a complete binary tree with given height. (Root node will be 0 and placed in the middle of the array\n\n\n\n\n\n"
},

{
    "location": "tree.html#Traversal-1",
    "page": "Tree Graphs",
    "title": "Traversal",
    "category": "section",
    "text": "Classical depth first searchGraphIdx.Tree.dfs_walkFor binary trees, there is also the inorder:GraphIdx.Tree.binary_inorder"
},

{
    "location": "tree.html#GraphIdx.Tree.lowest_common_ancestors",
    "page": "Tree Graphs",
    "title": "GraphIdx.Tree.lowest_common_ancestors",
    "category": "function",
    "text": "lowest_common_ancestors(tree, parent, pairs)\n\nFor a list of pairs [(u1, v1), ..., (uk, vk)] compute the lowest common ancestors [a1, ..., ak], i.e. ai is the lowest node in the tree that is ancestor (parent, grand-parent, grand-grand-parent, etc.) of both, ui and vi.\n\nRuntime is O(length(tree) + length(pairs)).  More precisely, the inverse Ackermann function is at most 4 for all practical instances.\n\nSee CLRS (3rd edition) page 584 or networkx [n].\n\nCompare to [n]: networkx/algorithms/lowestcommonancestors.py [1]: https://en.wikipedia.org/wiki/Lowestcommonancestor\n\n\n\n\n\n"
},

{
    "location": "tree.html#Lowest-Common-Ancestors-1",
    "page": "Tree Graphs",
    "title": "Lowest Common Ancestors",
    "category": "section",
    "text": "GraphIdx.Tree.lowest_common_ancestors"
},

{
    "location": "mst.html#",
    "page": "Minimum Spanning Trees",
    "title": "Minimum Spanning Trees",
    "category": "page",
    "text": ""
},

{
    "location": "mst.html#Minimum-Spanning-Trees-1",
    "page": "Minimum Spanning Trees",
    "title": "Minimum Spanning Trees",
    "category": "section",
    "text": ""
},

{
    "location": "mst.html#GraphIdx.prim_mst_edges",
    "page": "Minimum Spanning Trees",
    "title": "GraphIdx.prim_mst_edges",
    "category": "function",
    "text": "prim_mst_edges(g, weights, [root = 1])\n\nPrim\'s algorithm for minimum spanning tree. Start from node root (will become the root node of the spanning tree).  Return the parent vector and a Boolean vector indicating for each edge whether the edge is part of the spanning tree.\n\n\n\n\n\n"
},

{
    "location": "mst.html#GraphIdx.Utils.Heap",
    "page": "Minimum Spanning Trees",
    "title": "GraphIdx.Utils.Heap",
    "category": "module",
    "text": "PriorityQueue datastructure (based on a implicit binary heap). Code copied from DataStructures.jl (long time ago)\n\n\n\n\n\n"
},

{
    "location": "mst.html#Base.setindex!-Union{Tuple{V}, Tuple{K}, Tuple{PriorityQueue{K,V,O} where O<:Ordering,V,K}} where V where K",
    "page": "Minimum Spanning Trees",
    "title": "Base.setindex!",
    "category": "method",
    "text": "Change the priority of an existing element, or equeue it if it isn\'t present.\n\n\n\n\n\n"
},

{
    "location": "mst.html#Prim\'s-Algorithm-1",
    "page": "Minimum Spanning Trees",
    "title": "Prim\'s Algorithm",
    "category": "section",
    "text": "GraphIdx.prim_mst_edgesThe runtime is mathcalO(E log V) whereby E are the number of edges and V the number of nodes. We use an ordinary binary heap as priority queue:Modules = [GraphIdx.Utils.Heap]"
},

{
    "location": "mst.html#GraphIdx.kruskal_mst",
    "page": "Minimum Spanning Trees",
    "title": "GraphIdx.kruskal_mst",
    "category": "function",
    "text": "kruskal_mst(n, edges, weight)\n\nKruskal\'s minimum spanning tree algorithm. Return a vector indicating for each edge whether it is part of the spanning tree.\n\n\n\n\n\n"
},

{
    "location": "mst.html#GraphIdx.Utils.UnionFind",
    "page": "Minimum Spanning Trees",
    "title": "GraphIdx.Utils.UnionFind",
    "category": "type",
    "text": "UnionFind(n)\n\nClassical Union-Find data structure with path compression as described e.g. in CLRS in Chapter 21 \"Disjooint Sets\" (3rd edition).\n\n\n\n\n\n"
},

{
    "location": "mst.html#Kruskal\'s-Algorithm-1",
    "page": "Minimum Spanning Trees",
    "title": "Kruskal\'s Algorithm",
    "category": "section",
    "text": "GraphIdx.kruskal_mstAs you know, Kruskal\'s algorithm is best implemented using the UnionFind data structure which is useful on its own:GraphIdx.Utils.UnionFind"
},

{
    "location": "lina.html#",
    "page": "Linear Algebra",
    "title": "Linear Algebra",
    "category": "page",
    "text": ""
},

{
    "location": "lina.html#GraphIdx.LinA",
    "page": "Linear Algebra",
    "title": "GraphIdx.LinA",
    "category": "module",
    "text": "Linear Algebra related graph functions. Especially, methods for handling incidence matrixes IncMat.\n\n\n\n\n\n"
},

{
    "location": "lina.html#GraphIdx.LinA.IncMat",
    "page": "Linear Algebra",
    "title": "GraphIdx.LinA.IncMat",
    "category": "type",
    "text": "IncMat(edges, λ, n [, T = Int])\n\nCreate an incidence matrix from a list of edges, weights λ having n nodes. The resulting matrix index will have type T.\n\n\n\n\n\n"
},

{
    "location": "lina.html#GraphIdx.LinA.is_incmat-Tuple{SparseArrays.SparseMatrixCSC{Float64,Int64}}",
    "page": "Linear Algebra",
    "title": "GraphIdx.LinA.is_incmat",
    "category": "method",
    "text": "is_incmat(::IncMat)\n\nCheck whether a matrix is a graph (oriented) edge-vertex incidence matrix.\n\n\n\n\n\n"
},

{
    "location": "lina.html#Linear-Algebra-1",
    "page": "Linear Algebra",
    "title": "Linear Algebra",
    "category": "section",
    "text": "Modules = [GraphIdx.LinA]"
},

{
    "location": "bits.html#",
    "page": "Bits and Bytes",
    "title": "Bits and Bytes",
    "category": "page",
    "text": ""
},

{
    "location": "bits.html#GraphIdx.Bits.hyperfloor",
    "page": "Bits and Bytes",
    "title": "GraphIdx.Bits.hyperfloor",
    "category": "function",
    "text": "hyperfloor(x)\n\nHeighest power of 2 that is smaller than x.\n\nExample\n\njulia> bitstring(UInt8(123))\n\"01111011\"\n\njulia> bitstring(UInt8(hyperfloor(123)))\n\"01000000\"\n\n\n\n\n\n"
},

{
    "location": "bits.html#Bit-Manipulations-1",
    "page": "Bits and Bytes",
    "title": "Bit Manipulations",
    "category": "section",
    "text": "DocTestSetup  = quote\n    import GraphIdx.Bits.hyperfloor\nendGraphIdx.Bits.hyperfloor"
},

]}
