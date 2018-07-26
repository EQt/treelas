#pragma once


double*
tree_dual(const int n,
          double *x,
          const int *parent,
          const int *_postord,
          double *alpha,
          const int root);
