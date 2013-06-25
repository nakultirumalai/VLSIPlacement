# include <Design.h>

/*********************************************************************
   Top level function to perform default clustering 
   Iterate over the netlist and generate vector     
   of vector of cells and a cluster is created for  
   each vector of cells in the top level vector     
*********************************************************************/
bool
Design::DesignDoDefaultCluster(HyperGraph &myGraph)
{
  Design& myDesign = (*this);
  Cell *cellPtr, *thisCellPtr;
  vector<vector<void * > >cellsToCluster;
  string cellName;
  unsigned int numClustered;
  uint numClusters;
  bool rtv;

  _STEP_BEGIN("Begin default clustering");

  rtv = true;
  numClusters = 0;
  DESIGN_FOR_ALL_STD_CELLS(myDesign, cellName, cellPtr) {
    if ((*cellPtr).CellIsHidden()) continue;
    vector<Cell *> connectedCells = DesignGetConnectedCells(myGraph, cellPtr);
    if (connectedCells.empty()) {
      continue;
    }
    connectedCells.push_back(cellPtr);
    DesignClusterCells(connectedCells, /* collapseClustersInList */ true, 
		       /* placeBoundaryCells */ true);
    numClusters++;
    //    if (numClusters == 1000) break;
  } DESIGN_END_FOR;

  _STEP_END("End default clustering");

  return rtv;
}

/***************************************************
  TOP LEVEL FUNCTION TO DO TIMING DRIVEN CLUSTERING
***************************************************/
bool
Design::DesignDoTimingDrivenCluster(HyperGraph &myGraph)
{
  Path *thisPath;
  Cell *cellPtr;
  vector<Cell *> clusterList;
  uint pathIdx;

  _STEP_BEGIN("Path-based clustering");

  DESIGN_FOR_ALL_PATHS((*this), pathIdx, thisPath) {
    vector<Cell *> &pathCells = (*thisPath).PathGetCells();
    clusterList.clear();
    VECTOR_FOR_ALL_ELEMS(pathCells, Cell*, cellPtr) {
      if ((*cellPtr).CellIsHidden()) continue;
      if ((*cellPtr).CellIsSequential()) continue;
      clusterList.push_back(cellPtr);
    } END_FOR;
    if (clusterList.empty()) continue;
    DesignClusterCells(clusterList, /* collapseClustersInList */ true, 
		       /* placeBoundaryCells */ true);
  } DESIGN_END_FOR;

  _STEP_END("Path-based clustering");
}

/* Supporting function for best choice clustering */
/* Computes the score between two cells by using the hypergraph */
double
computeScore(HyperGraph &myGraph, uint nodeIdx1, uint nodeIdx2)
{
  Cell *cellObj1, *cellObj2;
  double score, totalEdgeWeight, edgeWeight;
  double areaCell1, areaCell2;
  uint edgeIdx;
  
  totalEdgeWeight = 0;
  cellObj1 = (Cell *)myGraph.GetNodeObject(nodeIdx1);
  cellObj2 = (Cell *)myGraph.GetNodeObject(nodeIdx2);
  HYPERGRAPH_FOR_ALL_EDGES_OF_NODES(myGraph, nodeIdx1, nodeIdx2, edgeIdx, 
				    edgeWeight) {
    totalEdgeWeight += edgeWeight;
  } HYPERGRAPH_END_FOR;
  areaCell1 = (*cellObj1).CellGetArea() / (GRID_COMPACTION_RATIO ^ 2);
  areaCell2 = (*cellObj2).CellGetArea() / (GRID_COMPACTION_RATIO ^ 2);
  score = totalEdgeWeight / (areaCell1 + areaCell2);
  
  return (score);
}

/* Get the best neighbour of a node u from the hypergraph */
bool
getBestNeighbor(HyperGraph &myGraph, uint &nodeIdx1, uint &nodeIdx2, double &score,
		map<uint, bool> &visitedLeftNodes)
{
  map<uint, uint> scoreVectIdxMap;
  map<uint, uint>::iterator itr;
  map<uint, bool> visitedNodes;
  vector<pair<uint, double> > scoreVect;
  Cell *cellObj1, *cellObj2;
  void *nodeObj1, *nodeObj2, *nodeObj;
  double areaCellObj1, areaCellObj2;
  double edgeWeight;
  uint edgeIdx;
  bool rtv, scoreComputed;
  
  nodeIdx2 = 0;
  score = 0.0;
  rtv = false;
  scoreComputed = false;
  cellObj1 = (Cell *)myGraph.GetNodeObject(nodeIdx1);
  areaCellObj1 = (*cellObj1).CellGetArea();
  HYPERGRAPH_FOR_ALL_EDGES_OF_NODE(myGraph, nodeIdx1, edgeIdx, edgeWeight) {
    rtv = true;
    if (myGraph.EdgeGetNumNodes(edgeIdx) > HIGH_FANOUT) {
      continue;
    }
    visitedNodes[nodeIdx1] = true;
    HYPERGRAPH_FOR_ALL_NODES_OF_EDGE(myGraph, edgeIdx, nodeIdx2) {
      if (nodeIdx1 == nodeIdx2) {
	continue;
      }
      _KEY_EXISTS(visitedNodes, nodeIdx2) continue;
      cellObj2 = (Cell *)myGraph.GetNodeObject(nodeIdx2);
      if ((*cellObj2).CellIsTerminal()) continue;
      areaCellObj2 = (*cellObj2).CellGetArea();
      _KEY_EXISTS_WITH_VAL(scoreVectIdxMap, nodeIdx2, itr) {
	score = edgeWeight / (areaCellObj1 + areaCellObj2);
	(scoreVect[(itr->second)]).second += score;
      } else {
	score = edgeWeight / (areaCellObj1 + areaCellObj2);
	scoreVectIdxMap[nodeIdx2] = scoreVect.size();
	scoreVect.push_back(make_pair(nodeIdx2, score));
      }
    } HYPERGRAPH_END_FOR;
  } HYPERGRAPH_END_FOR;
  if (rtv == false) {
    _ASSERT_TRUE("Error: (getBestNeighbor): No edges of node");
  }

  if (!scoreVect.empty()) {
    scoreCmpTypeUint cmpObjType;
    sort(scoreVect.begin(), scoreVect.end(), cmpObjType);
    nodeIdx2 = (scoreVect[0]).first;
    score = (scoreVect[0]).second;
    scoreComputed = true;
  }

  return (scoreComputed);
}

