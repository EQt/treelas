#pragma once
#include "stack.hpp"
#include "idx/children.hpp"


/**
   Compute a post_order, i.e. parent come after its children;
   more specific the DFS finish time.

   If postord is preallocated, it must have a length >= n.
   If postord  == nullptr, it will be allocated.
 */
int*
post_order(const size_t n,
           const int *parent,
           const int root = 0,
           int *postord = nullptr);

void
post_order(const int root,
           const ChildrenIndex &childs,
           stack<int> &stack,
           int *postord);

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
