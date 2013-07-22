# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <Stat.h>
# include <time.h>
# include <Cell.h>
# include <stdlib.h>

uint 
HyperGraph::AddNodeInt(void *object)
{
  uint nodeIdx;
  map<void *, uint>::iterator itr;

  _KEY_EXISTS_WITH_VAL(obj2idx, object, itr) {
    nodeIdx = itr->second;
  } else {
    Node *newNode = new Node();
    nodeIdx = numNodes++;
    (*newNode).NodeSetIdx(nodeIdx);
    /* Create link in the map */
    idx2Node.push_back(newNode);
    /* Create an entry in the connectivity table 
       for this object */
    Nodes2Edges.push_back(vector<uint> ());
    NodeSetIsTop(nodeIdx);
    (*newNode).NodeSetData(object);
    obj2idx[object] = nodeIdx;
  }
  return (nodeIdx);
}

uint 
HyperGraph::AddEdgeInt(void *object, double weight)
{
  uint edgeIdx;

  Edge *newEdge = new Edge(object);
  edgeIdx = numEdges++;
  (*newEdge).EdgeSetIdx(edgeIdx);
  (*newEdge).EdgeSetWeight(weight);

  /* Create link in the map */
  idx2Edge.push_back(newEdge);
  
  return (edgeIdx);
}

void
HyperGraph::HideEdge(uint edgeIdx)
{
  Edge *thisEdge;
  
  thisEdge = idx2Edge[edgeIdx];
  (*thisEdge).EdgeSetIsHidden(true);
}

uint 
HyperGraph::RemoveEdge(uint edgeIdx)
{
  void *nodeObj;
  uint nodeIdx;
  uint idx;
  bool edgeFound;

  HideEdge(edgeIdx);
}

uint 
HyperGraph::RemoveNode(uint nodeIdx)
{
  
}

