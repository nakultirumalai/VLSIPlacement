# include <FDP.h>

unsigned int row_height, site_width;
bool fdpDebug = false;

FDPSite*
FDPGetSiteOfCell(vector<vector<FDPSite*> > &gridMatrix, Cell *thisCell, 
		 int siteWidth, int rowHeight)
{
  double cellXpos, cellYpos;
  int siteNum, rowNum;
  int cellWidth;
  FDPSite *thisSite;
  cellXpos = (*thisCell).x;
  cellYpos = (*thisCell).y;
  siteNum = cellXpos / siteWidth;
  rowNum = cellYpos / rowHeight;
  thisSite = gridMatrix[rowNum][siteNum];

  return (thisSite);
}

inline 
void printSite(FDPSite *site)
{
  uint siteNum, rowNum;
  
  siteNum = (*site).FDPSiteGetSiteNum();
  rowNum = (*site).FDPSiteGetRowNum();

  cout << "Visited:" << endl;
  cout << "  Site: " << siteNum << " Row: " << rowNum << endl;
}

inline
bool siteAvailableForPlacement(FDPSite *thisSite, bool vacantOnly)
{
  bool rtv;

  rtv = true;
  if ((*thisSite).FDPSiteGetIsLocked()) {
    rtv = false;
  } else if (vacantOnly) {
    if (!((*thisSite).FDPSiteIsVacant())) rtv = false;
  }

  return (rtv);
}

