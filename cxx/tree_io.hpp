#pragma once
#include "tree.hpp"
#include "utils/hdf5.hpp"


template <typename float_, typename int_>
TreeLasso<float_, int_>
load_treelasso(const char *fname, const char *group)
{
    if (std::string("/") != group) {
        throw std::logic_error("Not implemented yet");
    }

    TreeLasso<float_, int_> t;
    {   Timer _ ("Loading Tree");
        HDF5 io (fname, "r+");
        t.y = io.read<typename decltype(t.y)::value_type>("y");
        t.lam = io.read<typename decltype(t.lam)::value_type>("lam");
        t.dfs = io.read<typename decltype(t.dfs)::value_type>("dfs"),
        t.parent = io.read<typename decltype(t.dfs)::value_type>("parent");
        assert(t.lam.size() >= 1);
        // lam = float_(lams[0]);
        assert(t.dfs.size() >= 1);
        t.root = t.dfs[0];
        if (t.root == 1) {
            t.root--;
            for (auto &i : t.dfs)
                i--;
            for (auto &i : t.parent)
                i--;
        }
        if (io.has("bfs")) {
            t.bfs = io.read<typename decltype(t.bfs)::value_type>("bfs");
        }
        t.mu = io.has("mu") ?
            io.read<typename decltype(t.mu)::value_type>("mu") :
            decltype(t.mu)({0.5});
    }
    return t;
}


void
load_tree(const char * /*fname*/, const char * /*group = "/" */)
{
    throw std::logic_error("Not implemented, yet");
}
