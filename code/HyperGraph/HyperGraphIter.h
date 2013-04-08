# ifndef HYPERGRAPH_ITER_H
# define HYPERGRAPH_ITER_H

# include <HyperGraph.h>

# define HYPERGRAPH_FOR_ALL_NODES(Graph, NodeIdx) \
  { \
  for (int nodeIdx=0; nodeIdx < Graph.HyperGraphGetNumNodes(); nodeIdx++) {	\
    if (Graph.nodeIsTop(nodeIdx)) continue; \
  
# define HYPERGRAPH_FOR_ALL_EDGES(HyperGraph, EdgeIdx, EdgeWeight) \
  { \
  for (int edgeIdx=0; edgeIdx < HyperGraph.GetNumEdges(); edgeIdx++) {	\
  EdgeIdx = edgeIdx;							\
  EdgeWeight = HyperGraph.GetEdgeWeight(EdgeIdx); 

# define HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(HyperGraph, ObjPtr, EdgeIdx, Weight) \
  {									\
  vector<unsigned int> edgeIndices = HyperGraph.GetEdgesOfNode(ObjPtr); \
  unsigned int edgeIdx;							\
  for (int idx = 0; idx < edgeIndices.size(); idx ++) {			\
  EdgeIdx = edgeIndices[idx];						\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);				\

# define HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECTS(HyperGraph, ObjPtr1, ObjPtr2, EdgeIdx, Weight) \
  {									\
  vector<unsigned int> edgeIndices = HyperGraph.GetEdgesOfNodes(ObjPtr1, ObjPtr2); \
  unsigned int edgeIdx;							\
  for (int idx = 0; idx < edgeIndices.size(); idx ++) {			\
  EdgeIdx = edgeIndices[idx];						\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);				\

# define HYPERGRAPH_END_FOR }} 
# endif
