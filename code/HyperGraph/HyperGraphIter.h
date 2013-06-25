# ifndef HYPERGRAPH_ITER_H
# define HYPERGRAPH_ITER_H

# include <HyperGraph.h>

# define HYPERGRAPH_FOR_ALL_NODES(Graph, NodeIdx, NodeObj)		\
  {									\
  unsigned int numNodes = Graph.GetNumNodes();				\
  for (NodeIdx=0; NodeIdx < numNodes; NodeIdx++) {			\
  if (!Graph.NodeIsTop(NodeIdx)) continue;				\
  NodeObj = Graph.GetNodeObject(NodeIdx);

# define HYPERGRAPH_FOR_ALL_EDGES(HyperGraph, EdgeIdx, EdgeWeight)	\
  {									\
  for (int __edgeIdx=0; __edgeIdx < HyperGraph.GetNumEdges(); __edgeIdx++) { \
  if (HyperGraph.EdgeIsHidden(__edgeIdx)) continue;			\
  EdgeIdx = __edgeIdx;							\
  EdgeWeight = HyperGraph.GetEdgeWeight(EdgeIdx);			
  

# define HYPERGRAPH_FOR_ALL_OBJS_OF_EDGE(HyperGraph, EdgeIdx, NodeObj) \
  {									\
  map<uint, uint> NodesOfEdge = HyperGraph.GetNodesOfEdge(EdgeIdx);	\
  map<uint, uint>::iterator nodeItr;					\
  for (nodeItr = NodesOfEdge.begin(); nodeItr != NodesOfEdge.end(); nodeItr++) { \
  NodeObj = HyperGraph.GetNodeObject(nodeItr->first);

# define HYPERGRAPH_FOR_ALL_NODES_OF_EDGE(HyperGraph, EdgeIdx, NodeIndex) \
  {									\
  map<uint, uint> NodesOfEdge = HyperGraph.GetNodesOfEdge(EdgeIdx);	\
  map<uint, uint>::iterator nodeItr;					\
  for (nodeItr = NodesOfEdge.begin(); nodeItr != NodesOfEdge.end(); nodeItr++) { \
  NodeIndex = nodeItr->first;

# define HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(HyperGraph, ObjPtr, EdgeIdx, Weight) \
  {									\
  vector<unsigned int> edgeIndices = HyperGraph.GetEdgesOfObject(ObjPtr); \
  unsigned int __edgeIdx;						\
  for (int idx = 0; idx < edgeIndices.size(); idx ++) {			\
  __edgeIdx = edgeIndices[idx];						\
  if (HyperGraph.EdgeIsHidden(__edgeIdx)) continue;			\
  EdgeIdx = __edgeIdx;							\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);				\

# define HYPERGRAPH_FOR_ALL_EDGES_OF_NODE(HyperGraph, NodeIdx, EdgeIdx, Weight) \
  {									\
  vector<unsigned int> edgeIndices = HyperGraph.GetEdgesOfNode(NodeIdx); \
  unsigned int __edgeIdx;						\
  for (int idx = 0; idx < edgeIndices.size(); idx ++) {			\
  __edgeIdx = edgeIndices[idx];						\
  if (HyperGraph.EdgeIsHidden(__edgeIdx)) continue;			\
  EdgeIdx = __edgeIdx;							\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);				\

# define HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT_NO_FILT(HyperGraph, ObjPtr, EdgeIdx, Weight) \
  {									\
  vector<unsigned int> edgeIndices = HyperGraph.GetEdgesOfObject(ObjPtr); \
  unsigned int __edgeIdx;						\
  for (int idx = 0; idx < edgeIndices.size(); idx ++) {			\
  __edgeIdx = edgeIndices[idx];						\
  EdgeIdx = __edgeIdx;							\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);				\

# define HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECTS(HyperGraph, ObjPtr1, ObjPtr2, EdgeIdx, Weight) \
  {									\
  vector<unsigned int> edgeIndices = HyperGraph.GetEdgesOfObjects(ObjPtr1, ObjPtr2); \
  unsigned int __edgeIdx;						\
  for (int idx = 0; idx < edgeIndices.size(); idx ++) {			\
  __edgeIdx = edgeIndices[idx];						\
  if (HyperGraph.EdgeIsHidden(__edgeIdx)) continue;			\
  EdgeIdx = __edgeIdx;							\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);				\

# define HYPERGRAPH_FOR_ALL_EDGES_OF_NODES(HyperGraph, NodeIdx1, NodeIdx2, EdgeIdx, Weight) \
  {									\
  vector<uint> edgeIndices = HyperGraph.GetEdgesOfNodes(NodeIdx1, NodeIdx2); \
  uint __edgeIdx;							\
  uint numEdges = edgeIndices.size();					\
  for (int idx = 0; idx < numEdges; idx ++) {				\
  __edgeIdx = edgeIndices[idx];						\
  if (HyperGraph.EdgeIsHidden(__edgeIdx)) continue;			\
  EdgeIdx = __edgeIdx;                                   		\
  Weight = HyperGraph.GetEdgeWeight(EdgeIdx);                           \

# define HYPERGRAPH_END_FOR }} 
# endif
