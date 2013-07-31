#include <FDP.h>
# define Dir PIN_DIR_ALL

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

  if (rightX < numSitesInRow) {
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
  /* Get the four neighbouring sites to the present site, top, right, bot, left and check
     if they are unlocked 
  */
  FDPGetFourNeighbours(siteNum, rowNum, numSitesInRow, numRows, fourNeighbours);
  while (fourNeighbours.size()) {
    for (i = 0; i < fourNeighbours.size(); i += 2) {
      newSiteNum = fourNeighbours[i];
      newRowNum = fourNeighbours[i+1];
      /* Get the neighbouring site object */
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
FDPTopLevel(vector<Cell*> &allCells, int numRows, int numSitesInRow, 
	    int rowHeight, int siteWidth, HyperGraph &myGraph)
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
  Net *NetPtr;
  double netWeight;
  double sumWx, sumWy;
  double sumW;
  double tempX, tempY;
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
  bool debugMode;
  debugMode = false;
  
  uint stepCount;
  siteCount = 1;
  stepCount = 1;

  optimizeFind = true;
  
  rowCapInRow = numSitesInRow * siteWidth;
  rowEnd = numSitesInRow * siteWidth;
  
  /**********************************************************/
  /*  STEP 1. Create a grid for the cells to be placed */
  /**********************************************************/
  
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
  
  cout << "STEP " << stepCount++ << ". " << (siteCount - 1) << " SITE OBJECTS CREATED. " << endl;

  cout << "STATS: Number of cells to be placed - " << allCells.size() << endl;
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, thisCell) {
    cellXpos = (*thisCell).CellGetXpos();
    cellYpos = (*thisCell).CellGetYpos();
    siteNum = cellXpos / siteWidth;
    rowNum = cellYpos / rowHeight;
    thisSite = gridMatrix[rowNum][siteNum];
    (*thisSite).FDPSiteSetHasCell(thisCell);
  } END_FOR;
  cout << "STEP " << stepCount++ << ". ASSIGNED CELLS TO THEIR RESPECTIVE SITEs. Intial Placement" << endl;
  
  /************************************************************/
  /* STEP 2. Do iterative refinement on the initial solution */
  /***********************************************************/
  
  cout << "STEP " << stepCount++ << ". FORCE DIRECTED ITERATIVE REFINEMENT BEGINS HERE." << endl;
  
  /* MAIN ALGORITHM FORCE DIRECTED PLACEMENT STARTS HERE */
  iter_count = 0;
  iter_limit = 100;
  abort_limit = 3;
  abort_count = 0;

  /* Do iterative refinement on the initial placement using a force directed heuristic */
  counter = 0;
  while(iter_count < iter_limit) {
    cout << "STATS: Iteration Number - " << iter_count << endl;
    if (counter >= allCells.size()) {
      cout << "ALL CELLS USED. RECYCLING CELLS " << endl;
      counter = 0;
      usedCells.clear();
      iter_count++;
    }
    
    /************************************************************/
    /* STEP 3a. Select any cell from the list of cells that 
                hasn't been used yet 
    */
    /***********************************************************/

    /* No sorting is done based on connectivity */
    thisCell = allCells[counter++];
    _KEY_EXISTS(usedCells, thisCell) {
      if (debugMode) {
	cout << "Cell " << (*thisCell).CellGetName() << " had already been selected earlier. " << endl;
      }
      continue;
    }
    /* Add cell to a map to skip in next iteration */
    usedCells[thisCell] = true;
    if (debugMode) {
      cout << "Added cell " << (*thisCell).CellGetName() << "to usedCells map " << endl;
    }
    
    /************************************************************/
    /* STEP 3b. Mark the site of the selected cell as empty and
                remove the cell from the site
    */
    /***********************************************************/
    if (debugMode) {
      cout << "Cell " << (*thisCell).CellGetName() << "was at location(L) (" << (*thisCell).CellGetXpos() 
	   << "," << (*thisCell).CellGetYpos() << ") " << endl;
    }
    thisSite = FDPGetSiteOfCell(gridMatrix, thisCell, siteWidth, rowHeight);
    if (debugMode) {
      cout << "So marking site at (" << (*thisSite).FDPSiteGetXpos() << "," << (*thisSite).FDPSiteGetYpos()
	   << ") vacant " << endl;
    }
    (*thisSite).FDPSiteRemoveCell();
    oldSiteNum = (*thisSite).FDPSiteGetSiteNum();
    oldRowNum = (*thisSite).FDPSiteGetRowNum();
    
    while(!end_ripple) {
      if (debugMode) {
	cout << "SELECTED CELL FOR MOVEMENT - " << (*thisCell).CellGetName() << endl;
      }
      
      /**********************************************************************/
      /* STEP 3c. Get all the nets connected to this cell from the
	          Hypergraph and from each such net, get all the cells 
		  connected to it.
      */
      /*********************************************************************/

      sumWx = 0;
      sumWy = 0;
      sumW = 0;
      CELL_FOR_ALL_NETS((*thisCell), Dir, NetPtr) {
	netWeight = (*NetPtr).NetGetWeight();
	NET_FOR_ALL_CELLS((*NetPtr), nextCell) {
	  if (nextCell == thisCell) {
	    continue;
	  }
	  cellXpos = (*nextCell).CellGetXpos();
	  cellYpos = (*nextCell).CellGetYpos();
	  sumWx += (netWeight * cellXpos);
	  sumWy += (netWeight * cellYpos);
	  sumW += netWeight;
	} NET_END_FOR;
      } CELL_END_FOR;
      
      tempX = sumWx / sumW;
      tempY = sumWy / sumW;
      if (debugMode) {
	cout << "Exact optimal location of cell before rounding off : (" << tempX << "," 
	     << tempY << ")" << endl;
      }
      FDPGetRoundedSiteLocation(tempX, tempY, rowHeight, siteWidth, siteXpos, 
				siteYpos, numRows, numSitesInRow);
      if (debugMode) {
	cout << "Optimal location(L') for cell: " << (*thisCell).CellGetName() << "is found to be (" 
	     << siteXpos << "," << siteYpos << ")" << endl;
      }
      siteNum = siteXpos / siteWidth;
      rowNum = siteYpos / rowHeight;
      if ((siteNum == oldSiteNum) && (rowNum == oldRowNum)) {
	if (debugMode) {
	  cout << "OBSERVATION : (L' = L). Cell is already at its optimal location" << endl;
	}
	/* Reassign the site to the cell */
	(*thisSite).FDPSiteSetHasCell(thisCell);
	end_ripple = true;
	continue;
      }
      
      /* Get the site object associated with the cell */
      thisSite = gridMatrix[rowNum][siteNum];
      
      /*******************************************************************/
      /* Three different conditions are possible with the optimal location
	 1. LOCKED: In this case the cell under consideration
	    is placed at the nearest unlocked site in the grid(vacant 
	    or occupied). 
	      If the site is occupied, the cell belonging to that site is
	      selected next for movement.
	 2. OCCUPIED: In this case the selected cell is placed in the 
	    optimal location and the cell that was already present is 
	    selected next for movement.
	 3. VACANT: Cell is placed in its optimal location.
      */
      /*******************************************************************/
	 
      /* CASE : LOCKED */
      if ((*thisSite).FDPSiteGetIsLocked()) {
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
	    if (debugMode) {
	      cout << "Cell " << (*thisCell).CellGetName() << " is placed at (" << siteX << "," << siteY 
		   << ") " << "[ " << (*nearestSite).FDPSiteGetSiteNum() << "," << (*nearestSite).FDPSiteGetRowNum() 
		   << "]" << endl;
	    }
	    
	    if (nextCell != NIL(Cell*)) {
	      if (debugMode) {
		cout << "Nearest site found is occupied " << endl;
		cout << "Next cell to be placed " << (*nextCell).CellGetName() << endl;
	      }
	      end_ripple = false;
	      usedCells[thisCell] = true;
	      thisCell = nextCell;
	    }
	    if (nextCell == NIL(Cell*)) {
	      end_ripple = true;
	      usedCells[thisCell] = true;
	    } 
	    abort_count++;
	    FDPSite *prevSite = nearestSite;
	    if (abort_count > abort_limit) {
	      /* When there is still a cell left to be placed, as it was bumped out
		 of its site in the previous iteration, and the abort_count has crossed
		 abort limit then place the cell in nearest vacant location 
	      */
	      
	      if (nextCell != NIL(Cell*)) {
		nearestSite = FDPGetNearestVacantSite(prevSite, gridMatrix, numSitesInRow, numRows);
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
	      FDPClearAllLocks(allCells, gridMatrix, rowHeight, siteWidth);
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
	    if (debugMode) {
	      cout << "Cell " << (*thisCell).CellGetName() << " is placed at (" << siteX << "," << siteY 
		   << ") " << "[ " << (*nearestSite).FDPSiteGetSiteNum() << "," << (*nearestSite).FDPSiteGetRowNum() 
		   << "]" << endl;
	    }
	    end_ripple = true;
	    abort_count++;
	    if (abort_count > abort_limit) {
	      FDPClearAllLocks(allCells, gridMatrix, rowHeight, siteWidth);
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
	if (debugMode) {
	  cout << "Cell " << (*thisCell).CellGetName() << " is placed at (" << siteX << "," << siteY 
	       << ") " << "[ " << (*thisSite).FDPSiteGetSiteNum() << "," << (*thisSite).FDPSiteGetRowNum() 
	       << "]" << endl;
	}

	usedCells[thisCell] = true;
	if (nextCell != NIL(Cell*)) {
	  /* Indicates site is occupied. So choose the displaced cell to moved next */
	  if (debugMode) {
	    cout << "Next cell to be placed " << (*nextCell).CellGetName() << endl;
	  }
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