FDPSite*
FDPGetNearestVacantSite(FDPSite *thisSite, vector<vector<FDPSite*> > &gridMatrix, 
			int numSites, int numRows, bool vacantOnly)
{
  if (fdpDebug) 
    cout << " BEGIN: CALL TO GET NEAREST VACANT SITE: VACANT ONLY: " << vacantOnly << endl;
  queue<FDPSite*> myQueue;
  map<FDPSite*, bool> visitedSites;
  FDPSite *topSite, *selectedSite;
  uint siteNum, rowNum;
  uint nextSiteNum, nextRowNum;
  
  siteNum = (*thisSite).FDPSiteGetSiteNum();
  rowNum = (*thisSite).FDPSiteGetRowNum();
  topSite = thisSite;
  selectedSite = NIL(FDPSite *);
  do {
    visitedSites[topSite] = true;
    if (siteNum > 0) {
      nextSiteNum = siteNum - 1;
      nextRowNum = rowNum;
      if (fdpDebug) 
	cout << "EXPLORING NEIGHBORING SITE: (" << nextSiteNum << ", " << nextRowNum << ")" << endl;
      topSite = gridMatrix[nextRowNum][nextSiteNum];
      if (siteAvailableForPlacement(topSite, vacantOnly)) {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : YES " << endl;
	selectedSite = topSite;
	break;
      } else {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : NO " << endl;
	_KEY_DOES_NOT_EXIST(visitedSites, topSite) {
	  if (fdpDebug) 
	    cout << "\t ADDING TO QUEUE" << endl;
	  myQueue.push(topSite);
	  //	  printSite(topSite);
	} else {
	  if (fdpDebug) 
	    cout << "\t VISITED.. SKIP" << endl;
	}
      }
    } 
    if (siteNum < (numSites - 1)) {
      nextSiteNum = siteNum + 1;
      nextRowNum = rowNum;
      if (fdpDebug) 
	cout << "EXPLORING NEIGHBORING SITE: (" << nextSiteNum << ", " << nextRowNum << ")" << endl;
      topSite = gridMatrix[nextRowNum][nextSiteNum];
      if (siteAvailableForPlacement(topSite, vacantOnly)) {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : YES " << endl;
	selectedSite = topSite;
	break;
      } else {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : NO" << endl;
	_KEY_DOES_NOT_EXIST(visitedSites, topSite) {
	  if (fdpDebug) 
	    cout << "\t ADDING TO QUEUE" << endl;
	  myQueue.push(topSite);
	  //	  printSite(topSite);
	} else {
	  if (fdpDebug) 
	    cout << "\t VISITED.. SKIP" << endl;
	}
      }
    }
    if (rowNum > 0) {
      nextSiteNum = siteNum;
      nextRowNum = rowNum - 1;
      topSite = gridMatrix[nextRowNum][nextSiteNum];
      if (fdpDebug) 
	cout << "EXPLORING NEIGHBORING SITE: (" << nextSiteNum << ", " << nextRowNum << ")" << endl;
      if (siteAvailableForPlacement(topSite, vacantOnly)) {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : YES " << endl;
	selectedSite = topSite;
	break;
      } else {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : NO" << endl;
	_KEY_DOES_NOT_EXIST(visitedSites, topSite) {
	  if (fdpDebug) 
	    cout << "\t ADDING TO QUEUE" << endl;
	  myQueue.push(topSite);
	  //	  printSite(topSite);
	} else {
	  if (fdpDebug) 
	    cout << "\t VISITED.. SKIP" << endl;
	}
      }
    } 
    if (rowNum < (numRows - 1)) {
      nextSiteNum = siteNum;
      nextRowNum = rowNum + 1;
      topSite = gridMatrix[nextRowNum][nextSiteNum];
      if (fdpDebug) 
	cout << "EXPLORING NEIGHBORING SITE: (" << nextSiteNum << ", " << nextRowNum << ")" << endl;
      if (siteAvailableForPlacement(topSite, vacantOnly)) {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : YES " << endl;
	selectedSite = topSite;
	break;
      } else {
	if (fdpDebug) 
	  cout << "\t SITE (" << nextSiteNum << ", " << nextRowNum << ") : NO" << endl;
	_KEY_DOES_NOT_EXIST(visitedSites, topSite) {
	  if (fdpDebug) 
	    cout << "\t ADDING TO QUEUE" << endl;
	  myQueue.push(topSite);
	  //	  printSite(topSite);
	} else {
	  if (fdpDebug) 
	    cout << "\t VISITED.. SKIP" << endl;
	}
      }
    }
    topSite = myQueue.front(); myQueue.pop();
    siteNum = (*topSite).FDPSiteGetSiteNum();
    rowNum = (*topSite).FDPSiteGetRowNum();
    if (fdpDebug) 
      cout << "POPPED SITE: (" << siteNum << ", " << rowNum << ")" 
	   << "QUEUE SIZE: " << myQueue.size() << endl;
  } while (!myQueue.empty());

  if (fdpDebug) 
    cout << " END: CALL TO GET NEAREST VACANT SITE: VACANT ONLY: " << vacantOnly << endl;
  if (selectedSite == NIL(FDPSite *)) {
    _ASSERT_TRUE("Error: Finding a site for a cell failed");
  } else {
    if (fdpDebug) 
      cout << "RETURNING SITE: (" << (*selectedSite).FDPSiteGetSiteNum() 
	   << ", " << (*selectedSite).FDPSiteGetRowNum() << ")" << endl;
  }

  return (selectedSite);
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
      if ((*thisSite).FDPSiteIsVacant()) {
	cout << "[" << (*thisSite).FDPSiteGetSiteNum() << "," 
	     << (*thisSite).FDPSiteGetRowNum() << "]" << endl;
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
FDPFixCellsInSites(vector<vector<FDPSite*> > &gridMatrix,
		   vector<Cell *> &allCells, uint siteWidth, 
		   uint rowHeight) 
{
  Cell *thisCell;
  FDPSite *thisSite; 
  double siteNum, rowNum;
  double cellXpos, cellYpos;

  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, thisCell) {
    cellXpos = (*thisCell).CellGetXpos();
    cellYpos = (*thisCell).CellGetYpos();
    siteNum = ((double)cellXpos) / siteWidth;
    rowNum = ((double)cellYpos) / rowHeight;
    thisSite = gridMatrix[rowNum][siteNum];
    (*thisSite).FDPSiteSetCell(thisCell);
  } END_FOR;

  if (fdpDebug) {
    cout << "FDP DEBUG: Fixed " << allCells.size() 
	 << " cells in their respective sites" << endl;
  }
}

Cell*
FDPGetNextCellToPlace(vector<Cell *> &allCells, 
		      map<Cell *, bool> &usedCells, 
		      uint &counter)
{
  Cell *thisCell, *selectedCell;
  uint sizeOfVector;

  thisCell = NIL(Cell *);
  selectedCell = NIL(Cell *);
  sizeOfVector = allCells.size();
  /* No sorting is done based on connectivity */
  while (1) {
    if (counter == sizeOfVector) {
      break;
    }
    thisCell = allCells[counter++];
    _KEY_EXISTS(usedCells, thisCell) {
      if (fdpDebug) {
	cout << "Cell " << (*thisCell).CellGetName() 
	     << " had already been selected earlier. " << endl;
      }
      continue;
    } else {
      usedCells[thisCell] = true;
      if (fdpDebug) {
	cout << "FDP_DEBUG: SELECTED CELL: " << (*thisCell).CellGetName() << endl;
      }
      selectedCell = thisCell;
      break;
    }
  }
  
  return (selectedCell);
}

void
FDPCreateFDPNetlist(vector<Cell*> &allCells, map<Cell*, vector<FDPNet*> > &fdpNetList)
{
  FDPNet *fdpNet;
  Cell *cellPtr, *connCellPtr;
  Net *netPtr;
  //  map<Cell *, bool> visitedCells;
  map<Cell *, vector<FDPNet*> >::iterator fdpNetListIter;
  double netWeight;

  cout << endl;
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, cellPtr) {
    map<Cell*, double> connectedCellMap;
    map<Cell*, double>::iterator connectedCellMapItr;
    map<Net *, bool> visitedNets;
    CELL_FOR_ALL_NETS_NO_DIR((*cellPtr), netPtr) {
      netWeight = (*netPtr).NetGetWeight();
      _KEY_EXISTS(visitedNets, netPtr) {
	continue;
      } else {
	visitedNets[netPtr] = true;
      } 
      NET_FOR_ALL_CELLS((*netPtr), connCellPtr) {
	if (connCellPtr == cellPtr) {
	  continue;
	}
	_KEY_EXISTS_WITH_VAL(connectedCellMap, connCellPtr, connectedCellMapItr) {
	  connectedCellMapItr->second += netWeight;
	} else {
	  connectedCellMap[connCellPtr] = netWeight;
	}
      } NET_END_FOR;
    } CELL_END_FOR;
    MAP_FOR_ALL_ELEMS(connectedCellMap, Cell*, double, connCellPtr, netWeight) {
      fdpNet = new FDPNet(cellPtr, connCellPtr, netWeight);
      _KEY_EXISTS_WITH_VAL(fdpNetList, cellPtr, fdpNetListIter) {
	vector<FDPNet*> &netList = fdpNetListIter->second;
	netList.push_back(fdpNet);
      } else {
	vector<FDPNet*> netList;
	netList.push_back(fdpNet);
	fdpNetList[cellPtr] = netList;
      }
    } END_FOR;
    cout << "." << flush;
  } END_FOR;
  cout << endl;
}

