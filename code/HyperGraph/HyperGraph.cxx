# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <Stat.h>
# include <time.h>
# include <Cell.h>
# include <stdlib.h>

unsigned int 
HyperGraph::AddNodeInt(void *object)
{

  unsigned int nodeIdx;

  if (nodeExists(object)) {
    nodeIdx = HyperGraphGetCellIndex(object);
  } else {
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
  }
  return (nodeIdx);
}

bool
HyperGraph::nodeExists(void *object)
{
  bool rtv;
  
  rtv = false;
  if (obj2idx.find(object) != obj2idx.end()) {
    rtv = true;
  }
  
  return (rtv);
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
HyperGraph::GetEdgeWeight(unsigned int edgeIdx)
{
  Edge *thisEdge;
  unsigned int edgeWeight;
  
  thisEdge = idx2Edge[edgeIdx];
  if (thisEdge != NULL) {
    edgeWeight = (*thisEdge).EdgeGetWeight();
  }
  
  return edgeWeight;
}

map<unsigned int, unsigned int>&
HyperGraph::getEdgeConnectivity(unsigned int edgeIdx)
{
  return ((*this).Edges2Nodes[edgeIdx]);
}

unsigned int
HyperGraph::getNewNodeIdx(void)
{
  return (HyperGraphGetNumNodes());
}

unsigned int
HyperGraph::getNewEdgeIdx(void)
{
  return (GetNumEdges());
}

bool
HyperGraph::clusterNodes(vector<unsigned int>& nodeSet, void* object,
			 vector<void *>& affectedNets)
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
	if ((Edges2Nodes[edgeIdx]).size() == 1) {
	  Edge &thisEdge = (*idx2Edge[edgeIdx]);
	  affectedNets.push_back(thisEdge.EdgeGetObject());
	}
      } END_FOR;
      nodeClearIsTop(nodeIdx);
    } END_FOR;

    finish = clock();
    //cout << "    SIZE: " << nodeSet.size() << " TIME: " << (finish - start) << " CPU cycles." << endl;

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

