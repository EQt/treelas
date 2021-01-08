/**
   Additional functions to handle edges.
 */
#pragma once
#include "graphidx/idx/incidence.hpp"
#include <functional>

#include <graphidx/idx/biadjacent.hpp>


template <typename int_t = int>
void
edges(const BiAdjacentIndex<int_t> &idx, std::function<void(int_t, int_t)> f)
{
    for (int_t v = 0; v < (int_t) idx.num_nodes(); v++) {
	for (int_t u : idx[v]) {
	    f(v, u);
	}
    }
}


template <typename int_t = int>
void
edges(const IncidenceIndex<int_t> &idx, std::function<void(int_t, int_t, int_t)> f)
{
    for (int_t u = 0; u < (int_t) idx.num_nodes(); u++) {
	for (const auto [v, ei] : idx[u]) {
	    f(u, v, ei);
	}
    }
}