vector<uint> 
HyperGraph::GetConnectedNodeIndices(uint nodeIdx)
{
  vector<uint> returnNodes;
  vector<uint> Edges;
  map<uint, uint> EdgeNodeList;
  uint connectedEdgeIdx;
  uint connectedNodeIdx;
  
  /* Find the vector of edges that nodeIdx has connections to */
  Edges = (this->Nodes2Edges)[nodeIdx];
  /* Iterate over all the edges */
  VECTOR_FOR_ALL_ELEMS(Edges, uint, connectedEdgeIdx) {
    /* Get the list of nodes that are connected to edge index 
       connectedEdgeIdx */
    EdgeNodeList = (this->Edges2Nodes)[connectedEdgeIdx];
    /* Iterate over all keys of the EdgeNodeList map 
       as this would give the list of nodes that are 
       connected to the edge connectedEdgeIdx */
    MAP_FOR_ALL_KEYS(EdgeNodeList, uint, uint, connectedNodeIdx) {
      if (!NodeIsTop(connectedNodeIdx)) {
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

/* Function to check, set and clear if a node index 
   represents a top node of a cluster */
void 
HyperGraph::NodeSetIsTop(uint nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeSetIsTop();
}

void 
HyperGraph::NodeClearIsTop(uint nodeIdx)
{
  Node *node;
  
  node = idx2Node[nodeIdx];
  (*node).NodeClearIsTop();
  
  _KEY_DOES_NOT_EXIST(nonTopNodes, nodeIdx) {
    nonTopNodes[nodeIdx] = 1;
  }
}

/*************************************************
  PUBLIC FUNCTIONS BEGIN
**************************************************/
void 
HyperGraph::AddNode(void *object)
{
  AddNodeInt(object);
}

void* 
HyperGraph::GetNodeObject(uint nodeIdx) 
{
  Node *thisNode;
  void *nodeObject;
  
  thisNode = idx2Node[nodeIdx];
  nodeObject = (*thisNode).NodeGetData();
  
  return (nodeObject);
}

int
HyperGraph::GetObjIndex(void *nodeObj) 
{
  int rtv;
  map<void *, uint>::iterator itr;

  rtv = -1;
  _KEY_EXISTS_WITH_VAL(obj2idx, nodeObj, itr) {
    rtv = itr->second;
  }
  
  return rtv;
}

bool
HyperGraph::NodeIsTop(uint nodeIdx)
{
  Node *node;
  bool result;

  node = idx2Node[nodeIdx];
  result = (*node).NodeIsTop();
  
  return (result);
}

void 
HyperGraph::NodeSetIsInvalid(uint nodeIdx)
{
  Node *node;

  node = idx2Node[nodeIdx];
  (*node).NodeSetIsInvalid();
}

void 
HyperGraph::NodeClearIsInvalid(uint nodeIdx)
{
  Node *node;

  node = idx2Node[nodeIdx];
  (*node).NodeClearIsInvalid();
}

bool 
HyperGraph::NodeIsInvalid(uint nodeIdx)
{
  Node *node;
  bool result;

  node = idx2Node[nodeIdx];
  result = (*node).NodeIsInvalid();
  
  return (result);
}

uint
HyperGraph::AddEdge(vector<void *> &cellList, void *EdgeObject, 
		    double weight)
{
  vector<uint> nodeIdxArray;
  vector<uint> nodeIdxArrayCopy;
  Node *nodePtr;
  Edge *edgePtr;
  uint nodeIdx, nodeIdxCopy;
  uint edgeIdx;
  void *object;

  edgeIdx = AddEdgeInt(EdgeObject, weight);
  edgePtr = idx2Edge[edgeIdx];
  Edges2Nodes.push_back(map<uint, uint> ());
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
  
  return edgeIdx;
}

double 
HyperGraph::GetEdgeWeight(uint edgeIdx)
{
  Edge *thisEdge;
  double edgeWeight;
  
  thisEdge = idx2Edge[edgeIdx];
  if (thisEdge != NULL) {
    edgeWeight = (*thisEdge).EdgeGetWeight();
  }
  
  return edgeWeight;
}

void 
HyperGraph::SetEdgeWeight(uint edgeIdx, double newWeight)
{
  Edge *thisEdge;
  
  thisEdge = idx2Edge[edgeIdx];
  if (thisEdge != NULL) {
    (*thisEdge).EdgeSetWeight(newWeight);
  }
}

void* 
HyperGraph::GetEdgeObject(uint edgeIdx)
{
  Edge *thisEdge;
  void *edgeObject;
  
  thisEdge = idx2Edge[edgeIdx];
  if (thisEdge != NULL) {
    edgeObject = (*thisEdge).EdgeGetObject();
  }
  
  return edgeObject;
}

bool
HyperGraph::EdgeIsHidden(uint edgeIdx) 
{
  Edge *thisEdge;

  thisEdge = idx2Edge[edgeIdx];

  return ((*thisEdge).EdgeIsHidden());
}

bool
HyperGraph::ClusterNodes(vector<void *>& nodeSet, void* object,
			 map<void *, bool>& externalNets, 
			 map<void *, bool>& internalNets)
{
  void *nodePtr;
  void *edgeObject;
  vector<uint> nodeSetCopy;
  map<uint, bool> newNodeEdgeMap;
  vector<uint> newNodeEdgeList;
  uint nodeIdx, edgeIdx, newNodeIdx;
  uint idx;
  bool result;
  bool debug = false;

  result = true;
  VECTOR_FOR_ALL_ELEMS(nodeSet, void*, nodePtr) {
    nodeIdx = GetObjIndex(nodePtr);
    if (!NodeIsTop(nodeIdx)) {
      result = false;
      _ASSERT_TRUE("Error: Non-top node sent for input to cluster function");
    }
    nodeSetCopy.push_back(nodeIdx);
  } END_FOR;
  
  if (result == false) {
    return (result);
  }

  externalNets.clear();
  internalNets.clear();
  //  PrintSubGraph(nodeSetCopy);
  /* Add the new object and get the new edge index */
  newNodeIdx = AddNodeInt(object);
  map<uint, bool> visitedEdges;
  VECTOR_FOR_ALL_ELEMS(nodeSetCopy, uint, nodeIdx) {
    /* For each node in the node set, go over all the *unique* edges and remove the node
       index from the map corresponding to the edge index */
    VECTOR_FOR_ALL_ELEMS((Nodes2Edges[nodeIdx]), uint, edgeIdx) {
      _KEY_DOES_NOT_EXIST(visitedEdges, edgeIdx) {
	visitedEdges[edgeIdx] = 1;
	newNodeEdgeMap[edgeIdx] = 1;
	/* Add the new node to the list of edges */
	_KEY_DOES_NOT_EXIST((Edges2Nodes[edgeIdx]), newNodeIdx) {
	  (Edges2Nodes[edgeIdx])[newNodeIdx] = 1;
	}
      }

      /* Remove the node indices from the list of edge index */
      (Edges2Nodes[edgeIdx]).erase(nodeIdx);

      /* Hide the node index */
      NodeClearIsTop(nodeIdx);
    } END_FOR;
  } END_FOR;
  
  MAP_FOR_ALL_KEYS(visitedEdges, uint, bool, edgeIdx) {
    edgeObject = GetEdgeObject(edgeIdx);
    if ((Edges2Nodes[edgeIdx]).size() == 1) {
      RemoveEdge(edgeIdx);
      internalNets[edgeObject] = 1;
      newNodeEdgeMap.erase(edgeIdx);
    } else {
      externalNets[edgeObject] = 1;
    }
  } END_FOR;

  nodeSetCopy.push_back(newNodeIdx);
  NodeSetIsCluster(newNodeIdx, true);
  MAP_FOR_ALL_KEYS(newNodeEdgeMap, uint, bool, edgeIdx) {
    newNodeEdgeList.push_back(edgeIdx);
  } END_FOR;
  Nodes2Edges[newNodeIdx] = newNodeEdgeList;
  NodeSetIsTop(newNodeIdx);
  //  PrintSubGraph(nodeSetCopy);

  return (result);
}

  /*
    if (NodeIsCluster(nodeIdx)) {
      if ((*this).eraseClusters == true) {
	numEdges = Edges2Nodes.size();
	uint origNodeIdx, keyVal;
	for (uint idx = 0; idx < numEdges; idx++) {
	  map<uint, uint> nodeMapCopy = Edges2Nodes[idx];
	  map<uint, uint> modNodeMap;
	  MAP_FOR_ALL_ELEMS(nodeMapCopy, uint, uint, origNodeIdx, keyVal) {
	    if (origNodeIdx > nodeIdx) {
	      modNodeMap[(origNodeIdx - 1)] = 1;
	    } else {
	      modNodeMap[origNodeIdx] = 1;
	    }
	  } END_FOR;
	  Edges2Nodes[idx] = modNodeMap;
	}
	Nodes2Edges.erase(Nodes2Edges.begin() + nodeIdx);
      }
    }
  */

uint 
HyperGraph::GetNumNodes(void)
{
  return (numNodes);
}

uint 
HyperGraph::GetNumEdges(void)
{
  return (numEdges);
}

uint 
HyperGraph::EdgeGetNumNodes(uint edgeIdx)
{
  return ((Edges2Nodes[edgeIdx]).size());
}

uint 
HyperGraph::NodeGetNumEdges(uint nodeIdx)
{
  return ((Nodes2Edges[nodeIdx]).size());
}

map<uint, uint>
HyperGraph::GetNodesOfEdge(uint edgeIdx)
{
  return (Edges2Nodes[edgeIdx]);
}

vector<uint>
HyperGraph::GetEdgesOfObject(void *object)
{
  uint nodeIdx;
  vector<uint> rtv;

  nodeIdx = obj2idx[object];
  rtv = Nodes2Edges[nodeIdx];
  
  return (rtv);
}

vector<uint>
HyperGraph::GetEdgesOfNode(uint nodeIdx)
{
  vector<uint> rtv;

  rtv = Nodes2Edges[nodeIdx];
  
  return (rtv);
}

vector<uint>
HyperGraph::GetEdgesOfObjects(void *object1, void *object2)
{
  uint nodeIdx1, nodeIdx2;
  uint edgeIdx;
  vector<uint> Edges;
  vector<uint> rtv;

  nodeIdx1 = obj2idx[object1];
  nodeIdx2 = obj2idx[object2];
  Edges = Nodes2Edges[nodeIdx1];

  VECTOR_FOR_ALL_ELEMS(Edges, uint, edgeIdx) {
    map<uint, uint>& edges2NodesMap = Edges2Nodes[edgeIdx];
    if (edges2NodesMap.find(nodeIdx2) != edges2NodesMap.end()) {
      rtv.push_back(edgeIdx);
    }
  } END_FOR;

  return (rtv);
}

vector<uint>
HyperGraph::GetEdgesOfNodes(uint nodeIdx1, uint nodeIdx2)
{
  uint edgeIdx;
  vector<uint> Edges;
  vector<uint> rtv;

  Edges = Nodes2Edges[nodeIdx1];
  VECTOR_FOR_ALL_ELEMS(Edges, uint, edgeIdx) {
    map<uint, uint>& edges2NodesMap = Edges2Nodes[edgeIdx];
    if (edges2NodesMap.find(nodeIdx2) != edges2NodesMap.end()) {
      rtv.push_back(edgeIdx);
    }
  } END_FOR;

  return (rtv);
}

vector<void *> 
HyperGraph::GetConnectedNodes(void* nodePtr)
{
  uint subjectIdx;
  uint resultIdx;
  vector<uint> connectedNodeIndices;
  vector<void *> rtvCells;
  Node *resultNode;
  
  if (nodePtr == NIL(void *)) {
    _ASSERT_TRUE("Error: Cell of whose connected nodes have to be found is NULL!");
  }
  subjectIdx = obj2idx[nodePtr];
  connectedNodeIndices = GetConnectedNodeIndices(subjectIdx);
  vectorRemoveDuplicates(connectedNodeIndices);
  VECTOR_FOR_ALL_ELEMS(connectedNodeIndices, uint, resultIdx) {
    _ASSERT("Subject node index matches with the subject node index", 
	    (resultIdx == subjectIdx));
    resultNode = idx2Node[resultIdx];
    rtvCells.push_back((*resultNode).NodeGetData());
  } END_FOR;

  return (rtvCells);
}

/*** TEST FUNCTIONS ***/
void
HyperGraph::testClustering(void)
{
  vector<vector<uint > > toBeClustered;
  vector<uint> listOfNodes;
  uint nodeIdx;

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

  //HyperGraphUnclusterNodes(listOfNodes);
  listOfNodes.clear();
}

long int 
HyperGraph::NodesAreAdjacent(void* cellPtr1, void* cellPtr2) 
{
  uint nodeIdx1, nodeIdx2, edgeIdx;
  vector<uint> connectedEdges;
  long int returnEdgeIdx = -1;
  
  nodeIdx1 = obj2idx[cellPtr1]; /* O(logn) */
  nodeIdx2 = obj2idx[cellPtr2]; /* O(logn) */
  
  connectedEdges = Nodes2Edges[nodeIdx1];
  
  VECTOR_FOR_ALL_ELEMS(connectedEdges, uint, edgeIdx) {
    if ((Edges2Nodes[edgeIdx]).find(nodeIdx2) != (Edges2Nodes[edgeIdx]).end()) {
      returnEdgeIdx = edgeIdx;
      break;
    }
  } END_FOR;
  
  return (returnEdgeIdx);
}

void* 
HyperGraph::GetOtherNodeOfEdge(uint edgeIdx, void *obj)
{
  map<uint, uint> &nodeMap = Edges2Nodes[edgeIdx];
  uint thisNodeIdx, otherNodeIdx;
  void *rtv;
  
  thisNodeIdx = obj2idx[obj];
  MAP_FOR_ALL_KEYS(nodeMap, uint, uint, otherNodeIdx) {
    if (otherNodeIdx == thisNodeIdx) continue;
    break;
  } END_FOR;
  
  rtv = GetNodeObject(otherNodeIdx);
  
  return (rtv);
}

bool
HyperGraph::NodeIsCluster(uint nodeIdx)
{
  Node *nodeObj;
  bool rtv;
  
  nodeObj = idx2Node[nodeIdx];
  rtv = (*nodeObj).NodeIsCluster();
  
  return (rtv);
}

void
HyperGraph::NodeSetIsCluster(uint nodeIdx, bool isCluster)
{
  Node *nodeObj;
  
  nodeObj = idx2Node[nodeIdx];
  (*nodeObj).NodeSetIsCluster(isCluster);
}

void 
HyperGraph::PrintGraph(void)
{
  uint nodeIdx, count;
  uint edgeIdx, numEdges;

  /* First print the hypergraph details */
  cout << "***********************************************************" << endl;
  cout << "  Number of nodes in the hypergraph: " << GetNumNodes() << endl;
  cout << "  Number of edges in the hypergraph: " << GetNumEdges() << endl;
  cout << "***********************************************************" << endl;

  /* Print the edges */
  numEdges = GetNumEdges();
  for (edgeIdx = 0; edgeIdx < numEdges; edgeIdx++) {
    cout << "Edge Index: " << endl;
    cout << "Nodes: " << endl;
    count = 0;
    MAP_FOR_ALL_KEYS((Edges2Nodes[edgeIdx]), uint, uint, nodeIdx) {
      cout << nodeIdx << "    ";
      if (count > 0 && count % 4 == 0) {
	cout << endl;
      }
      count++;
    } END_FOR;
    cout << endl;
  }
}

void 
HyperGraph::PrintSubGraph(vector<uint>& nodeSet) 
{
  uint edgeIdx;
  double edgeWeight;
  void *nodeObj;
  uint nodeIdx;

  VECTOR_FOR_ALL_ELEMS(nodeSet, uint, nodeIdx) {
    PrintNode(nodeIdx);
    cout << endl;
    nodeObj = GetNodeObject(nodeIdx);
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT((*this), nodeObj, edgeIdx, edgeWeight) {
      PrintEdge(edgeIdx);
    } HYPERGRAPH_END_FOR;
    cout << endl;
  } END_FOR;
}

void 
HyperGraph::PrintEdge(uint edgeIdx)
{
  void *nodeObj;
  void *netObj;
  uint nodeIdx;
  uint maxPrintCount = 20;
  uint count = 0;
  netObj = GetEdgeObject(edgeIdx);
  cout << "Edge: " << edgeIdx << " " << (*((Net*)netObj)).NetGetName() << endl;
  cout << "Nodes: ";
  HYPERGRAPH_FOR_ALL_NODES_OF_EDGE((*this), edgeIdx, nodeIdx) {
    if (count > maxPrintCount) break;
    nodeObj = GetNodeObject(nodeIdx);
    cout << nodeIdx << "(" << (*((Cell *)nodeObj)).CellGetName() << ")  ";
    count++;
  } HYPERGRAPH_END_FOR;
  cout << endl;
}

void 
HyperGraph::PrintNode(uint nodeIdx)
{
  uint edgeIdx;
  double edgeWeight;
  void *nodeObj;
  void *netObj;

  nodeObj = GetNodeObject(nodeIdx);
  cout << "Node: " << nodeIdx << "(" << (*((Cell *)nodeObj)).CellGetName() << ")  " << endl;
  cout << "Edges: ";
  
  HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT((*this), nodeObj, edgeIdx, edgeWeight) {
    netObj = GetEdgeObject(edgeIdx);
    cout << edgeIdx << "(" << (*((Net*)netObj)).NetGetName() << ")  ";
  } HYPERGRAPH_END_FOR;
  cout << endl;
}


uint
HyperGraph::GetNumNonTopNodes(void)
{
  return (nonTopNodes.size());
}

HyperGraph::HyperGraph() 
{
  numEdges = 0;
  numNodes = 0;
  eraseClusters = false;
}

HyperGraph::~HyperGraph() 
{
  Node *thisNode;
  Edge *thisEdge;
  uint idx;

  VECTOR_FOR_ALL_ELEMS(idx2Node, Node*, thisNode) {
    delete thisNode;
  } END_FOR;

  VECTOR_FOR_ALL_ELEMS(idx2Edge, Edge*, thisEdge) {
    delete thisEdge;
  } END_FOR;
}

Edge* 
HyperGraph::GetEdge(uint edgeIdx)
{
  return (idx2Edge[edgeIdx]);
}

Node* 
HyperGraph::GetNode(uint nodeIdx)
{
  return (idx2Node[nodeIdx]);
}
