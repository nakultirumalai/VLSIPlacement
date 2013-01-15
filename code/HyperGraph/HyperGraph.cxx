# include <HyperGraph.h>
# include <Stat.h>
# include <time.h>
# include <Cell.h>
# include <stdlib.h>

unsigned int 
HyperGraph::AddNodeInt(void *object)
{

  unsigned int nodeIdx;

  Node *newNode = new Node();
  nodeIdx = numNodes++;
  (*newNode).NodeSetIdx(nodeIdx);
  /* Create link in the map */
  idx2Node[nodeIdx] = newNode;
  /* Create an entry in the connectivity table 
     for this object */
  Nodes2Edges.push_back(vector<unsigned int> ());
  nodeConnectivity.push_back(0);
  nodeSetIsTop(nodeIdx);

  (*newNode).NodeSetData(object);
  obj2idx[object] = nodeIdx;

  setDirty();

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
  setDirty();
  
  return (edgeIdx);
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

unsigned int
HyperGraph::getNewNodeIdx(void)
{
  return (HyperGraphGetNumNodes());
}

unsigned int
HyperGraph::getNewEdgeIdx(void)
{
  return (HyperGraphGetNumEdges());
}

bool
HyperGraph::clusterNodes(vector<unsigned int>& nodeSet, void* object)
{
  map<unsigned int, bool> visitedEdges;
  vector<unsigned int>& nodeSetCopy = nodeSet;
  vector<unsigned int> newNodeEdgeList;
  unsigned int nodeIdx, edgeIdx, newNodeIdx;
  bool result;
  bool debug = false;
  static int count;

  result = true;
  if (debug) 
    cout << "COUNT: " << count++ << " Size of node set is " << nodeSet.size() << endl;
  VECTOR_FOR_ALL_ELEMS(nodeSet, unsigned int, nodeIdx) {
    if (!nodeIsTop(nodeIdx)) {
      result = false;
      break;
    }
  } END_FOR;

  if (result == true) {
    /* Begin clustering here */
    newNodeIdx = AddNodeInt(object);
    clock_t start, finish;
    start = clock();
    unsigned int iter = 0;
    unsigned int added = 0;
    VECTOR_FOR_ALL_ELEMS(nodeSet, unsigned int, nodeIdx) {
      /* For each node in the node set, go over the edges of 
	 each node */
      if (debug) 
	cout << "   Node index: " << nodeIdx << " Iterating over its edges. Edge count: " << (Nodes2Edges[nodeIdx]).size() << endl;
      VECTOR_FOR_ALL_ELEMS((Nodes2Edges[nodeIdx]), unsigned int, edgeIdx) {
	iter++;
	if (visitedEdges.find(edgeIdx) == visitedEdges.end()) {
	  added++;
	  visitedEdges[edgeIdx] = 1;
	  newNodeEdgeList.push_back(edgeIdx);
	  (Edges2Nodes[edgeIdx])[newNodeIdx] = 1;
	}
	/* Remove nodeIdx from the map corresponding to edgeIdx */
	(Edges2Nodes[edgeIdx]).erase(nodeIdx);
      } END_FOR;
      nodeClearIsTop(nodeIdx);
    } END_FOR;

    finish = clock();
    cout << "    SIZE: " << nodeSet.size() << " TIME: " << (finish - start) << " CPU cycles." << endl;

    //   cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;    
    //ClusterInfo[newNodeIdx] = affectedEdgeList;
    Nodes2Edges[newNodeIdx] = newNodeEdgeList;
    nodeSetIsTop(newNodeIdx);
    nodeClearIsClusterParent(newNodeIdx);
  }
  
  return (result);
}

bool
HyperGraph::unclusterNode(unsigned int clusterNodeIdx)
{
  map<unsigned int, vector<unsigned int > > affectedEdgeList;
  vector<unsigned int> nodeList;
  unsigned int edgeIdx, nodeIdx;
  bool result = false;

  affectedEdgeList = ClusterInfo[clusterNodeIdx];
  if (affectedEdgeList.empty()) {
    return result;
  }
  MAP_FOR_ALL_ELEMS(affectedEdgeList, unsigned int, vector<unsigned int>, 
		   edgeIdx, nodeList) {
    (Edges2Nodes[edgeIdx]).erase(clusterNodeIdx);
    VECTOR_FOR_ALL_ELEMS(nodeList, unsigned int, nodeIdx) {
      (Edges2Nodes[edgeIdx])[nodeIdx] = 1;
      nodeSetIsTop(nodeIdx);
    } END_FOR;
  } END_FOR;
  (Nodes2Edges[clusterNodeIdx]).clear();
  ClusterInfo.erase(clusterNodeIdx);

  nodeClearIsTop(clusterNodeIdx);
  nodeClearIsClusterParent(clusterNodeIdx);

  result = true;

  return (result);
}

void 
HyperGraph::graphUpdate(void)
{
  int nodeIdx, numNodes;
  
  struct cmpObjStruct {
    bool operator() (int i,int j) { return (i<j);}
  } myCmpObj;

# if 0  
  numNodes = HyperGraphGetNumNodes();  
  for (nodeIdx = 0; nodeIdx < numNodes; nodeIdx++) {
    sort((Nodes2Edges[nodeIdx]).begin(), 
	 (Nodes2Edges[nodeIdx]).end(), 
	 myobject);
  }
# endif
  clearDirty();
}

void 
HyperGraph::setDirty(void)
{
  dirtyGraph = true;
}

void
HyperGraph::clearDirty(void)
{
  dirtyGraph = false;
}

/* Function to check, set and clear if a node index 
   represents a top node of a cluster */
void 
HyperGraph::nodeSetIsTop(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeSetIsTop();
}

void 
HyperGraph::nodeClearIsTop(unsigned int nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeClearIsTop();
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

/* PUBLIC FUNCTIONS BEGIN */
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
  Edges2Nodes.push_back(map<unsigned int, unsigned int> ());
  /* Collect the node indices. O(k) where there are k nodes 
     on an edge. */
  VECTOR_FOR_ALL_ELEMS(cellList, void*, object) {
    nodeIdx = obj2idx[object];
    nodeIdxArray.insert(nodeIdxArray.end(), nodeIdx);
    nodeIdxArrayCopy.insert(nodeIdxArrayCopy.end(), nodeIdx);
    nodePtr = idx2Node[nodeIdx];
    (*nodePtr).NodeAddEdge(edgeIdx);
    (*edgePtr).EdgeAddNode(nodeIdx);
    (Nodes2Edges[nodeIdx]).insert((Nodes2Edges[nodeIdx]).end(), edgeIdx);
    (Edges2Nodes[edgeIdx])[nodeIdx] = 1;
  } END_FOR;
}

bool
HyperGraph::HyperGraphClusterNodes(vector<vector<unsigned int > > nodesSet,
				   void *object)
{
  vector<unsigned int> clusterSet;
  bool success;
  
  success = true;
  VECTOR_FOR_ALL_ELEMS(nodesSet, vector<unsigned int>, clusterSet) {
    success &= clusterNodes(clusterSet, object);
    if (success == false) {
      break;
    }
  } END_FOR;
  
  return success;
}

bool 
HyperGraph::HyperGraphUnclusterNodes(vector<unsigned int> clusteredNodeSet) 
{
  unsigned int clusterNodeIdx;
  bool success;

  success = true;
  VECTOR_FOR_ALL_ELEMS(clusteredNodeSet, unsigned int, clusterNodeIdx) {
    success &= unclusterNode(clusterNodeIdx);
    if (success == false) {
      break;
    }
  } END_FOR;
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

vector<unsigned int>& 
HyperGraph::HyperGraphGetNodes(void)
{
  
}

vector<unsigned int>& 
HyperGraph::HyperGraphGetEdges(void)
{
  
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

bool 
HyperGraph::nodeIsClusterParent(unsigned int nodeIdx)
{
  Node *node;
  bool result;

  node = idx2Node[nodeIdx];
  result = (*node).NodeIsClusterParent();

  return (result);
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

HyperGraph::HyperGraph() 
{
  numEdges = 0;
  numNodes = 0;
}

/*** TEST FUNCTIONS ***/
void
HyperGraph::testClustering(void)
{
  vector<vector<unsigned int > > toBeClustered;
  vector<unsigned int> listOfNodes;
  unsigned int nodeIdx;

  cout << "Clustering nodes 0 1 and 2" << endl;
  listOfNodes.push_back(0);
  listOfNodes.push_back(1);
  listOfNodes.push_back(2);
  toBeClustered.push_back(listOfNodes);
  listOfNodes.clear();

  cout << "Clustering nodes 6 10 and 15" << endl;
  listOfNodes.push_back(6);
  listOfNodes.push_back(10);
  listOfNodes.push_back(15);
  toBeClustered.push_back(listOfNodes);
  listOfNodes.clear();
  
  //  HyperGraphClusterNodes(toBeClustered);
  cout << "Finished clustering" << endl;

  MAP_FOR_ALL_KEYS(ClusterInfo, unsigned int, mapOfVectors, nodeIdx) {
    listOfNodes.push_back(nodeIdx);
    cout << "Unclustering " << nodeIdx << endl;
  } END_FOR;
  //HyperGraphUnclusterNodes(listOfNodes);
  listOfNodes.clear();
}

vector<unsigned int> 
HyperGraph::getConnectedIndices(unsigned int nodeIdx)
{
  vector<unsigned int> returnNodes;
  vector<unsigned int> Edges;
  map<unsigned int, unsigned int> EdgeNodeList;
  unsigned int connectedEdgeIdx;
  unsigned int connectedNodeIdx;
  
  /* Find the vector of edges that nodeIdx has connections to */
  Edges = (this->Nodes2Edges)[nodeIdx];
  /* Iterate over all the edges */
  VECTOR_FOR_ALL_ELEMS(Edges, unsigned int, connectedEdgeIdx) {
    /* Get the list of nodes that are connected to edge index 
       connectedEdgeIdx */
    EdgeNodeList = (this->Edges2Nodes)[connectedEdgeIdx];
    /* Iterate over all keys of the EdgeNodeList map 
       as this would give the list of nodes that are 
       connected to the edge connectedEdgeIdx */
    MAP_FOR_ALL_KEYS(EdgeNodeList, unsigned int, unsigned int, connectedNodeIdx) {
      if (!nodeIsTop(connectedNodeIdx)) {
	continue;
      }
      if (connectedNodeIdx == nodeIdx) {
	continue;
      }
      returnNodes.push_back(connectedNodeIdx);
    } END_FOR;
  } END_FOR;
  
  return (returnNodes);
}

bool
HyperGraph::HyperGraphClusterCells(vector<void *> listOfCells, 
				   void* clusteredCell)
{
  vector<vector<unsigned int > > cellIndices;
  unsigned int subjectIdx;
  void *cellPtr;
  bool rtv;

  rtv = true;
  /* Push an empty member */
  cellIndices.push_back(vector<unsigned int> ());

  VECTOR_FOR_ALL_ELEMS(listOfCells, void*, cellPtr) {
    subjectIdx = obj2idx[cellPtr];
    _ASSERT("Unable to find cell in design database", (cellPtr == NIL(void*)));
    (cellIndices[0]).push_back(subjectIdx);
  } END_FOR;

  HyperGraphClusterNodes(cellIndices, clusteredCell);

  return (rtv);
}

vector<void *> 
HyperGraph::HyperGraphGetConnectedCells(void* cellPtr)
{
  unsigned int subjectIdx;
  unsigned int resultIdx;
  vector<unsigned int> connectedNodeIndices;
  vector<void *> rtvCells;
  Node *resultNode;
  
  subjectIdx = obj2idx[cellPtr];
  connectedNodeIndices = getConnectedIndices(subjectIdx);
  VECTOR_FOR_ALL_ELEMS(connectedNodeIndices, unsigned int, resultIdx) {
    _ASSERT("Subject node index matches with the subject node index", 
	    (resultIdx == subjectIdx));
    resultNode = idx2Node[resultIdx];
    rtvCells.push_back((*resultNode).NodeGetData());
  } END_FOR;

  return (rtvCells);
}
