#pragma once
#include <vector>
#include <algorithm>    // for std::make_heap


template<typename key_t = int, typename value_t = double>
class Heap
{
    std::vector<value_t> order;

public:
    Heap(const size_t n) {
        order.reserve(n);
    }

    
};
