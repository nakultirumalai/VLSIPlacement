#include <FDP.h>

FDPSite*
FDPGetSiteOfCell(vector<vector<FDPSite*> > &gridMatrix, Cell *thisCell, 
		 int siteWidth, int rowHeight)
{
  int cellXpos, cellYpos;
  int siteNum, rowNum;
  int cellWidth;
  FDPSite *thisSite;
  cellXpos = (*thisCell).CellGetXpos();
  cellYpos = (*thisCell).CellGetYpos();
  siteNum = cellXpos / siteWidth;
  rowNum = cellYpos / rowHeight;
  thisSite = gridMatrix[rowNum][siteNum];
  return (thisSite);
}

void
FDPGetRoundedSiteLocation(double tempX, double tempY, int rowHeight, 
			  int siteWidth, int &X, int &Y, int numRows,
			  int numSitesInRow)
{
  int modVal, halfVal;
  double quotient;
  
  /* Optimal rounded off value for X */
  X = dtoi(tempX / siteWidth);
  if (X == numSitesInRow) {
    X = numSitesInRow;
  }
  X = X * siteWidth;

  /* Optimal rounded off value for Y */
  Y = dtoi(tempY / rowHeight);
  if (Y == numRows) {
    Y = numRows - 1;
  }
  Y = Y * rowHeight;
}

void
FDPGetOptLocation(vector<void*> &connectedNodes, vector<Cell*> &movableCells,
		  vector<Cell*> &fixedCells, map<Cell *, bool> &mapOfCells,
		  HyperGraph &myGraph, int rowHeight, int siteWidth, Cell *thisCell, 
		  int &x, int &y, int numRows, int numSitesInRow, bool topLevel)
{
  void *thisNode;
  void *seedNode;
  long int edgeIdx;
  map<Cell*, bool> cellsInCluster;
  double edgeWeight;
  double sumWDX, sumWDY, sumW;
  double tempX, tempY;
  int Xj, Yj;
    
  if ((*thisCell).CellGetName() == "c57") {
    cout << "BREAK HERE " << endl;
  }
  seedNode = (void*)thisCell;
  sumWDX = 0;
  sumWDY = 0;
  sumW = 0;
  
  if (!(connectedNodes.size())) {
    _ASSERT_TRUE("Unconnected cell is found");
  }
  

  VECTOR_FOR_ALL_ELEMS(connectedNodes, void*, thisNode) {
    thisCell = (Cell*)thisNode;
    if (!topLevel) {
      _KEY_DOES_NOT_EXIST(mapOfCells, thisCell) {
	continue;
      }
    } else {
      Xj = (int)(*thisCell).CellGetXpos();
      Yj = (int)(*thisCell).CellGetYpos();
      edgeIdx = myGraph.NodesAreAdjacent(thisNode, seedNode);
      if (edgeIdx == -1){
	_ASSERT_TRUE("ERROR: No edge found between connected cells");
      }
      edgeWeight = myGraph.GetEdgeWeight(edgeIdx);
      if (edgeWeight < 0) {
	_ASSERT_TRUE("ERROR: Negative edge weight found");
      }

      /* Summation WiXi */
      sumWDX += ((double)Xj * edgeWeight);
      /* Summation WiYi */
      sumWDY += ((double)Yj * edgeWeight);
      /* Summation Wi */
      sumW += edgeWeight;
    }
  } END_FOR;
  
  tempX = sumWDX / sumW;
  tempY = sumWDY /sumW;
  
  /* round off the x and y values to the nearest site location */
  FDPGetRoundedSiteLocation(tempX, tempY, rowHeight, siteWidth, x, y, numRows, numSitesInRow);
}

void
FDPGetFourNeighbours(int siteNum, int rowNum, int numSitesInRow, int numRows,
		      vector<int> &fourNeighbours)
{
  int leftX, rightX;
  int topY, botY;
  
  leftX = siteNum - 1;
  topY = rowNum + 1;
  rightX = siteNum + 1;
  botY = rowNum - 1;
  
  if (leftX >= 0) {
    /* Insert the left neighbour */
    fourNeighbours.push_back(leftX);
    fourNeighbours.push_back(rowNum);
  } 
  
  if (topY < numRows) {
    /* Insert the top neighbour */
    fourNeighbours.push_back(siteNum);
    fourNeighbours.push_back(topY);
  }

  if (rightX <= (numSitesInRow - 1)) {
    /* Insert the right neighbour */
    fourNeighbours.push_back(rightX);
    fourNeighbours.push_back(rowNum);
  }

  if (botY >= 0) {
    /* Insert the right neighbour */
    fourNeighbours.push_back(siteNum);
    fourNeighbours.push_back(botY);
  }
}
  
