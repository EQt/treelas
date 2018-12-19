/*
  All stuff related to queue, i.e. init (correct order) and merge.
 */
#include <algorithm>        // for std::sort
#include <cstring>          // for std::memmove

#include "utils/timer.hpp"

#include "stack.hpp"
#include "range.hpp"
#include "idx/children.hpp"


/**
   Initialize the queue list and processing order for dp_tree.

   proc_order: Post order (parent after their children), excluding root.

   timc: If not NULL, allocate a Timer before returning.
         This makes it possible to measure the time needed for deallocation.
*/
inline void
init_queues(const size_t n,
            std::vector<Range> &pq,
            std::vector<int> &proc_order,
            const ChildrenIndex &childs,
            stack<int> &stack,
            int root = 0)
{
    {
        proc_order.reserve(n);
        stack.reserve(n);
        proc_order.clear();
        stack.clear();
    }
    {
        Timer _ ("init_queue: dfs");
        stack.push_back(root);
        int t = 0;
        while (!stack.empty()) {
            t++;
            const auto v = stack.back();
            stack.pop_back();
            if (v >= 0) {
                stack.push_back(-v-1);
                for (const auto i : childs[v])
                    stack.push_back(i);
            } else {
                const auto w = -v -1;
                proc_order.push_back(w);
                pq[w] = Range({t+0, t-1});
            }
        }
        proc_order.pop_back();
    }
}



template <typename E>
inline void
sort_events(const Range &range, E *elements)
{
    std::sort(elements + range.start, elements + range.stop + 1);
}


template <typename E>
inline Range
merge(const Range &parent, const Range &child, E *elements)
{
    if (parent.start <= parent.stop) {
        const auto gap = child.start - parent.stop - 1;
        const auto old_stop = parent.stop;
        const Range res {parent.start, child.stop - gap};
        if (gap > 0) {
            for (int i = old_stop+1; i <= res.stop; i++) {
                elements[i] = elements[i + gap];
            }
        }
        // std::sort(elements + res.start, elements + res.stop + 1);
        return res;
    }
    return child;
}


template <typename E>
inline Range
merge2(const Range &parent, const Range &child, E *elements)
{
    static std::vector<E> buf (15);
    if (parent.start <= parent.stop) {
        const auto gap = child.start - parent.stop -1;
        const Range res {parent.start, child.stop - gap};
        buf.reserve(parent.length());
        buf.clear();
        /*
          for (int i = 0, j = parent.start; j <= parent.stop; j++)
              buf[i++] = elements[j];
        */
        std::memmove(buf.data(), elements + parent.start,
                     parent.length() * sizeof(E));
        const auto buf_end = parent.length();
        int
            l = 0,
            r = child.start;
        for (int k = res.start; k <= res.stop; k++) {
            elements[k] = (l < buf_end &&
                           (r > child.stop || buf[l].x < elements[r].x)) ?
                buf[l++] : elements[r++];
        }
        return res;
    }
    return child;
}


template <typename E>
inline Range
merge(const Range &parent, const Range &child, std::vector<E> &elements)
{
    return merge(parent, child, elements.data());
}


template <typename E>
inline Range
merge2(const Range &parent, const Range &child, std::vector<E> &elements)
{
    return merge2(parent, child, elements.data());
}
