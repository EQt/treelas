#pragma once
#include <vector>


template <typename float_ = double, typename int_ = int>
struct TreeLasso
{
    int_ root = 0;
    std::vector<float_> y, lam, mu;
    std::vector<int_> parent;

    // optional orders: .size() == 0 or .size() == n
    std::vector<int_> dfs, bfs;
    std::vector<int_> postorder, ipostord;
    std::vector<int_> preorder;     // maybe deleted in the future
};