void
FDPGetOptLocationForCell(Cell *thisCell, uint siteWidth, uint rowHeight,
			 uint numSitesInRow, uint numRows, uint &siteNum, uint &rowNum)
{
  Net *netPtr;
  Pin *pinPtr;
  Cell *nextCell;
  double sumWx, sumWy, sumW;
  double cellXpos, cellYpos;
  double netWeight;

  cellXpos = (*thisCell).CellGetXpos();
  cellYpos = (*thisCell).CellGetYpos();
  if (fdpDebug) {
    cout << endl << "FDP DEBUG: Cell: " << (*thisCell).CellGetName() 
	 << " Old position: (" << cellXpos
	 << ", " << cellYpos << ")" << endl
	 << " Site : " << (cellXpos / siteWidth) 
	 << " Row  : " << (cellYpos / rowHeight) << endl;
  }

  sumWx = 0; sumWy = 0; sumW = 0;
  CELL_FOR_ALL_NETS_NO_DIR((*thisCell), netPtr) {
    netWeight = (*netPtr).NetGetWeight();
    NET_FOR_ALL_PINS((*netPtr), pinPtr) {
      nextCell = (*pinPtr).ParentCell;
      if (nextCell == thisCell) {
	continue;
      }
      cellXpos = (*nextCell).x;
      cellYpos = (*nextCell).y;
      sumWx += (netWeight * cellXpos);
      sumWy += (netWeight * cellYpos);
      sumW += netWeight;
    } NET_END_FOR;
  } CELL_END_FOR;

# if 0
  vector<Pin*> &CellPins = CellObj.CellGetPins();
  uint numPins = CellPins.size();
  Pin *pinPtr1, *pinPtr2;
  Net *netPtr1, *netPtr2;
  for (int n = 0; n < numPins; n++) {               
    pinPtr1 = CellPins[n];
    netPtr1 = (*pinPtr1).ConnectedNet;
    if (n < (numPins - 1)) {
      pinPtr2 = CellPins[n + 1];
      netPtr2 = (*pinPtr).ConnectedNet;
    }
  }
# endif 
  sumWx /= sumW;
  sumWy /= sumW;

  /* Optimal rounded off value for X */
  siteNum = dtoi(sumWx / siteWidth);
  if (siteNum == numSitesInRow) {
    siteNum -= 1;
  }

  /* Optimal rounded off value for Y */
  rowNum = dtoi(sumWy / rowHeight);
  if (rowNum == numRows) {
    rowNum -= 1;
  }

  if (fdpDebug) {
    cout <<  "FDP DEBUG: Cell: " << (*thisCell).CellGetName() 
	 << " Optimal position: (" << sumWx
	 << ", " << sumWy << ")" << endl
	 << " Site : " << siteNum
	 << " Row  : " << rowNum << endl;
  }
}

