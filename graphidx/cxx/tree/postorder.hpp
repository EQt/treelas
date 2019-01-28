#pragma once
#include <stdexcept>
#include <string>

#include "std/stack.hpp"
#include "idx/children.hpp"
#include "../../cxx/utils/timer.hpp"



/**
   Compute a post_order, i.e. parent come after its children;
   more specific the DFS finish time.

   If postord is preallocated, it must have a length >= n.
   If postord  == nullptr, it will be allocated
   (giving the responsible of `delete[]` to the caller).
 */
template <typename int_ = int>
int_*
post_order(const size_t n,
           const int_ *parent,
           const int_ root = 0,
           int_ *postord = nullptr)
{
    if (postord == nullptr) {
        postord = new int_[n];
    }
    Timer tim("children index");
    ChildrenIndex childs (n, parent, root);
    tim.stop();
    stack<int_> stack;
    post_order(root, childs, stack, postord);
    return postord;
}


template <typename int_ = int>
void
post_order(const int_ root,
           const ChildrenIndex &childs,
           stack<int_> &stack,
           int_ *postord)
{
    {   Timer _ ("alloc stack");
        stack.reserve(2*childs.size());
        stack.push_back(root);
    }
    Timer _ ("dfs walk");
    size_t pos = 0;
    while (!stack.empty()) {
        const auto v = stack.back();
        stack.pop_back(); // stack.pop()
        if (v >= 0) {
            stack.push_back(-v-1);
            for (auto i : childs[v]) {
                stack.push_back(i);
            }
        } else {
            postord[pos++] = -v-1;
        }
    }
}

/**
   Compute a post_order (if called with nullptr).
   The array is released when this is destructed.
   Additionally, the iterator interface (std::begin, std::end) is provided.
*/
class PostOrder
{
    const int *postord;
    const size_t n;
    const bool include_root;
    std::vector<int> _postord_buf;

public:
    PostOrder(const std::vector<int> &parent,
              const int *postord = nullptr,
              const int root = 0,
              const bool include_root = false)
        : PostOrder(parent.size(),
                    parent.data(),
                    postord,
                    root,
                    include_root) { }

    PostOrder(size_t n,
              const int *parent,
              const int *postord = nullptr,
              const int root = 0,
              const bool include_root = false)
        : postord(postord), n(n), include_root(include_root) {
        if (this->postord == nullptr) {
            _postord_buf.reserve(n);
            this->postord =
                post_order(n, parent, root, _postord_buf.data());
        }
    }

    const int* begin() const { return postord; }

    const int* end() const { return postord + (include_root ? n : n-1); }
    
    size_t size() const { return end() - begin(); }
};
