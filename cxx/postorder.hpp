#pragma once
#include "children.hpp"

/**
   Compute a post_order; more specific the DFS finish time.
   If postord is preallocated, it has to have a length >= n
 */
int*
post_order(const int n,
           const int *parent,
           const int root = 0,
           int *postord = nullptr);


class PostOrder
{
    const int *postord;
    const int n;
    const bool include_root;
    std::vector<int> _postord_buf;

public:
    PostOrder(const std::vector<int> &parent,
              const int *postord = nullptr,
              const int root = 0,
              const bool include_root = false)
        : PostOrder(int(parent.size()),
                    parent.data(),
                    postord,
                    root,
                    include_root) { }

    PostOrder(int n,
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

    const int* begin() const {
        return postord;
    }

    const int* end() const {
        return postord + (include_root ? n : n-1);
    }
    
    int size() const {
        return end() - begin();
    }
};
