/**
   Additional functions to handle edges.
 */
#pragma once
#include <functional>

#include <graphidx/idx/index.hpp>


template <typename int_t = int>
void
edges(const Index<int_t> &idx, std::function<void(int_t, int_t)> f)
{
    for (int_t v = 0; v < (int_t) idx.size(); v++) {
	for (int_t u : idx[v]) {
	    if (v < u)
		f(v, u);
	}
    }
}