/* Another variation of the above function */
bool
getBestNeighbor(HyperGraph &myGraph, uint &nodeIdx1, uint &nodeIdx2, double &score)
{
  bool result;
  map<uint, bool> visitedLeftNodes;  
  
  result = getBestNeighbor(myGraph, nodeIdx1, nodeIdx2, score, visitedLeftNodes);
  
  return (result);
}

/* Builds the priority queue for best choice clustering */
priority_queue<objPairScore<uint>, vector<objPairScore<uint> >, objPairScoreCompareUint>
buildPriorityQueue(HyperGraph &myGraph)
{
  Cell *cellObj1, *cellObj2;
  void *nodeObj1, *nodeObj2;
  map<uint, bool> visitedLeftNodes;
  double edgeWeight, score;
  double areaCell1, areaCell2;
  uint nodeIdx1, nodeIdx2, edgeIdx;
  bool result;

  /* Create a priority queue */
  priority_queue<objPairScore<uint>, vector<objPairScore<uint> >, objPairScoreCompareUint> pqueue;
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx1, nodeObj1) {
    visitedLeftNodes[nodeIdx1] = 1;
    cellObj1 = (Cell *)nodeObj1;
    areaCell1 = (*cellObj1).CellGetArea();
    if ((*cellObj1).CellIsTerminal()) continue;
    result = getBestNeighbor(myGraph, nodeIdx1, nodeIdx2, score, visitedLeftNodes);
    if (!result) continue;
    objPairScore<uint> thisPair(nodeIdx1, nodeIdx2, score);
    pqueue.push(thisPair);
  } HYPERGRAPH_END_FOR;
  
  return (pqueue);
}

