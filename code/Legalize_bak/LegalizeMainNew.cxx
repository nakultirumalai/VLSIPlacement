void
LegalizeAugPathAlgoNew(HyperGraph &myGraph, LegalizeBin *suppNode, vector<LegalizeBin *> &PathBins)
{
  map<Cell *, double> moveCells;
  map<Cell*, double> AofV;
  map<LegalizeBin*, bool> usedNodes;
  map<LegalizeBin*,LegalizeBin*> pathMap;
  vector<pair<LegalizeBin* , double> > binsLValues;
  vector<LegalizeBin *> adjacentBins, reversePath;
  pair<LegalizeBin*, double> binLValObj;
  LegalizeBin *destBin, *currentNode, *prevNode;
  LegalizeBin *adjacentNode;
  LegalizeBin *currentBin, *prevBin, *nextBin;
  PhysRow *currentRow, *nextRow;
  double bValueCurrent;
  double currentNodeLValue;
  double fractWidthSelected;
  double supply;
  double currentLVal, adjLVal, modAdjLVal, moveCost;
  double totalFractWidth;
  uint numEdges, numNodes;
  int edgeIdx, binX, binY;
  bool sameZone;

  numEdges = myGraph.GetNumEdges();
  numNodes = myGraph.HyperGraphGetNumNodes();

  vector<map<Cell*, double> > chosenCells(numEdges);
  vector<double> totalFractWidths(numEdges);

  /* Clear all bins in PathBins */
  PathBins.clear();

  /* Assign all fractional cells of supply node to AofS */
  map<Cell *, double> &AofS = (*suppNode).BinGetAllFractCells();
  
  /* Set L-value of source node to 0 */
  LegalizeBinSetLValue(suppNode, 0.0);
  
  /* Set the b(v) of all supply bins to be 0 */
  HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
    supply = (*(LegalizeBin*)NodeObj).BinGetSupply();
    if (supply <= 0.0) {
      LegalizeBinSetBValue((LegalizeBin*)NodeObj, supply);
    } else {
      LegalizeBinSetBValue((LegalizeBin*)NodeObj, 0.0);
    }
  } HYPERGRAPH_END_FOR;
  
  /* Set the b(S) value to be equal to supply */
  supply = (*suppNode).BinGetSupply();
  LegalizeBinSetBValue(suppNode, supply);

  /* Set the dest bin to be NULL */
  destBin = NIL(LegalizeBin*);
  
  currentNode = suppNode;
  currentNodeLValue = LegalizeBinGetLValue(currentNode);
  
  /* Add the supply node to the used nodes map */
  usedNodes[currentNode] = 1;
  while (1) {
      /* Get the b value of the current node */
    bValueCurrent = LegalizeBinGetBValue(currentNode);
    if (currentNode != suppNode) {
      /* Compute the new b(v) */
      prevNode = pathMap[currentNode];
      edgeIdx = (int)myGraph.HyperGraphNodesAreAdjacent((void*)prevNode, (void*)currentNode);
      if (edgeIdx == -1) {
	cout << "SEVERE ERROR: Previous node";
	LegalizePrintBinData(prevNode);
	cout << " stored for current node: ";
	LegalizePrintBinData(currentNode);
	cout << " does not have an edge with the current node!!!" << endl;
      }
      fractWidthSelected = totalFractWidths[edgeIdx];
      LegalizeBinSetBValue(currentNode, (bValueCurrent + fractWidthSelected));

      /* Get the cells stored on the edge index */
      moveCells = chosenCells[edgeIdx];

      /* Find the union of cells from the current bin's 
	 fractional cells and the cells on the edge to the current
	 bin from the previous bin obtained from the pathMap */
      AofV.clear();
      AofV = (*currentNode).BinGetAllFractCells();
      MAP_FOR_ALL_ELEMS(moveCells, Cell*, double, Key, Value){
        _KEY_EXISTS(AofV, Key){
          AofV[Key] = AofV[Key] + Value;
        } else {
          AofV[Key] = Value;
        }
      } END_FOR;
    }

    /* Get the b value of the current node after it was updated */
    bValueCurrent = LegalizeBinGetBValue(currentNode);
    if (bValueCurrent <= 0) {
      destBin = currentNode;
      break;
    }
    
    /* Add the current node to the set of used nodes */
    usedNodes[currentNode] = 1;
    
    /* Clear the adjacentBins */
    adjacentBins.clear();

    /* Store the L value of the current node */
    currentLVal = LegalizeBinGetLValue(currentNode);

    /* Iterate over all the adjacent nodes of the current node */
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(myGraph, NodeObj, EdgeIdx, weight){
      adjacentNode = (LegalizeBin*)myGraph.GraphGetOtherNodeOfEdge(EdgeIdx, NodeObj);
      /* If the adjacent nodes is one of the used nodes, do not go there */
      _KEY_EXISTS(usedNodes, adjacentNode) {
	continue;
      }

      /* Get the L value of the adjacent node */
      adjLVal = LegalizeBinGetLValue(adjacentNode);

      /* Get the list of fractional cells to be moved, their widths and the 
	 total cost */
      sameZone = true;
      if ((*adjacentNode).BinGetBot() != (*currentNode).BinGetBot()) {
	sameZone = false;
      }
      moveCost = LegalizeGetCellsToMove((*currentNode), (*adjacentNode), binCells, 
					sameZone, cellsToMove, totalFractWidth);
      chosenCells[EdgeIdx] = cellsToMove;
      totalFractWidths[EdgeIdx] = totalFractWidth;
      
      /* Compute the new L value */
      modAdjLVal = currentLVal + moveCost;
      
      /* Update the L value and the path map and the cells to move 
	 if the new L value is smaller than the old L value */
      if (modAdjLVal < adjLVal) {
	LegalizeBinSetLValue(adjacentNode, modAdjLVal);
	pathMap[adjacentNode] = currentNode;
	binsLValues.push_back(make_pair(adjacentNode, modAdjLVal));
      }
    } HYPERGRAPH_END_FOR;

    /* Sort the bins on the basis of L values */
    sort(binsLValues.begin(), binsLValues.end(), sortLValues);

    /* Choose the first bin which is not in the map usedNodes.
       Since the L values are sorted, the bin chosen will have the 
       least L value */
    VECTOR_FOR_ALL_ELEMS(binsLValues, pair<LegalizeBin* MCOMMA double>, binLValObj){
      _KEY_EXISTS(usedNodes,(binLValObj.first)) {
        continue;
      } else {
        currentNode = binLValObj.first;
        break;
      }
    } END_FOR;

    /* Update iteration count */
  }
  
  if (destBin == NIL(LegalizeBin *)) {
    cout << "ERROR: Demand bin not found!!" << endl;
    return;
  }

  currentBin = destBin;
  reversePath.push_back(currentBin);
  PathBins.push_back(currentBin);

  prevBin = pathMap[currentBin];
  while (1) {
    currentBin = prevBin;
    reversePath.push_back(currentBin);
    PathBins.push_back(currentBin);
    if (currentBin == suppNode) {
      break;
    }
    prevBin = pathMap[currentBin];
  }

  /* Pop the source node */ 
  currentBin = reversePath.back();
  reversePath.pop_back();

  do {
    nextBin = reversePath.back();
    reversePath.pop_back();
    
    currentRow = (*currentBin).BinGetRow();
    nextRow = (*nextBin).BinGetRow();

    binX = (*nextBin).BinGetBegin();
    binY = (*nextBin).BinGetBot();

    edgeIdx = myGraph.HyperGraphNodesAreAdjacent((void*)currentBin, (void*)nextBin);
    cellsToMove = chosenCells[edgeIdx];
    vector<Cell*> cellsCurrBin = (*currentBin).BinGetCellsInBin();
    Cell *thisCell;
    double cellFract;
    int thisCellY;
    int currBinSize = cellsCurrBin.size();
    for(int cellCount = 0; cellCount < currBinSize; cellCount++) {
      thisCell = cellsCurrBin[cellCount];
      thisCellY = (*thisCell).CellGetYpos();
      _KEY_EXISTS(cellsToMove, thisCell) {
	(*currentBin).BinRemoveCellFromBin(thisCell);
	(*currentBin).BinRemoveCellFromFract(thisCell);
	(*nextBin).BinAddCellToBin(thisCell);
	(*nextBin).BinSetCellFract(thisCell, 1.0);
	if (binY != thisCellY) {
	  (*currentRow).PhysRowRemoveCellFromRow(thisCell);
	  (*nextRow).PhysRowAddCellToRow(thisCell);
	}
      }
    }
    
    Cell* Key;
    double Value;
    MAP_FOR_ALL_ELEMS(cellsToMove, Cell*, double, Key, Value){
      (*Key).CellSetXpos(binX);
      (*Key).CellSetYpos(binY);
    } END_FOR;
    currentBin = nextBin;

  } while(currentBin != destBin);

  LegalizeBinClearAllLValue();
  LegalizeBinClearAllBValue();
}

