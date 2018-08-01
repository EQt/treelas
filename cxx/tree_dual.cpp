#include "tree_dual.hpp"

#include <vector>
#include <stdexcept>
#include <cmath>

#include "postorder.hpp"


double*
tree_dual(const size_t n,
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
    if (alpha == nullptr)
        alpha = new double[n];

    alpha[root] = std::nan("");

    for (const auto c : postorder) {
        const auto v = parent[c];
        alpha[c] = c > v ?     -x[c] : +x[c];
        x[v]    += c > v ? -alpha[c] : +alpha[c];
    }

    return alpha;
}


void
tree_dual_gap(
    const size_t n,
    double *gamma,
    const double *x,
    const double *alpha,
    const double *lam,
    const int *parent,
    const double root_val)
{
    for (int i = 0; i < int(n); i++) {
        int p = parent[i];
        const double diff = x[i] - x[p];
        gamma[i] = (i == p) ? root_val :
            alpha[i] / lam[i] * diff + std::abs(diff);
    }
}


void
tree_dual_gap(
    const size_t n,
    double *gamma,
    const double *x,
    const double *alpha,
    const double lam,
    const int *parent,
    const double root_val)
{
    for (int i = 0; i < int(n); i++) {
        int p = parent[i];
        const double diff = x[i] - x[p];
        gamma[i] = (i == p) ? root_val :
            alpha[i] / lam * diff + std::abs(diff);
    }
}
