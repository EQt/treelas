GraphIdx
========

This is the documentation of the C++ part of GraphIdx.
For the Julia documentation see `here <../julia/index.html>`_.

The library is header only, so setting the include path apropriately suffices.

Node Type
---------

Within this library, nodes are represented by integers.
For being able to encode more than 2G nodes, mostly the used integer types ared templated with default to :code:`int`
(this often reduces the memory bandwidth compared to :code:`int64_t`).

Furtheron, we decided to use **signed** integers such that the first bit may encode additional information.

.. toctree::

   tree
   idx
   utils
