# include <HyperGraph.h>
# include <Cell.h>

int HyperGraph::AddNodeInt(void *object)
{
  unsigned int nodeIdx;

  Node *newNode = new Node(object);
  nodeIdx = numNodes++;
  (*newNode).NodeSetIdx(nodeIdx);
  /* Create link in the map */
  idx2Node[nodeIdx] = newNode;
  /* Create link for the object */
  obj2idx[object] = nodeIdx;
  /* Create an entry in the connectivity table 
     for this object */
  Connectivity.push_back(vector<pair<int,int> > ());
  
  return (nodeIdx);
}

int HyperGraph::AddEdgeInt(void *object)
{
  unsigned int edgeIdx;

  Edge *newEdge = new Edge(object);
  edgeIdx = numEdges++;
  (*newEdge).EdgeSetIdx(edgeIdx);

  /* Create link in the map */
  idx2Edge[edgeIdx] = newEdge;
  
  return (edgeIdx);
}

void HyperGraph::AddEdge(vector<void *> &cellList, void *EdgeObject)
{
  vector<unsigned int> nodeIdxArray;
  vector<unsigned int> nodeIdxArrayCopy;
  unsigned int nodeIdx, nodeIdxCopy;
  unsigned int edgeIdx;
  void *object;

  edgeIdx = AddEdgeInt(EdgeObject);
  /* Collect the node indices. O(k) where there are k nodes 
     on an edge. */
  VECTOR_FOR_ALL_ELEMS(cellList, void*, object) {
    nodeIdxArray.insert(nodeIdxArray.end(), obj2idx[object]);
    nodeIdxArrayCopy.insert(nodeIdxArrayCopy.end(), obj2idx[object]);
  } END_FOR;

  /* Establish adjacency of all nodes on the edge 
     This is an O(k^2). */
  VECTOR_FOR_ALL_ELEMS(nodeIdxArray, unsigned int, nodeIdx) {
    VECTOR_FOR_ALL_ELEMS(nodeIdxArrayCopy, unsigned int, nodeIdxCopy) {
      if (nodeIdxCopy != nodeIdx) {
	pair<int,int> mypair(nodeIdxCopy, edgeIdx);
	(Connectivity[nodeIdx]).push_back(mypair);
      }
    } END_FOR;
  } END_FOR;
}

void HyperGraph::AddNode(void *object)
{
  (void) AddNodeInt(object);
}

unsigned int HyperGraph::GetNumNodes(void)
{
  return (numNodes);
}

unsigned int HyperGraph::GetNumEdges(void)
{
  return (numEdges);
}

HyperGraph::HyperGraph() 
{
  numEdges = 0;
  numNodes = 0;
}

