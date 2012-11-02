# include <HyperGraph.h>
# include <Cell.h>
# include <Stat.h>

unsigned int 
HyperGraph::AddNodeInt(void *object)
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
  nodeConnectivity.push_back(0);

  return (nodeIdx);
}

unsigned int 
HyperGraph::AddEdgeInt(void *object, unsigned int weight)
{
  unsigned int edgeIdx;

  Edge *newEdge = new Edge(object);
  edgeIdx = numEdges++;
  (*newEdge).EdgeSetIdx(edgeIdx);
  (*newEdge).EdgeSetWeight(weight);

  /* Create link in the map */
  idx2Edge[edgeIdx] = newEdge;
  
  return (edgeIdx);
}

void 
HyperGraph::HyperGraphAddNode(void *object)
{
  AddNodeInt(object);
}

void 
HyperGraph::HyperGraphAddEdge(vector<void *> &cellList, void *EdgeObject, 
			 unsigned int weight)
{
  vector<unsigned int> nodeIdxArray;
  vector<unsigned int> nodeIdxArrayCopy;
  Node *nodePtr;
  Edge *edgePtr;
  unsigned int nodeIdx, nodeIdxCopy;
  unsigned int edgeIdx;
  void *object;

  edgeIdx = AddEdgeInt(EdgeObject, weight);
  edgePtr = idx2Edge[edgeIdx];
  /* Collect the node indices. O(k) where there are k nodes 
     on an edge. */
  VECTOR_FOR_ALL_ELEMS(cellList, void*, object) {
    nodeIdx = obj2idx[object];
    nodeIdxArray.insert(nodeIdxArray.end(), nodeIdx);
    nodeIdxArrayCopy.insert(nodeIdxArrayCopy.end(), nodeIdx);
    nodePtr = idx2Node[nodeIdx];
    (*nodePtr).NodeAddEdge(edgeIdx);
    (*edgePtr).EdgeAddNode(nodeIdx);
  } END_FOR;

  cout << "DEBUG1      :Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
  
  /* Establish adjacency of all nodes on the edge 
     This is an O(k^2). */
  VECTOR_FOR_ALL_ELEMS(nodeIdxArray, unsigned int, nodeIdx) {
    VECTOR_FOR_ALL_ELEMS(nodeIdxArrayCopy, unsigned int, nodeIdxCopy) {
      if (nodeIdxCopy != nodeIdx) {
	pair<int,int> mypair(nodeIdxCopy, edgeIdx);
	(Connectivity[nodeIdx]).push_back(mypair);
      }
    } END_FOR;
    nodeConnectivity[nodeIdx] = nodeConnectivity[nodeIdx] + getEdgeWeight(edgeIdx);
  } END_FOR;

  cout << "DEBUG2      :Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
}

unsigned int
HyperGraph::getNodeConnectivity(unsigned int nodeIdx)
{
  
}

unsigned int 
HyperGraph::getEdgeWeight(unsigned int edgeIdx)
{
  Edge *thisEdge;
  unsigned int edgeWeight;
  
  thisEdge = idx2Edge[edgeIdx];
  if (thisEdge != NULL) {
    edgeWeight = (*thisEdge).EdgeGetWeight();
  }
  
  return edgeWeight;
}


/* Function to check, set and clear if a node index 
   represents a top node of a cluster */
void 
HyperGraph::nodeSetIsTop(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeIsTop();
}

void 
HyperGraph::nodeClearIsTop(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeClearIsTop();
}

bool 
HyperGraph::nodeIsTop(unsigned int nodeIdx)
{
  Node *node;
  bool result;

  node = idx2Node[nodeIdx];
  result = (*node).NodeIsTop();
  
  return (result);
}

/* Function to check, set and clear if a node index represents a child node of a cluster */
void 
HyperGraph::nodeSetIsClusterParent(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeSetIsClusterParent();
}

void 
HyperGraph::nodeClearIsClusterParent(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeClearIsClusterParent();
}

bool 
HyperGraph::nodeIsClusterParent(unsigned int nodeIdx)
{
  Node *node;
  bool result;

  node = idx2Node[nodeIdx];
  result = (*node).NodeIsClusterParent();

  return (result);
}

/* Function to check, set and clear if a node index represents a child node of a cluster */
void 
HyperGraph::nodeSetIsClusterChild(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeSetIsClusterChild();
}

void 
HyperGraph::nodeClearIsClusterChild(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeClearIsClusterChild();
}

bool 
HyperGraph::nodeIsClusterChild(unsigned int nodeIdx)
{
  Node *node;
  bool result;

  node = idx2Node[nodeIdx];
  result = (*node).NodeIsClusterChild();

  return (result);
}

vector<unsigned int>& 
HyperGraph::HyperGraphGetNodes(void)
{
  
}

vector<unsigned int>& 
HyperGraph::HyperGraphGetEdges(void)
{
  
}

unsigned int 
HyperGraph::HyperGraphGetNumNodes(void)
{
  return (numNodes);
}

unsigned int 
HyperGraph::HyperGraphGetNumEdges(void)
{
  return (numEdges);
}

HyperGraph::HyperGraph() 
{
  numEdges = 0;
  numNodes = 0;
}

