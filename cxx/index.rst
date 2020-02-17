TreeLas
=======

This is the documentation of the C++ part of TreeLas.
For the Julia documentation see `here <../index.html>`_.

This is a *fused Lasso* Solver for tree graphs, i.e. we aim to **minimize** the following objective function

.. math::
   f(x) = \frac{1}{2} \sum_{i=1}^n \mu_i (x_i - y_i)^2 + \sum_{(i,j) \in E} \lambda_{ij} |x_i - x_j|

Hereby is
 * :math:`y_i` the input signal for node :math:`i`,
 * :math:`\mu_i \geq 0` is the node weight for node :math:`i`,
 * :math:`E` the edges, usually from a tree, and
 * :math:`\lambda_{ij} > 0` is the edge weight for :math:`(i,j)`.


More Concepts
-------------

.. toctree::

   clip
