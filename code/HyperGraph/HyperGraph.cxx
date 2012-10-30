# include <HyperGraph.h>
# include <Cell.h>

int HyperGraph::AddNodeInt(void *object)
{
  unsigned int nodeIdx;

  Node *newNode = new Node(object);
  nodeIdx = numNodes++;
  (*newNode).NodeSetIdx(nodeIdx++);

  /* Create link in the map */
  idx2Node[nodeIdx] = newNode;
  /* Create link for the object */
  obj2idx[object] = nodeIdx;
  
  return (nodeIdx);
}

int HyperGraph::AddEdgeInt(void *object)
{
  unsigned int edgeIdx;

  Edge *newEdge = new Edge(object);
  edgeIdx = numEdge++;
  (*newEdge).EdgeSetIdx(edgeIdx++);

  /* Create link in the map */
  idx2Edge[edgeIdx] = newEdge;
  
  return (nodeIdx);
}

void HyperGraph::AddEdge(vector<void *> &cellList, void *EdgeObject)
{
  vector<unsigned int> nodeIdxArray;
  unsigned int nodeIdx;
  void *object;

  AddEdgeInt(EdgeObject);
  VECTOR_FOR_ALL_ELEMS(cellList, void*, object) {
    nodeIdxArray.insert(nodeIdxArray.end(), obj2idx[object]);
  } END_FOR;
  
  VECTOR_FOR_ALL_ELEMS(nodeIdxArray, unsigned int, nodeIdx) {
    
  } END_FOR;

}

void HyperGraph::AddNode(void *object)
{
  Node *newNode;

  newNode = nodeList[(void*)object];
  if (newNode == NULL) {
    newNode = new Node(object);
    AddNodeInt(object, newNode);
  }
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
}