/***************************************************
  TOP LEVEL FUNCTION TO DO BEST CHOICE CLUSTERING
***************************************************/
bool
Design::DesignDoBestChoiceClustering(HyperGraph &myGraph)
{
  Cell *clusteredCell, *cellObj1, *cellObj2;
  void *nodeObj1, *nodeObj2;
  vector<void *> connectedObjs;
  map<uint, bool> visitedLeftNodes;
  double edgeWeight, score, newScore;
  double areaCell1, areaCell2;
  uint nodeIdx1, nodeIdx2, edgeIdx;
  uint numNonTopNodes, numTopNodes;
  uint numOrigTopNodes, numClusters;
  uint edgeDegree, allNodes;
  bool result;

  _STEP_BEGIN("Best choice clustering");
  /* Compute the number of top nodes */
  numTopNodes = DesignGetNumTopCells();
  numOrigTopNodes = numTopNodes;
  numClusters = 0;
 
  /* Get the priority queue */
  priority_queue<objPairScore<uint>, vector<objPairScore<uint> >, objPairScoreCompareUint> pqueue;
  pqueue = buildPriorityQueue(myGraph);

  //  cout << "BEGIN CLUSTERING NUMTOPNODES: " << numTopNodes << " " << getCPUTime() << "   " << getMemUsage() << endl;
  while (numTopNodes > (0.3 * ((double)numOrigTopNodes))) {
    /* Get the top pair u,v, score */
    objPairScore<uint> topPair = pqueue.top(); pqueue.pop();
    topPair.GetObjs(nodeIdx1, nodeIdx2);
    score = topPair.GetScore();
    if (!myGraph.NodeIsTop(nodeIdx1) || !myGraph.NodeIsTop(nodeIdx2)) {
      continue;
    }

    /* If u is invalid recompute the best neighbor for u */
    if (myGraph.NodeIsInvalid(nodeIdx1)) {
      if (getBestNeighbor(myGraph, nodeIdx1, nodeIdx2, score)) {
	myGraph.NodeClearIsInvalid(nodeIdx1);
	myGraph.NodeClearIsInvalid(nodeIdx2);
	objPairScore<uint> thisPair(nodeIdx1, nodeIdx2, score);
	pqueue.push(thisPair);
      }
      continue;
    }

    //    cout << "SCORE COMPUTATION DONE: " << getCPUTime() << "   " << getMemUsage() << endl;    
    vector<Cell *> cellVect;
    cellObj1 = (Cell *)myGraph.GetNodeObject(nodeIdx1);
    cellObj2 = (Cell *)myGraph.GetNodeObject(nodeIdx2);
    cellVect.push_back(cellObj1);
    cellVect.push_back(cellObj2);

    /* Create the cluster */
    //    cout << "CLUSTER: Clustered cells " << (*cellObj1).CellGetName() << "  " << (*cellObj2).CellGetName() << endl;
    clusteredCell = DesignClusterCellsSimple(cellVect);
    //    cout << "CLUSTER FORMATION DONE: " << getCPUTime() << "   " << getMemUsage() << endl;    
    numClusters++;

    /* Find the closest neighbor of u', v' and insert the pair u', v' into the priority queue */
    nodeIdx1 = myGraph.GetObjIndex((void *)clusteredCell);
    result = getBestNeighbor(myGraph, nodeIdx1, nodeIdx2, score, visitedLeftNodes);
    
    /* Mark all the neighbors of u' as invalid */
    MAP_FOR_ALL_KEYS(visitedLeftNodes, uint, bool, nodeIdx1) {
      myGraph.NodeSetIsInvalid(nodeIdx1);
    } END_FOR;
    visitedLeftNodes.clear();
    if (!result) continue;
    //    cout << "IDENTIFYING NEIGHBOURING OBJECTS DONE: " << getCPUTime() << "   " << getMemUsage() << endl;

    /* Push the new pair u', v', score into the priority queue */
    objPairScore<uint> newPair(nodeIdx1, nodeIdx2, score);
    pqueue.push(newPair);
    //    cout << "PUSHING PAIR AND SORTING OBJECTS DONE: " << getCPUTime() << "   " << getMemUsage() << endl;    

    /* Find out how many top nodes are left */
    //    allNodes = DesignGetNumCells() + DesignGetNumClusters();
    //    numNonTopNodes = myGraph.GetNumNonTopNodes();
    numTopNodes = DesignGetNumTopCells();
    //    cout << "END, NUMTOPNODES: " << numTopNodes << " POP: " << getCPUTime() << "   " << getMemUsage() << endl;
    //    if (numClusters == 2000) {
    //      break;
    //    }
  }
  //  cout << "CLUSTERING DONE" << endl;
  //  cout << "END CLUSTERING NUMTOPNODES: " << numTopNodes << " " << getCPUTime() << "   " << getMemUsage() << endl;
  //  cout << "COMMITTING CLUSTERS " << getCPUTime() << "   " << getMemUsage() << endl;
  Cell *clusterCell;
  string clusterName;
  DESIGN_FOR_ALL_CLUSTERS((*this), clusterName, clusterCell) {
    DesignCommitCluster(clusterCell);
  } END_FOR;
  //  cout << "END COMMITTING CLUSTERS " << getCPUTime() << "   " << getMemUsage() << endl;
  //  cout << "CONDITION MET: Number of top nodes: " << numTopNodes << endl;
  _STEP_END("Best choice clustering");
}

/***************************************************
  TOP LEVEL FUNCTION TO DO FIRST CHOICE CLUSTERING
***************************************************/
bool
Design::DesignDoFirstChoiceClustering(HyperGraph &myGraph)
{
  
}

/***************************************************
  FUNCTION TO TEST THE CLUSTER CREATION AND 
  DESTRUCTION AND PLACEMENT OF CELLS AROUND
***************************************************/
bool
Design::DesignDoClusterTest()
{
  vector<Cell *> listOfCells;
  Cell *clusterCell;
  Cell *cell1 = DesignGetNode("o90");  listOfCells.push_back(cell1);
  Cell *cell2 = DesignGetNode("o89");  listOfCells.push_back(cell2);
  Cell *cell3 = DesignGetNode("o2138");  listOfCells.push_back(cell3);
  Cell *cell4 = DesignGetNode("o1778");   listOfCells.push_back(cell4);
  Cell *cell5 = DesignGetNode("o91");   listOfCells.push_back(cell5);
  Cell *cell6 = DesignGetNode("o0"); listOfCells.push_back(cell6);
  Cell *cell7 = DesignGetNode("o1986"); listOfCells.push_back(cell7);
  Cell *cell8 = DesignGetNode("o1987"); listOfCells.push_back(cell8);
  Cell *cell9 = DesignGetNode("o92"); listOfCells.push_back(cell9);

  DesignClusterCells(listOfCells, /* collapseClustersInList */ true, 
		     /* placeBoundaryCells */ true);
  
  clusterCell = DesignGetNode("c1");
  
  (*clusterCell).CellSetXpos(100000);
  (*clusterCell).CellSetYpos(100000);

  /* Dissolve the cluster */
  DesignUnclusterCell(clusterCell, false);
}
