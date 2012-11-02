# ifndef HYPERGRAPH_ITER_H
# define HYPERGRAPH_ITER_H

# define HYPERGRAPH_FOR_ALL_NODES(Graph, NodeIdx) \
  { \
  for (int nodeIdx=0; nodeIdx < Graph.HyperGraphGetNumNodes(); nodeIdx++) {	\
    if (Graph.nodeIsTop(nodeIdx)) continue; \
  

# define HYPERGRAPH_FOR_ALL_EDGES(HyperGraph, EdgeIdx) \
  { \
  for (int edgeIdx=0; edgeIdx < Graph.HyperGraphGetNumNodes(); edgeIdx++) {	\
  
# define HYPERGRAPH_END_FOR }} 
# endif