FDPSite*
FDPGetNearestVacantSite(FDPSite *thisSite, vector<vector<FDPSite*> > &gridMatrix, 
			int numSitesInRow, int numRows, Cell *&nextCell)
{
  queue<FDPSite*> myQueue;
  map<FDPSite*, bool> visitedSites;
  vector<int> fourNeighbours;
  FDPSite *selectedSite;
  bool foundVacantSite;
  bool foundOccupied;
  int siteNum, rowNum;
  int newSiteNum, newRowNum;
  int i;
  
  foundOccupied = false;
  visitedSites[thisSite] = true;
  siteNum = (*thisSite).FDPSiteGetSiteNum();
  rowNum = (*thisSite).FDPSiteGetRowNum();
  foundVacantSite = false;
  FDPGetFourNeighbours(siteNum, rowNum, numSitesInRow, numRows, fourNeighbours);
  while (fourNeighbours.size()) {
    for (i = 0; i < fourNeighbours.size(); i += 2) {
      newSiteNum = fourNeighbours[i];
      newRowNum = fourNeighbours[i+1];
      selectedSite = gridMatrix[newRowNum][newSiteNum];
      if ((*selectedSite).FDPSiteGetHasCell()) {
	if ((*selectedSite).FDPSiteGetIsLocked()) {
	  _KEY_EXISTS(visitedSites, selectedSite) {
	  } else {
	    visitedSites[selectedSite] = true;
	    myQueue.push(selectedSite);
	  }
	} else {
	  foundVacantSite = true;
	  foundOccupied = true;
	  nextCell = (*selectedSite).FDPSiteGetHasCell();
	  break;
	}
      } else {
	foundVacantSite = true;
	break;
      }
    }

    if (foundVacantSite) {
      break;
    } 
    thisSite = myQueue.front();
    if (myQueue.empty()) {
      _ASSERT_TRUE("Queue is empty");
    }
    myQueue.pop();
    siteNum = (*thisSite).FDPSiteGetSiteNum();
    rowNum = (*thisSite).FDPSiteGetRowNum();    
    fourNeighbours.clear();
    FDPGetFourNeighbours(siteNum, rowNum, numSitesInRow, numRows, fourNeighbours);
  }
  if (foundVacantSite && foundOccupied) {
    return selectedSite;
  } else if (foundVacantSite && !foundOccupied) {
    nextCell = NIL(Cell*);
    return selectedSite;
  } else {
    return NIL(FDPSite*);
  }
}

FDPSite*
FDPGetNearestVacantSite(FDPSite *thisSite, vector<vector<FDPSite*> > &gridMatrix, 
			int numSitesInRow, int numRows)
{
  queue<FDPSite*> myQueue;
  map<FDPSite*, bool> visitedSites;
  vector<int> fourNeighbours;
  FDPSite *selectedSite;
  bool foundVacantSite;
  int siteNum, rowNum;
  int newSiteNum, newRowNum;
  int i;
  
  visitedSites[thisSite] = true;
  siteNum = (*thisSite).FDPSiteGetSiteNum();
  rowNum = (*thisSite).FDPSiteGetRowNum();
  foundVacantSite = false;
  FDPGetFourNeighbours(siteNum, rowNum, numSitesInRow, numRows, fourNeighbours);
  while (fourNeighbours.size()) {
    for(i = 0; i < fourNeighbours.size(); i+=2) {
      newSiteNum = fourNeighbours[i];
      newRowNum = fourNeighbours[i+1];
      selectedSite = gridMatrix[newRowNum][newSiteNum];
      if ((*selectedSite).FDPSiteGetHasCell()) {
	_KEY_EXISTS(visitedSites, selectedSite) {
	  /* Don't do anything */
	} else {
	  visitedSites[selectedSite] = true;
	  myQueue.push(selectedSite);
	}
      } else {
	foundVacantSite = true;
	break;
      }
    } 

    if (foundVacantSite) {
      break;
    } 
    thisSite = myQueue.front();
    if (myQueue.empty()) {
      _ASSERT_TRUE("Queue is empty");
    }
    myQueue.pop();
    siteNum = (*thisSite).FDPSiteGetSiteNum();
    rowNum = (*thisSite).FDPSiteGetRowNum();    
    fourNeighbours.clear();
    FDPGetFourNeighbours(siteNum, rowNum, numSitesInRow, numRows, fourNeighbours);
  }
  if (foundVacantSite) {
    return selectedSite;
  } else {
    return NIL(FDPSite*);
  }
}