void
FDPGetOptLocationForCellUsingFDPNetList(Cell *thisCell, map<Cell *, vector<FDPNet*> > &fdpNetList,
					uint siteWidth, uint rowHeight,
					uint numSitesInRow, uint numRows, 
					uint &siteNum, uint &rowNum)
{
  FDPNet *fdpNet;
  Net *netPtr;
  Pin *pinPtr;
  Cell *connCell;
  double sumWx, sumWy, sumW;
  double cellXpos, cellYpos;
  double netWeight;

  cellXpos = (*thisCell).CellGetXpos();
  cellYpos = (*thisCell).CellGetYpos();
  _KEY_DOES_NOT_EXIST(fdpNetList, thisCell) {
    _ASSERT_TRUE("Error: FDP netlist passed does not contain cell pointer");
  }
  if (fdpDebug) {
    cout << endl << "FDP DEBUG: Cell: " << (*thisCell).CellGetName() 
	 << " Old position: (" << cellXpos
	 << ", " << cellYpos << ")" << endl
	 << " Site : " << (cellXpos / siteWidth) 
	 << " Row  : " << (cellYpos / rowHeight) << endl;
  }
  vector<FDPNet*> &fdpNets = fdpNetList[thisCell];
  sumWx = 0; sumWy = 0; sumW = 0;
  VECTOR_FOR_ALL_ELEMS(fdpNets, FDPNet*, fdpNet) {
    connCell = (*fdpNet).FDPNetGetOtherCell(thisCell);
    netWeight = (*fdpNet).FDPNetGetWeight();
    cellXpos = (*connCell).x;
    cellYpos = (*connCell).y;
    sumWx += (netWeight * cellXpos);
    sumWy += (netWeight * cellYpos);
    sumW += netWeight;
  } END_FOR;
  sumWx /= sumW;
  sumWy /= sumW;

  /* Optimal rounded off value for X */
  siteNum = dtoi(sumWx / siteWidth);
  if (siteNum == numSitesInRow) {
    siteNum -= 1;
  }

  /* Optimal rounded off value for Y */
  rowNum = dtoi(sumWy / rowHeight);
  if (rowNum == numRows) {
    rowNum -= 1;
  }

  if (fdpDebug) {
    cout <<  "FDP DEBUG: Cell: " << (*thisCell).CellGetName() 
	 << " Optimal position: (" << sumWx
	 << ", " << sumWy << ")" << endl
	 << " Site : " << siteNum
	 << " Row  : " << rowNum << endl;
  }
}

