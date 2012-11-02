# ifndef HYPERGRAPH_ITER_H
# define HYPERGRAPH_ITER_H

# define HYPERGRAPH_FOR_ALL_NODES(HyperGraph, NodeIdx) \
  { \
  vector<unsigned int> NodeSet = HyperGraph.HyperGraphGetNodes(); \
  vector<unsigned int>::iterator vectIter; \
  for (vectIter = NodeSet.begin(); vectIter != NodeSet.end(); vectIter++) { \
  NodeIdx = (unsigned int)(*vectIter); \

# define HYPERGRAPH_FOR_ALL_NODES_PTR(HyperGraph, NodePtr) \
  { \
  vector<Node *> NodeSet = HyperGraph.HyperGraphGetNodes(); \
  vector<Node *>::iterator vectIter; \
  for (vectIter = NodeSet.begin(); vectIter != NodeSet.end(); vectIter++) { \
  NodeIdx = (Node *)(*vectIter); \

# define HYPERGRAPH_FOR_ALL_EDGES(HyperGraph, EdgeIdx) \
  { \
  vector<unsigned int> EdgeSet = HyperGraphGetEdges(HyperGraph); \
  vector<unsigned int>::iterator vectIter; \
  for (vectIter = EdgeSet.begin(); vectIter != EdgeSet.end(); vectIter++) { \
  EdgeIdx = (unsigned int)(*vectIter); \

# define HYPERGRAPH_END_FOR }} 
# endif