void
FDPClearAllLocks(vector<Cell*> &movableCells, vector<vector<FDPSite*> > &gridMatrix,
		 int rowHeight, int siteWidth)
{
  Cell *thisCell;
  FDPSite *thisSite;
  int cellX, cellY;
  int siteNum, rowNum;
  VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, thisCell){
    cellX = (*thisCell).CellGetXpos();
    cellY = (*thisCell).CellGetYpos();
    siteNum = cellX / siteWidth;
    rowNum = cellY / rowHeight;
    thisSite = gridMatrix[rowNum][siteNum];
    (*thisSite).FDPSiteSetIsLocked(0);
  } END_FOR;
}

void
printQueueSize(queue<FDPSite*> &myQueue)
{
  cout << "Size is " << myQueue.size() << endl;
}

void
printVacantSites(vector<vector<FDPSite*> > &gridMatrix)
{
  FDPSite *thisSite;
  vector<FDPSite*> sitesInRow;
  int count;
  count = 0;
  VECTOR_FOR_ALL_ELEMS(gridMatrix, vector<FDPSite*>, sitesInRow) {
    VECTOR_FOR_ALL_ELEMS(sitesInRow, FDPSite*, thisSite) {
      if ((*thisSite).FDPSiteGetHasCell() == NIL(Cell*)) {
	cout << "[" << (*thisSite).FDPSiteGetSiteNum() << "," << (*thisSite).FDPSiteGetRowNum() << "]" << endl;
	count++;
      }
    } END_FOR;
  } END_FOR;
  cout << "Number of vacant sites in design : " << count << endl;
}

void
FDPPrintAllConnectedNodes(vector<void*> &connectedNodes) 
{
  Cell *thisCell;
  void *thisNode;
  cout << "Connected cells are : " << endl;
  VECTOR_FOR_ALL_ELEMS(connectedNodes, void*, thisNode) {
    thisCell = (Cell*)thisNode;
    cout << (*thisCell).CellGetName() << " ";
  } END_FOR;
  cout << endl;
}

void
FDPGetSiteInfo(vector<vector<FDPSite*> > &gridMatrix) 
{
  FDPSite *thisSite;
  vector<FDPSite*> rowSites;
  
}