void
FDPPlaceAndLockCellInSite(FDPSite *thisSite, Cell *thisCell)
{
  uint siteX, siteY;
  if (fdpDebug) {
    cout << "FDP DEBUG: Cell: " << (*thisCell).CellGetName() 
	 << " Old position: (" << (*thisCell).CellGetXpos() 
	 << ", " << (*thisCell).CellGetYpos() << ")" << endl;
  }
  (*thisSite).FDPSiteSetCell(thisCell);
  (*thisSite).FDPSiteSetIsLocked(true);
  siteX = (*thisSite).FDPSiteGetXpos();
  siteY = (*thisSite).FDPSiteGetYpos();
  (*thisCell).CellSetXpos(siteX);
  (*thisCell).CellSetYpos(siteY);

  if (fdpDebug) {
    cout <<  "FDP DEBUG: Placed and locked Cell: " << (*thisCell).CellGetName() 
	 << " in position : (" << siteX
	 << ", " << siteY << ")" << endl
	 << " Site : " << (siteX / site_width) 
	 << " Row  : " << (siteY / row_height) 
	 << endl;
  }
}

void
FDPCreateGrid(vector<vector<FDPSite*> > &gridMatrix, vector<uint> &rowCap,
	      uint numSitesInRow, uint numRows, uint siteWidth, uint rowHeight)
{
  vector<FDPSite*> oneRow;
  FDPSite *thisSite;
  uint i, j, rowCapInRow;
  uint siteXpos, siteYpos;
  uint siteCount;

  rowCapInRow = numSitesInRow * siteWidth;
  siteCount = 0;
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
  if (fdpDebug) {
    cout << "FDP DEBUG: Created grid with " << (siteCount + 1) 
	 << " sites" << endl;
  }
}

inline
void saveBestCellPositions(vector<Cell *> &allCells, vector<uint> &cellXPositions, 
			   vector<uint> &cellYPositions)
{
  Cell *thisCell;
  uint numCells;
  uint idx;
  
  numCells = allCells.size();
  for (idx = 0; idx < numCells; idx++) {
    thisCell = allCells[idx];
    cellXPositions[idx] = thisCell->x;
    cellYPositions[idx] = thisCell->y;
  } 
}

inline
void restoreBestCellPositions(vector<Cell *> &allCells, vector<uint> &cellXPositions, 
			      vector<uint> &cellYPositions)
{
  Cell *thisCell;
  uint numCells;
  uint idx;
  
  numCells = allCells.size();
  for (idx = 0; idx < numCells; idx++) {
    thisCell = allCells[idx];
    thisCell->x = cellXPositions[idx];
    thisCell->y = cellYPositions[idx];
  } 
}

