# include <Design.h>

/* Supporting function for best choice clustering */
/* Get the best neighbour of a node u from the hypergraph */
inline
bool getBestNeighborCstr(HyperGraph &myGraph, uint &nodeIdx1, uint &nodeIdx2, 
			 double &score, map<uint, bool> &visitedLeftNodes, 
			 double maxArea, double maxWidth, bool widthCost)
{
  map<uint, uint> scoreVectIdxMap;
  map<uint, uint>::iterator itr;
  map<uint, bool> visitedNodes;
  vector<pair<uint, double> > scoreVect;
  Cell *cellObj1, *cellObj2;
  void *nodeObj1, *nodeObj2, *nodeObj;
  double areaCellObj1, areaCellObj2;
  double widthCellObj1, widthCellObj2;
  double totalWidth, totalArea;
  double edgeWeight;
  uint edgeIdx;
  bool rtv, scoreComputed;
  
  nodeIdx2 = 0;
  score = 0.0;
  rtv = false;
  scoreComputed = false;
  cellObj1 = (Cell *)myGraph.GetNodeObject(nodeIdx1);
  areaCellObj1 = (*cellObj1).CellGetArea();
  widthCellObj1 = (*cellObj1).CellGetWidth();
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
      widthCellObj2 = (*cellObj2).CellGetWidth();
      areaCellObj2 = (*cellObj2).CellGetArea();
      score = 0;
      if (widthCost) {
	totalWidth = widthCellObj1 + widthCellObj2;
	if (totalWidth > maxWidth) {
	  continue;
	}
	score = edgeWeight / totalWidth;
      } else {
	totalArea = areaCellObj1 + areaCellObj2;
	if (totalArea > maxArea) {
	  continue;
	}
	score = edgeWeight / totalArea;
      }
      _KEY_EXISTS_WITH_VAL(scoreVectIdxMap, nodeIdx2, itr) {
	(scoreVect[(itr->second)]).second += score;
      } else {
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
inline bool
getBestNeighborCstr(HyperGraph &myGraph, uint &nodeIdx1, uint &nodeIdx2, 
			   double &score, double maxArea, double maxWidth, 
			   bool widthCost)
{
  bool result;
  map<uint, bool> visitedLeftNodes;  
  
  result = getBestNeighborCstr(myGraph, nodeIdx1, nodeIdx2, score, 
			       visitedLeftNodes, maxArea, maxWidth, 
			       widthCost);
  
  return (result);
}

/* Builds the priority queue for best choice clustering */
priority_queue<objPairScore<uint>, vector<objPairScore<uint> >, objPairScoreCompareUint>
buildPriorityQueueCstr(HyperGraph &myGraph, double maxArea, double maxWidth, bool widthCost)
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
    result = getBestNeighborCstr(myGraph, nodeIdx1, nodeIdx2, score, 
				 visitedLeftNodes, maxArea, maxWidth, 
				 widthCost);
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
Design::DesignDoBestChoiceClusteringCstr(HyperGraph &myGraph, double maxArea, 
					 double maxWidth, bool widthCost,
					 double tratio)
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
  pqueue = buildPriorityQueueCstr(myGraph, maxArea, maxWidth, widthCost);

  //  cout << "BEGIN CLUSTERING NUMTOPNODES: " << numTopNodes << " " << getCPUTime() << "   " << getMemUsage() << endl;
  while (numTopNodes > (tratio * ((double)numOrigTopNodes))) {
    if (pqueue.empty()) {
      break;
    }
    /* Get the top pair u,v, score */
    objPairScore<uint> topPair = pqueue.top(); pqueue.pop();
    topPair.GetObjs(nodeIdx1, nodeIdx2);
    score = topPair.GetScore();
    if (!myGraph.NodeIsTop(nodeIdx1) || !myGraph.NodeIsTop(nodeIdx2)) {
      continue;
    }

    /* If u is invalid recompute the best neighbor for u */
    if (myGraph.NodeIsInvalid(nodeIdx1)) {
      if (getBestNeighborCstr(myGraph, nodeIdx1, nodeIdx2, score, maxArea,
				     maxWidth, widthCost)) {
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
    result = getBestNeighborCstr(myGraph, nodeIdx1, nodeIdx2, score, visitedLeftNodes, 
				 maxArea, maxWidth, widthCost);
    
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

/*******************************************************************************************/
/*********************************** SECOND FUNCTION ***************************************/
/*******************************************************************************************/
/* Supporting function for best choice clustering */
/* Get the best neighbour of a node u from the hypergraph */
inline
bool getBestNeighborPenalty(HyperGraph &myGraph, uint &nodeIdx1, uint &nodeIdx2, 
			    double &score, map<uint, bool> &visitedLeftNodes, 
			    double penalty, bool widthCost)
{
  map<uint, uint> scoreVectIdxMap;
  map<uint, uint>::iterator itr;
  map<uint, bool> visitedNodes;
  vector<pair<uint, double> > scoreVect;
  Cell *cellObj1, *cellObj2;
  void *nodeObj1, *nodeObj2, *nodeObj;
  double areaCellObj1, areaCellObj2;
  double widthCellObj1, widthCellObj2;
  double totalWidth, totalArea;
  double edgeWeight;
  uint edgeIdx;
  bool rtv, scoreComputed;
  
  nodeIdx2 = 0;
  score = 0.0;
  rtv = false;
  scoreComputed = false;
  cellObj1 = (Cell *)myGraph.GetNodeObject(nodeIdx1);
  areaCellObj1 = (*cellObj1).CellGetArea();
  widthCellObj1 = (*cellObj1).CellGetWidth();
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
      widthCellObj2 = (*cellObj2).CellGetWidth();
      areaCellObj2 = (*cellObj2).CellGetArea();
      score = 0;
      if (widthCost) {
	totalWidth = widthCellObj1 + widthCellObj2;
	score = edgeWeight / pow(totalWidth, penalty);
      } else {
	totalArea = areaCellObj1 + areaCellObj2;
	score = edgeWeight / pow(totalArea, penalty);
      }
      _KEY_EXISTS_WITH_VAL(scoreVectIdxMap, nodeIdx2, itr) {
	(scoreVect[(itr->second)]).second += score;
      } else {
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
inline bool
getBestNeighborPenalty(HyperGraph &myGraph, uint &nodeIdx1, uint &nodeIdx2, 
		       double &score, double penalty, bool widthCost)
{
  bool result;
  map<uint, bool> visitedLeftNodes;  
  
  result = getBestNeighborPenalty(myGraph, nodeIdx1, nodeIdx2, score, 
			       visitedLeftNodes, penalty, widthCost);
  return (result);
}

/* Builds the priority queue for best choice clustering */
priority_queue<objPairScore<uint>, vector<objPairScore<uint> >, objPairScoreCompareUint>
buildPriorityQueuePenalty(HyperGraph &myGraph, double penalty, bool widthCost)
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
    result = getBestNeighborPenalty(myGraph, nodeIdx1, nodeIdx2, score, 
				    visitedLeftNodes, penalty, widthCost);
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
Design::DesignDoBestChoiceClusteringPenalty(HyperGraph &myGraph, double penalty, 
					    bool widthCost, double tratio)
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
  pqueue = buildPriorityQueuePenalty(myGraph, penalty, widthCost);

  //  cout << "BEGIN CLUSTERING NUMTOPNODES: " << numTopNodes << " " << getCPUTime() << "   " << getMemUsage() << endl;
  while (numTopNodes > (tratio * ((double)numOrigTopNodes))) {
    if (pqueue.empty()) {
      break;
    }
    /* Get the top pair u,v, score */
    objPairScore<uint> topPair = pqueue.top(); pqueue.pop();
    topPair.GetObjs(nodeIdx1, nodeIdx2);
    score = topPair.GetScore();
    if (!myGraph.NodeIsTop(nodeIdx1) || !myGraph.NodeIsTop(nodeIdx2)) {
      continue;
    }

    /* If u is invalid recompute the best neighbor for u */
    if (myGraph.NodeIsInvalid(nodeIdx1)) {
      if (getBestNeighborPenalty(myGraph, nodeIdx1, nodeIdx2, score, penalty,
				 widthCost)) {
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
    result = getBestNeighborPenalty(myGraph, nodeIdx1, nodeIdx2, score, visitedLeftNodes, 
				    penalty, widthCost);
    
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

/*******************************************************************************************/
/*********************************** THIRD  FUNCTION ***************************************/
/*******************************************************************************************/
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
Design::DesignDoBestChoiceClustering(HyperGraph &myGraph, double tratio)
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
  while (numTopNodes > (tratio * ((double)numOrigTopNodes))) {
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