/* Top level function to place the cells using a force-directed heuristic */
void 
FDPTopLevel(vector<Cell*> &fixedCells, vector<Cell*> &movableCells, 
	    int numRows, int numSitesInRow, int rowHeight, int siteWidth,
	    bool topLevel, HyperGraph &myGraph)
{
  map<Cell*, bool> usedCells;
  map<Cell*, bool> mapOfCells;
  vector<vector<FDPSite*> > gridMatrix;
  vector<FDPSite*> oneRow;
  vector<int> rowCap;
  vector<void*> connectedNodes;
  Cell *thisCell;
  Cell *nextCell;
  FDPSite *thisSite;
  FDPSite *nearestSite;
  void *thisNode;
  int siteXpos, siteYpos;
  int cellXpos, cellYpos;
  int cellWidth, cellEndX, rowEnd;
  int siteX, siteY;
  int xOpt, yOpt;
  int siteNum, rowNum;
  int vacantSiteBegin, vacantSiteEnd;
  int numVacantSites, siteCount;
  int numSitesOfCell;
  int lastPlacedCell, nextSiteBegin;
  int rowCapInRow;
  int iter_count, abort_count;
  int counter, abort_limit, iter_limit;
  int oldSiteNum, oldRowNum;
  uint i, j;
  uint fixedCellNum;
  uint cellNum;
  bool end_ripple;
  bool optimizeFind;
  uint stepCount;
  siteCount = 1;
  stepCount = 1;

  optimizeFind = true;
  
  rowCapInRow = numSitesInRow * siteWidth;
  rowEnd = numSitesInRow * siteWidth;
  
  /* Create a grid containing all the sites for a design */
  for (i = 0; i < numRows; i++) {
    siteYpos = i * rowHeight;
    for (j = 0; j < numSitesInRow; j++) {
      siteXpos = j * siteWidth;
      thisSite = new FDPSite();
      (*thisSite).FDPSiteSetSiteNum(j);
      (*thisSite).FDPSiteSetRowNum(i);
      (*thisSite).FDPSiteSetXpos(siteXpos);
      (*thisSite).FDPSiteSetYpos(siteYpos);
      oneRow.push_back(thisSite);
      siteCount++;
    }
    gridMatrix.push_back(oneRow);
    rowCap.push_back(rowCapInRow);
    oneRow.clear();
  }
  
  cout << "STEP " << stepCount++ << ". Site objects created and initialized" << endl;
  cout << "Total number of Sites created : " << (siteCount - 1) << endl;
  
  if (fixedCells.size()) {
    /* Mark the gridMatrix with cellNumbers that are locked */
    VECTOR_FOR_ALL_ELEMS(fixedCells, Cell*, thisCell) {
      /* Found an internal cell */
      cellXpos = (int)(*thisCell).CellGetXpos();
      cellYpos = (int)(*thisCell).CellGetYpos();
      siteNum = cellXpos / siteWidth;
      rowNum = cellYpos / rowHeight;
      cellWidth = (int)(*thisCell).CellGetWidth();
      numSitesOfCell = (cellXpos + cellWidth) / siteWidth;
      siteX = cellXpos;
      for (i = 0; i < numSitesOfCell; i++) {
	thisSite = gridMatrix[rowNum][siteNum];
	(*thisSite).FDPSiteSetIsLocked(1);
	siteX += siteWidth;
      }
      rowCap[cellYpos] -= cellWidth;
      mapOfCells[thisCell] = true;
    } END_FOR;
  }
  
  /* Adding the movable cells to the map of cells to check if a connected 
     cell lies outside the cluster or not. (NOT REQUIRED FOR TOP LEVEL).
  */
  if (!topLevel) {
    VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, thisCell) {
      mapOfCells[thisCell] = true;
    } END_FOR;
  }

  /* Creating an intial placement of the internal cells and the residual
     boudary cells inside the cluster. (NOT REQUIRED FOR TOP LEVEL).
  */
  if (!(topLevel)) {
    // Keeps a track of which cell index is to best placed 
    lastPlacedCell = 0;
    for (i = 0; i < numRows; i++) {
      vacantSiteBegin = 0;
      for (j = 0; j < numSitesInRow; j++) {
	thisSite = gridMatrix[i][j];
	if ((*thisSite).FDPSiteGetIsLocked()) {
	  vacantSiteBegin += siteWidth;
	}
      }
      if (vacantSiteBegin < rowEnd) {
	nextSiteBegin = vacantSiteBegin;
	thisCell = movableCells[lastPlacedCell];
	cellWidth = (*thisCell).CellGetWidth();
	while (((nextSiteBegin + cellWidth) <= rowEnd) && (rowCap[i] > 0)) {
	  thisSite = gridMatrix[i][nextSiteBegin];
	  (*thisSite).FDPSiteSetHasCell(thisCell);
	  rowCap[i] -= cellWidth;
	  lastPlacedCell++;
	  nextSiteBegin += siteWidth;
	  thisCell = movableCells[lastPlacedCell];
	  cellWidth = (*thisCell).CellGetWidth();
	} 
      }
    }
  } else if(topLevel) {
    /* USED ONLY FOR TOP LEVEL. Initializes the sites with the cells placed using 
       a constructive placement strategy where cells have already been 
       placed one after the other
    */
    cout << "STEP " << stepCount++ << ". Assign the cells in the intial placement to sites" << endl;
    cout << "Number of cells to be placed : " << movableCells.size() << endl;
    VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, thisCell) {
      cellXpos = (*thisCell).CellGetXpos();
      cellYpos = (*thisCell).CellGetYpos();
      siteNum = cellXpos / siteWidth;
      rowNum = cellYpos / rowHeight;
      thisSite = gridMatrix[rowNum][siteNum];
      (*thisSite).FDPSiteSetHasCell(thisCell);
    } END_FOR;
  }
  
  cout << "STEP " << stepCount++ << ". Iterative refinement starts here." << endl;
  /* MAIN ALGORITHM FORCE DIRECTED PLACEMENT STARTS HERE */
  iter_count = 0;
  iter_limit = 910;
  abort_limit = 5;
  abort_count = 0;

  /* Do iterative refinement on the initial placement using a force directed heuristic */
  counter = 0;
  
  while(iter_count < iter_limit) {
    cout << "Iteration Number " << iter_count << endl;
    if (counter >= movableCells.size()) {
      cout << "ALL CELLS USED. RECYCLING CELLS " << endl;
      counter = 0;
      usedCells.clear();
      iter_count++;
    } 
    
    
    /* Select any cell from the list of movable cells */
    /* No sorting is done based on connectivity */
    thisCell = movableCells[counter++];
    
    _KEY_EXISTS(usedCells, thisCell) {
      cout << "Cell " << (*thisCell).CellGetName() << " had already been selected earlier. " << endl;
      continue;
    }
    
    /* Add cell to a map to skip in next iteration */
    usedCells[thisCell] = true;
    cout << "Added cell " << (*thisCell).CellGetName() << "to usedCells map " << endl;
    /* Change the location of that cell to vacant before removing it 
       from that site */
    cout << "Cell " << (*thisCell).CellGetName() << "was at location (" << (*thisCell).CellGetXpos() 
	 << "," << (*thisCell).CellGetYpos() << ") " << endl;
    thisSite = FDPGetSiteOfCell(gridMatrix, thisCell, siteWidth, rowHeight);
    cout << "So marking site at (" << (*thisSite).FDPSiteGetXpos() << "," << (*thisSite).FDPSiteGetYpos()
	 << ") vacant " << endl;
    (*thisSite).FDPSiteRemoveCell();
    oldSiteNum = (*thisSite).FDPSiteGetSiteNum();
    oldRowNum = (*thisSite).FDPSiteGetRowNum();
    
    while(!end_ripple) {
      cout << "SELECTED CELL FOR MOVEMENT: " << (*thisCell).CellGetName() << endl;
      thisNode = (void*)thisCell;
      connectedNodes = myGraph.GetConnectedNodes(thisNode);
      FDPPrintAllConnectedNodes(connectedNodes);
      
      cout << "STEP " << stepCount++ << ". Get the cells connected to " << (*thisCell).CellGetName() << endl;
      if (stepCount == 219) {
	cout << "BREAK HERE " << endl;
      }
      /* Get equilibrium position of cell */
      FDPGetOptLocation(connectedNodes, movableCells, fixedCells, mapOfCells,
			myGraph, rowHeight, siteWidth, thisCell, siteXpos, 
			siteYpos, numRows, numSitesInRow, topLevel);
      cout << "Optimal location for cell " << (*thisCell).CellGetName() << "is (" 
	   << siteXpos << "," << siteYpos << ")" << endl;
      siteNum = siteXpos / siteWidth;
      rowNum = siteYpos / rowHeight;
      if ((siteNum == oldSiteNum) && (rowNum == oldRowNum)) {
	cout << "Optimal location is the same as present location" << endl;
	(*thisSite).FDPSiteSetHasCell(thisCell);
	end_ripple = true;
	continue;
      }
      /* Get the site object associated with the cell */
      thisSite = gridMatrix[rowNum][siteNum];
	
      /* CASE : LOCKED */
      if ((*thisSite).FDPSiteGetIsLocked()) {
	/* The equilibrium position for the cell is locked. So choose a vacant
	   site nearby 
	*/
	
	/* If optimizeFind is set it will choose the nearest UNLOCKED site
	   irrespective of whether its vacant 
	*/
	if (optimizeFind) {
	  /* Overloaded function to also give the next cell to be placed */
	  nearestSite = FDPGetNearestVacantSite(thisSite, gridMatrix, numSitesInRow, numRows, nextCell);
	  if (nearestSite == NIL(FDPSite*)) {
	    _ASSERT_TRUE("ERROR: Cannot find a vacant site for cell to be placed");
	  } else {
	    (*nearestSite).FDPSiteSetHasCell(thisCell);
	    (*nearestSite).FDPSiteSetIsLocked(1);
	    siteX = (*nearestSite).FDPSiteGetXpos();
	    siteY = (*nearestSite).FDPSiteGetYpos();
	    (*thisCell).CellSetXpos(siteX);
	    (*thisCell).CellSetYpos(siteY);
	    cout << "Cell " << (*thisCell).CellGetName() << " is placed at (" << siteX << "," << siteY 
		 << ") " << "[ " << (*nearestSite).FDPSiteGetSiteNum() << "," << (*nearestSite).FDPSiteGetRowNum() 
		 << "]" << endl;
	    
	    if (nextCell != NIL(Cell*)) {
	      cout << "Next cell to be placed " << (*nextCell).CellGetName() << endl;
	      end_ripple = false;
	      usedCells[thisCell] = true;
	      thisCell = nextCell;
	    }
	    if (nextCell == NIL(Cell*)) {
	      end_ripple = true;
	      usedCells[thisCell] = true;
	    }
	    abort_count++;
	    FDPSite *prevNearestSite = nearestSite;
	    if (abort_count > abort_limit) {
	      if (nextCell != NIL(Cell*)) {
		nearestSite = FDPGetNearestVacantSite(prevNearestSite, gridMatrix, numSitesInRow, numRows);
		if (nearestSite == NIL(FDPSite*)) {
		  _ASSERT_TRUE("ERROR: Cannot find a vacant site for cell to be placed");
		} else {
		  (*nearestSite).FDPSiteSetHasCell(thisCell);
		  siteX = (*nearestSite).FDPSiteGetXpos();
		  siteY = (*nearestSite).FDPSiteGetYpos();
		  (*thisCell).CellSetXpos(siteX);
		  (*thisCell).CellSetYpos(siteY);
		}
	      }
	      FDPClearAllLocks(movableCells, gridMatrix, rowHeight, siteWidth);
	      iter_count++;
	      usedCells.clear();
	      end_ripple = true;
	      abort_count++;
	    }
	  }
	} else {
	  nearestSite = FDPGetNearestVacantSite(thisSite, gridMatrix, numSitesInRow, numRows);
	  if (nearestSite == NIL(FDPSite*)) {
	    _ASSERT_TRUE("ERROR: Cannot find a vacant site for cell to be placed");
	  } else {
	    (*nearestSite).FDPSiteSetHasCell(thisCell);
	    (*nearestSite).FDPSiteSetIsLocked(1);
	    siteX = (*nearestSite).FDPSiteGetXpos();
	    siteY = (*nearestSite).FDPSiteGetYpos();
	    (*thisCell).CellSetXpos(siteX);
	    (*thisCell).CellSetYpos(siteY);
	    cout << "Cell " << (*thisCell).CellGetName() << " is placed at (" << siteX << "," << siteY 
		 << ") " << "[ " << (*nearestSite).FDPSiteGetSiteNum() << "," << (*nearestSite).FDPSiteGetRowNum() 
		 << "]" << endl;
	    end_ripple = true;
	    abort_count++;
	    if (abort_count > abort_limit) {
	      FDPClearAllLocks(movableCells, gridMatrix, rowHeight, siteWidth);
	      usedCells.clear();
	      iter_count++;
	    }
	  }
	}
      } else {
	/* If Site is occupied or vacant but not locked. First place the cell
	   at its equilibrium position 
	*/
	nextCell = (*thisSite).FDPSiteGetHasCell();
	(*thisSite).FDPSiteSetHasCell(thisCell);
	(*thisSite).FDPSiteSetIsLocked(1);
	siteX = (*thisSite).FDPSiteGetXpos();
	siteY = (*thisSite).FDPSiteGetYpos();
	(*thisCell).CellSetXpos(siteX);
	(*thisCell).CellSetYpos(siteY);
	cout << "Cell " << (*thisCell).CellGetName() << " is placed at (" << siteX << "," << siteY 
	     << ") " << "[ " << (*thisSite).FDPSiteGetSiteNum() << "," << (*thisSite).FDPSiteGetRowNum() 
	     << "]" << endl;

	usedCells[thisCell] = true;
	if (nextCell != NIL(Cell*)) {
	  /* Indicates site is occupied. So choose the displaced cell to moved next */
	  cout << "Next cell to be placed " << (*nextCell).CellGetName() << endl;
	  thisCell = nextCell;
	  end_ripple = false;
	  abort_count = 0;
	} else {
	  /* Indicates site is vacant. So place the cell in the site and lock it */
	  end_ripple = true;
	  abort_count = 0;
	}
      }
    }
    end_ripple = false;
  }
}