/* Top level function to place the cells using a force-directed heuristic */
void 
FDPTopLevel(Design &myDesign, vector<Cell*> &allCells, uint numRows, uint numSitesInRow, 
	    uint rowHeight, uint siteWidth, uint &iterLimit, uint &abortLimit)
{
  Cell *thisCell, *nextCell;
  Net *NetPtr;
  FDPSite *nearestSite, *prevSite;
  FDPSite *cellSite, *newSite;
  map<Cell*, bool> usedCells;
  vector<vector<FDPSite*> > gridMatrix;
  map<Cell*, vector<FDPNet*> > fdpNetList;
  vector<uint> rowCap;
  uint siteNum, rowNum, siteCount, stepCount;
  uint oldSiteNum, oldRowNum;
  uint iterCount, abortCount;
  uint counter, numCells;
  ulong HPWL, prevHPWL, bestHPWL;
  ulong xHPWL, yHPWL;
  ulong prevHPWL1, prevHPWL2, prevHPWL3;
  double stepTime;
  bool endRipple, iterationComplete;
  bool useFDPNetlist, useWeightedHPWL;
  Env &DesignEnv = myDesign.DesignGetEnv();
  useFDPNetlist = true;
  numCells = allCells.size();
  vector<uint> cellXPositions(numCells);
  vector<uint> cellYPositions(numCells);

  if (numCells == 0) {
    _ASSERT_TRUE(" FD-SOLVER: NO CELLS PROVIDED");
  }

  cout << "Invoking force directed solver for : " << iterLimit 
       << " iterations with abort limit: " << abortLimit << endl;
  cout << "BEGIN: CPU TIME: " << getCPUTime() << "s" << endl;
  HPWL = 0;
  prevHPWL = 0; prevHPWL1 = 0; prevHPWL2 = 0; prevHPWL3 = 0;
  useWeightedHPWL = DesignEnv.EnvGetUseWeightedHPWL();
  myDesign.DesignComputeHPWL();
  bestHPWL = myDesign.DesignGetHPWL();
  if (useWeightedHPWL) {
    myDesign.DesignComputeWtHPWL(xHPWL, yHPWL, bestHPWL);
  } 
  row_height = rowHeight;
  site_width = siteWidth;
  stepTime = getCPUTime();
  if (useFDPNetlist) {
    FDPCreateFDPNetlist(allCells, fdpNetList);
  }
  stepTime = getCPUTime() - stepTime;
  cout << "Created intermediate netlist for force-directed solver" << endl;
  DesignEnv.EnvRecordFDNetlistBuildTime(stepTime);
  cout << "Running force-directed solver" << endl;
  stepTime = getCPUTime();
  FDPCreateGrid(gridMatrix, rowCap, numSitesInRow, numRows, siteWidth, rowHeight);
  FDPFixCellsInSites(gridMatrix, allCells, siteWidth, rowHeight);
  iterCount = 0; 

  /* MAIN ALGORITHM FORCE DIRECTED PLACEMENT STARTS HERE */
  uint dotLimit = 75;
  uint dotCount = 0;
  while(iterCount < iterLimit) {
    iterationComplete = false;
    abortCount = 0;  
    prevSite = NIL(FDPSite *);
    for (counter = 0; counter < numCells; counter++) {
      thisCell = FDPGetNextCellToPlace(allCells, usedCells, counter);
      if (thisCell == NIL(Cell *)) break;

      cellSite = FDPGetSiteOfCell(gridMatrix, thisCell, siteWidth, rowHeight);
      oldSiteNum = (*cellSite).FDPSiteGetSiteNum();
      oldRowNum = (*cellSite).FDPSiteGetRowNum();
      if (useFDPNetlist) {
	FDPGetOptLocationForCellUsingFDPNetList(thisCell, fdpNetList, siteWidth,
						rowHeight, numSitesInRow, numRows, siteNum,
						rowNum);
      } else {
	FDPGetOptLocationForCell(thisCell, siteWidth, rowHeight, numSitesInRow, numRows, 
				 siteNum, rowNum);
      }
      /* This is the optimal site for the cell */
      newSite = gridMatrix[rowNum][siteNum];
      if (cellSite == newSite) {
	if (fdpDebug) {
	  cout << "FDP DEBUG: Not moving cell " << (*thisCell).CellGetName() 
	       << " as it has the same new position " << endl;
	}
	continue;
      }
      endRipple = false;
      (*cellSite).FDPSiteRemoveCell();
      while(!endRipple) {
	if ((*newSite).FDPSiteGetIsLocked()) {
	  /* LOCKED CASE */
	  if (fdpDebug) cout << "LOCKED CASE" << endl;
	  abortCount++; nextCell = NIL(Cell *);
	  if (abortCount == abortLimit) {
	    /* FIND THE NEAREST VACANT SITE */
	    newSite = FDPGetNearestVacantSite(prevSite, gridMatrix, numSitesInRow, numRows, true);
	    iterationComplete = true;
	  } else {
	    /* FIND THE NEAREST VACANT / OCCUPIED SITE */
	    newSite = FDPGetNearestVacantSite(newSite, gridMatrix, numSitesInRow, numRows, false);
	    //	    cout << "OBTAINED NEAREST VACANT SITE:" << endl;
	    //	    printSite(newSite);
	    nextCell = (*newSite).FDPSiteGetCell();
	  }
	  prevSite = newSite;
	  FDPPlaceAndLockCellInSite(newSite, thisCell);
	  if (nextCell == NIL(Cell *)) {
	    if (fdpDebug) cout << "END RIPPLE" << endl;
	    endRipple = true;
	  } else {
	    thisCell = nextCell;
	    if (fdpDebug) {
	      cout << " Nearest site occupied by cell:  " << (*nextCell).CellGetName() << endl;
	    }
	    usedCells[thisCell] = true;
	  } 
	} else {
	  /* OCCUPIED / VACANT CASE */
	  if (fdpDebug) cout << "OCCUPIED CASE" << endl;
	  abortCount = 0;
	  if ((*newSite).FDPSiteIsVacant()) {
	    FDPPlaceAndLockCellInSite(newSite, thisCell);
	    if (fdpDebug) cout << "END RIPPLE" << endl;
	    endRipple = true;
	  } else {
	    nextCell = (*newSite).FDPSiteGetCell();
	    if (fdpDebug) {
	      cout << " Obtained next cell: " << (*nextCell).CellGetName() << endl;
	    }
	    FDPPlaceAndLockCellInSite(newSite, thisCell);
	    if (nextCell == NIL(Cell*)) {
	      _ASSERT_TRUE("Error: No cell obtained from an occupied site");
	    }
	    thisCell = nextCell;
	    usedCells[thisCell] = true;
	  }
	}
	if (iterationComplete) { 
	  break;
	} else if (!endRipple) {
	  if (useFDPNetlist) {
	    FDPGetOptLocationForCellUsingFDPNetList(thisCell, fdpNetList, siteWidth,
						    rowHeight, numSitesInRow, numRows, siteNum,
						    rowNum);
	  } else {
	    FDPGetOptLocationForCell(thisCell, siteWidth, rowHeight, numSitesInRow, numRows, 
				     siteNum, rowNum);
	  }
	  newSite = gridMatrix[rowNum][siteNum];
	}
      }
      if (iterationComplete) break;
    }
    FDPClearAllLocks(allCells, gridMatrix, rowHeight, siteWidth);
    usedCells.clear();
    myDesign.DesignComputeHPWL();
    HPWL = (ulong)myDesign.DesignGetHPWL();
    if (useWeightedHPWL) {
      myDesign.DesignComputeWtHPWL(xHPWL, yHPWL, HPWL);
    }
    if (prevHPWL3 != 0 && prevHPWL2 != 0 && prevHPWL1 != 0) {
      if (HPWL == prevHPWL3 && prevHPWL3 == prevHPWL2 && prevHPWL2 == prevHPWL1) {
	cout << endl << "ITER COUNT: " << iterCount << " HPWL: " << HPWL << endl;
	break;
      }
    }
    if (HPWL < bestHPWL) {
      bestHPWL = HPWL;
      cout << "*" << flush;
      cout << "\nITER COUNT: " << iterCount << " HPWL: " << HPWL << " CPU: " << getCPUTime() << endl;
      dotCount = 0;
      saveBestCellPositions(allCells, cellXPositions, cellYPositions);
    }

    dotCount++;
    if (dotCount == dotLimit) {
      dotCount = 0;
      cout << endl;
    } else {
      cout << "." << flush;
    }
    prevHPWL3 = prevHPWL2; prevHPWL2 = prevHPWL1; prevHPWL1 = HPWL;
    iterCount++;
  }
  restoreBestCellPositions(allCells, cellXPositions, cellYPositions);
  stepTime = getCPUTime() - stepTime;
  DesignEnv.EnvRecordFDSolverTime(stepTime);
  cout << endl;
}