bool
HyperGraph::HyperGraphNodeExists(void *object)
{
  return nodeExists(object);
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
HyperGraph::GetNumEdges(void)
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

unsigned int
HyperGraph::HyperGraphGetCellIndex(void *cellPtr)
{
  unsigned int subjectIdx;

  _ASSERT("Unable to find cell in design database", (cellPtr == NIL(void*)));

  subjectIdx = obj2idx[cellPtr];
  
  return (subjectIdx);
}

bool
HyperGraph::HyperGraphClusterNodes(vector<unsigned int> nodesList,
				   void *object, 
				   vector<void*>& affectedNets)
{
  bool success;
  
  success &= clusterNodes(nodesList, object, affectedNets);

  return success;
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
  vectorRemoveDuplicates(connectedNodeIndices);
  VECTOR_FOR_ALL_ELEMS(connectedNodeIndices, unsigned int, resultIdx) {
    _ASSERT("Subject node index matches with the subject node index", 
	    (resultIdx == subjectIdx));
    resultNode = idx2Node[resultIdx];
    rtvCells.push_back((*resultNode).NodeGetData());
  } END_FOR;

  return (rtvCells);
}

unsigned int 
HyperGraph::GetNumEdgesOfNode(void *object) 
{
  unsigned int nodeIdx;
  unsigned int numEdges;
  
  nodeIdx = obj2idx[object];
  numEdges = (Nodes2Edges[nodeIdx]).size();
 
  return (numEdges);
}

unsigned int 
HyperGraph::GetNumPairsOfNode(void *object, unsigned int& weightedSum) 
{
  unsigned int nodeIdx1, nodeIdx2;
  unsigned int numPairs;
  unsigned int edgeIdx;
  
  nodeIdx1 = obj2idx[object];
  numPairs = 0;

  vector<unsigned int>& edgeList = Nodes2Edges[nodeIdx1];
  weightedSum = 0;
  VECTOR_FOR_ALL_ELEMS(edgeList, unsigned int, edgeIdx) {
    map<unsigned int, unsigned int>& nodeMap = Edges2Nodes[edgeIdx];
    unsigned int thisEdgeWeight = GetEdgeWeight(edgeIdx);
    unsigned int thisEdgeNumPairs = 0;
    MAP_FOR_ALL_KEYS(nodeMap, unsigned int, unsigned int, nodeIdx2) {
      if (nodeIdx1 == nodeIdx2) {
	continue;
      }
      thisEdgeNumPairs++;
      numPairs++;
    } END_FOR;
    weightedSum += thisEdgeWeight * thisEdgeNumPairs;
  } END_FOR;
  
  return (numPairs);
}

vector<unsigned int>
HyperGraph::GetEdgesOfNode(void *object)
{
  unsigned int nodeIdx;
  vector<unsigned int> rtv;

  nodeIdx = obj2idx[object];
  rtv = Nodes2Edges[nodeIdx];
  
  return (rtv);
}

vector<unsigned int>
HyperGraph::GetEdgesOfNodes(void *object1, void *object2)
{
  unsigned int nodeIdx1, nodeIdx2;
  unsigned int edgeIdx;
  vector<unsigned int> Edges;
  vector<unsigned int> rtv;

  nodeIdx1 = obj2idx[object1];
  nodeIdx2 = obj2idx[object2];
  Edges = Nodes2Edges[nodeIdx1];

  VECTOR_FOR_ALL_ELEMS(Edges, unsigned int, edgeIdx) {
    map<unsigned int, unsigned int>& edges2NodesMap = Edges2Nodes[edgeIdx];
    if (edges2NodesMap.find(nodeIdx2) != edges2NodesMap.end()) {
      rtv.push_back(edgeIdx);
    }
  } END_FOR;

  return (rtv);
}

unsigned int 
HyperGraph::GetNumPairsOfNode(void *object) 
{
  unsigned int nodeIdx1, nodeIdx2;
  unsigned int numPairs;
  unsigned int edgeIdx;
  
  nodeIdx1 = obj2idx[object];
  numPairs = 0;
  vector<unsigned int>& edgeList = Nodes2Edges[nodeIdx1];

  VECTOR_FOR_ALL_ELEMS(edgeList, unsigned int, edgeIdx) {
    map<unsigned int, unsigned int>& nodeMap = Edges2Nodes[edgeIdx];
    MAP_FOR_ALL_KEYS(nodeMap, unsigned int, unsigned int, nodeIdx2) {
      if (nodeIdx1 == nodeIdx2) {
	continue;
      }
      numPairs++;
    } END_FOR;
  } END_FOR;
 
  return (numPairs);
}

unsigned int 
HyperGraph::GetNumEdgesOfNodes(void *object1, void *object2, unsigned int& weightedSum) 
{
  unsigned int nodeIdx1, nodeIdx2;
  unsigned int numEdges, edgeIdx;
  
  nodeIdx1 = obj2idx[object1];
  nodeIdx2 = obj2idx[object2];

  vector<unsigned int>& edgeList = Nodes2Edges[nodeIdx1];
  
  numEdges = 0;
  weightedSum = 0;
  VECTOR_FOR_ALL_ELEMS(edgeList, unsigned int, edgeIdx) {
    unsigned int thisEdgeWeight = GetEdgeWeight(edgeIdx);
    unsigned int thisEdgeNumPairs = 0;
    if ((Edges2Nodes[edgeIdx]).find(nodeIdx2) != (Edges2Nodes[edgeIdx]).end()) {
      numEdges++;
      weightedSum += thisEdgeWeight; 
    }
  } END_FOR;

  return (numEdges);
}


unsigned int 
HyperGraph::GetNumEdgesOfNodes(void *object1, void *object2) 
{
  unsigned int nodeIdx1, nodeIdx2;
  unsigned int numEdges, edgeIdx;
  
  nodeIdx1 = obj2idx[object1];
  nodeIdx2 = obj2idx[object2];

  vector<unsigned int>& edgeList = Nodes2Edges[nodeIdx1];
  
  numEdges = 0;
  VECTOR_FOR_ALL_ELEMS(edgeList, unsigned int, edgeIdx) {
    if ((Edges2Nodes[edgeIdx]).find(nodeIdx2) != (Edges2Nodes[edgeIdx]).end()) {
      numEdges++;
    }
  } END_FOR;

  return (numEdges);
}

/* 
   Returns the total weight of all edges connecting nodes 
   corresponding to cellPtr1 and cellPtr2. If there 
   is no connection, the returned value is -1

   if cellPtr1 has k edges and each edge on an average 
   has p nodes, complexity of this function would be
   (klogk)*(logp) = O(a) (some constant a as k and p 
   have upper limits)
*/
long int 
HyperGraph::HyperGraphNodesAreAdjacent(void* cellPtr1, void* cellPtr2) 
{
  unsigned int nodeIdx1, nodeIdx2, edgeIdx;
  vector<unsigned int> connectedEdges;
  long int returnEdgeIdx = -1;
  
  nodeIdx1 = obj2idx[cellPtr1]; /* O(logn) */
  nodeIdx2 = obj2idx[cellPtr2]; /* O(logn) */
  
  connectedEdges = Nodes2Edges[nodeIdx1];
  
  VECTOR_FOR_ALL_ELEMS(connectedEdges, unsigned int, edgeIdx) {
    if ((Edges2Nodes[edgeIdx]).find(nodeIdx2) != (Edges2Nodes[edgeIdx]).end()) {
      returnEdgeIdx = edgeIdx;
      break;
    }
  } END_FOR;
  
  return (returnEdgeIdx);
}


/* 
   FOR a cell C1, returns ΣFor all nets (Σ (W(n)*(Number of pins of cells other than C1 on each net)))
*/
double
HyperGraph::HyperGraphGetWeightedSumOfEdges(void* cellPtr, bool pinOffset, int xposypos)
{
  double weightedSum;
  double offsetDiffSum;
  unsigned int weight;
  unsigned int edgeIdx;
  unsigned int nodeIdx;
  unsigned int connectedNodeIdx;

  weightedSum = 0.0;
  offsetDiffSum = 0.0;
  nodeIdx = HyperGraphGetCellIndex(cellPtr);
  HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT((*this), cellPtr, edgeIdx, weight) {
    unsigned int numConnected;
    map<unsigned int, unsigned int>& nodesOfEdge = getEdgeConnectivity(edgeIdx);
    numConnected = 0;
    Cell& cellP = *(Cell*)(cellPtr);
    MAP_FOR_ALL_KEYS(nodesOfEdge, unsigned int, unsigned int, connectedNodeIdx) {
      if (connectedNodeIdx == nodeIdx) {
	continue;
      }
      numConnected++;
      if (pinOffset == true) {
	Node& thisNode = *(*this).idx2Node[nodeIdx];
	Cell& cellQ = *(Cell *)thisNode.NodeGetData();
	if (cellP.CellIsPort()) {
	  if (xposypos == 0) {
	    offsetDiffSum -= cellP.CellGetXpos();
	  } else {
	    offsetDiffSum -= cellP.CellGetYpos();
	  }
	} 
	if (cellQ.CellIsPort()) {
	  if (xposypos == 0) {
	    offsetDiffSum -= cellQ.CellGetXpos();
	  } else {
	    offsetDiffSum -= cellQ.CellGetYpos();
	  }
	}
      }
    } END_FOR;
    if (pinOffset == false) {
      weightedSum += weight * numConnected;
    } else {
      weightedSum += weight * numConnected * offsetDiffSum;
    }
  } HYPERGRAPH_END_FOR;

  return (weightedSum);
}

double
HyperGraph::HyperGraphGetWeightedSumOfEdges(void* cellPtr1, void *cellPtr2)
{
  double weightedSum;
  unsigned int weight;
  unsigned int edgeIdx;
  unsigned int nodeIdx1, nodeIdx2;
  unsigned int connectedNodeIdx;

  weightedSum = 0.0;
  nodeIdx1 = HyperGraphGetCellIndex(cellPtr1);
  nodeIdx2 = HyperGraphGetCellIndex(cellPtr2);

  HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT((*this), cellPtr1, edgeIdx, weight) {
    unsigned int numConnected;
    map<unsigned int, unsigned int>& nodesOfEdge = getEdgeConnectivity(edgeIdx);
    if (nodesOfEdge.find(nodeIdx2) == nodesOfEdge.end()) {
      continue;
    }
    weightedSum += weight;
  } HYPERGRAPH_END_FOR;

  return (weightedSum);
}

HyperGraph::~HyperGraph() 
{

}
