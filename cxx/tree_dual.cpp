#include <vector>
#include <stdexcept>
#include <cmath>

#include "postorder.hpp"


double*
tree_dual(const int n,
          double *x,
          const int *parent,
          const int *_postord,
          double *alpha,
          const int root)
{
    if (parent[root] != root) {
        throw std::runtime_error(std::string("dp_dual(): ") +
                                 "root = " + std::to_string(root) +
                                 " != " + std::to_string(parent[root]) +
                                 " = parent[root]");
    }
    PostOrder postorder (n, parent, _postord, root);
    if (alpha == nullptr) {
        alpha = new double[n];
    }
    alpha[root] = std::nan("");

    for (const auto c : postorder) {
        const auto v = parent[c];
        alpha[c] = c > v ?     -x[c] : +x[c];
        x[v]    += c > v ? -alpha[c] : +alpha[c];
    }

    return alpha;
}
