Graph Indexes
=============

The unique feature of this graph framework is to compute indices to be able to access certain elements in constant time.
The index computation is always based on counting sort to group certain elements together by an index:

.. doxygenfile:: idx/groupby.hpp
   :project: graphidx


Neighbor Index
--------------

.. doxygenfile:: idx/biadjacent.hpp
   :project: graphidx


Partition Index
---------------

.. doxygenfile:: idx/cluster.hpp
   :project: graphidx

.. doxygenclass:: PartitionIndex
   :members:
   :project: graphidx
