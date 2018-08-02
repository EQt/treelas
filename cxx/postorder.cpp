#include "postorder.hpp"
#include "utils/timer.hpp"


void
post_order(const int root,
           const ChildrenIndex &childs,
           stack<int> &stack,
           int *postord)
{
    {   Timer _ ("alloc stack");
        stack.reserve(2*childs.size());
        stack.push_back(root);
    }
    Timer _ ("dfs walk");
    int t = 0, pos = 0;
    while (!stack.empty()) {
        t++;
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


int*
post_order(const size_t n,
           const int *parent,
           const int root,
           int *postord)
{
    if (postord == nullptr) {
        postord = new int[n];
    }
    Timer tim("children index");
    ChildrenIndex childs (parent, n, root);
    tim.stop();
    stack<int> stack;
    post_order(root, childs, stack, postord);
    return postord;
}
