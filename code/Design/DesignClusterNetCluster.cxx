# include <Design.h>
# include <lists.h>

bool
Design::DesignDoNetCluster(HyperGraph &myGraph, double maxArea, double maxWidth)
{
  uint numNodes, numEdges;
  uint idx, nodeIdx, edgeIdx, nodeNumEdges;
  uint maxGainIdx, zeroGainIdx;
  uint connectedNodeIdx;
  uint bucketIdx, bucketIdxOld, bucketIdxNew;
  int gainOfNode, oldGain, newGain;
  uint gainBucketSize, gainToAdd;
  double edgeWeight;
  bool netDebug;
  listNode *nodePtr;
  vector<dllist> bucketList;
  vector<uint> fromNodesOfEdge;
  vector<uint> toNodesOfEdge;

  gainBucketSize = DesignGetNumMaxCellPins();
  zeroGainIdx = gainBucketSize;
  gainBucketSize *= 2; gainBucketSize++;
  numNodes = myGraph.GetNumNodes();
  numEdges = myGraph.GetNumEdges();
  vector<listNode *> nodeInBucket(numNodes);
  vector<int> gains(numNodes);

  for (idx = 0; idx < gainBucketSize; idx++) {
    bucketList.push_back(dllist());
  }
  
  vector<char> partitions(numNodes);
  vector<char> terminalCells(numNodes);
  /* Populate the initial gain bucket */
  maxGainIdx = 0;
  for (nodeIdx = 0; nodeIdx < numNodes; nodeIdx++) {
    nodeNumEdges = myGraph.NodeGetNumEdges(nodeIdx);
    gainOfNode = -nodeNumEdges;
    if (gainOfNode == -1) {
      terminalCells[nodeIdx] = 1;
      continue;
    }
    terminalCells[nodeIdx] = 0;
    gains[nodeIdx] = gainOfNode;
    bucketIdx = gainOfNode + zeroGainIdx;
    if (bucketIdx > maxGainIdx) {
      maxGainIdx = bucketIdx;
    }
    dllist &listAtGain = bucketList[bucketIdx];
    nodeInBucket[nodeIdx] = listAtGain.addNode(nodeIdx);
    partitions[nodeIdx] = 0;
  }
  
  /* Compute the T(n) and F(n) values for each net */
  for (edgeIdx = 0; edgeIdx < numEdges; edgeIdx++) {
    toNodesOfEdge.push_back(0);
    fromNodesOfEdge.push_back(myGraph.EdgeGetNumNodes(edgeIdx));
  }

  netDebug = true;
  /* Move a seed cell and update the gains of all neighboring cells */
  nodeIdx = 1;
  if (netDebug) {
    cout << "NETCLUSTER DEBUG: Moving " << nodeIdx << " to natural" << endl;
  }

  /* Move to the other partition */
  partitions[nodeIdx] = 1;
  while (1) {
    /* Traverse through all the connected edges, decrement fromNodesOfEdge
       increment toNodesOfEdge */
    uint fromBefore, fromAfter;
    uint toBefore, toAfter;
    HYPERGRAPH_FOR_ALL_EDGES_OF_NODE(myGraph, nodeIdx, edgeIdx, edgeWeight) {
      fromBefore = fromNodesOfEdge[edgeIdx];
      toBefore = toNodesOfEdge[edgeIdx];
      fromNodesOfEdge[edgeIdx] -= 1; fromAfter = fromBefore - 1;
      toNodesOfEdge[edgeIdx] += 1; toAfter = toBefore + 1;
      if (netDebug) {
	cout << "NETCLUSTER DEBUG: Visting edge " << edgeIdx;
	cout << " FB: " << fromBefore << " TB: " << toBefore;
	cout << " FA: " << fromAfter << " TA: " << toAfter << endl;
      }
      gainToAdd = 0;
      if (fromAfter == 1) gainToAdd++;
      if (toAfter == 1) gainToAdd++;
      if (netDebug) {
	cout << "NETCLUSTER DEBUG: Updating gains of all nodes on edge " << edgeIdx 
	     << " by " << gainToAdd << endl;
      }
      if (gainToAdd == 0) continue;
      /* Gain update on these nodes required */
      HYPERGRAPH_FOR_ALL_NODES_OF_EDGE(myGraph, edgeIdx, connectedNodeIdx) {
	if (terminalCells[connectedNodeIdx]) continue;
	/* If the node has been moved, skip node */
	if (partitions[connectedNodeIdx] == 1) continue;
	oldGain = gains[connectedNodeIdx];
	newGain = oldGain + gainToAdd;
	if (netDebug) {
	  cout << "NETCLUSTER DEBUG: Node: " << connectedNodeIdx 
	       << " BG: " <<  oldGain << " AG: " << newGain << endl;
	}
	/* Move the node to its appropriate position in the bucket */
	nodePtr = nodeInBucket[connectedNodeIdx];
	bucketIdxOld = oldGain + zeroGainIdx;
	bucketIdxNew = newGain + zeroGainIdx;
	if (netDebug) {
	  cout << "NETCLUSTER DEBUG: old bucket index: " << bucketIdxOld 
	       << " new bucket index: " << bucketIdxNew << endl;
	}
	if (bucketIdxNew > maxGainIdx) {
	  uint oldMaxGainIdx = maxGainIdx;
	  maxGainIdx = bucketIdxNew;
	  if (netDebug) {
	    cout << "NETCLUSTER DEBUG: old max gain index: " << oldMaxGainIdx
		 << " new max gain index: " << maxGainIdx << endl;
	  }
	}
	dllist &listAtGainOld = bucketList[bucketIdxOld];
	dllist &listAtGainNew = bucketList[bucketIdxNew];
	if (netDebug) {
	  cout << "NETCLUSTER DEBUG: old bucket index: " << bucketIdxOld 
	       << " new bucket index: " << bucketIdxNew << endl;
	}
	listAtGainOld.deleteNode(nodePtr);
	listAtGainNew.addNode(nodePtr);
      } HYPERGRAPH_END_FOR;
    } HYPERGRAPH_END_FOR;
    
    if (maxGainIdx < zeroGainIdx) {
      break;
    }
    /* Find the next node to be moved */
    dllist &listAtMaxGain = bucketList[maxGainIdx];
    nodePtr = listAtMaxGain.popNode();
    nodeIdx = (*nodePtr).GetData();
    /* Adjust the maxgainindex here */
  }
  
  return true;
}
