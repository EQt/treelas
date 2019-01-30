#include "../bits/clz.hpp"


/**
   Frederik Rønn: "Cache-Oblivious Searching and Sorting", Master's Thesis,
   Department of Computer Science University of Copenhagen, 2003.
   Page 76:
*/   
template<typename Iter>
void build_hp_tree(const Iter begin_in,
                   Iter begin_out,
                   int height,
                   int step)
{
    int bottom_height = height == 2 ? 1 : int(hyperfloor(height-1));
    int top_height    = height - bottom_height;
    int bottom_size   = (1 << bottom_height) -1;
    int top_size      = (1 << top_height) -1;

    if (top_height == 1 && bottom_height == 1) {
        begin_out[1] = begin_in[0];
        begin_out[0] = begin_in[1*step];
        begin_out[2] = begin_in[2*step];
        return;
    }
    if (top_height == 1) {
        begin_out[0] = begin_in[bottom_size*step];
    } else {
        build_hp_tree(begin_in+bottom_size*step,
                      begin_out,
                      top_height,
                      bottom_size*step+step);
    }
    for(int i = 0; i <= top_size; i++) {
        build_hp_tree(begin_in+(i*bottom_size+i)*step,
                      begin_out+top_size+i*bottom_size,
                      bottom_height,
                      step);
    }
}
