# include <Design.h>

/* Function to plot the cluster. This will generate two plots.
   The first plot will be an internal view of how the boundary cells
   have been arranged in the cluster. Pins will be plotted with 
   offset here for each cell and boundary pins will be highlighted.
   The second plot will give only the cluster cell and the boundary
   pins */
void
Design::DesignPlotCluster(string plotTitle, string plotFileName,
			  Cell *clusterCell, vector<Cell *> &boundaryCells,
			  vector<uint> &rowNum, vector<uint> &xPosInRow,
			  vector<Pin*> &clusterPins)
{
  Cell *cellPtr;
  Pin *pinPtr;
  Plot clusterPlot(plotTitle, plotFileName);
  uint numBoundaryCells, idx;
  uint numClusterPins;
  uint minx, miny, maxx, maxy;
  uint cellXpos, cellYpos;
  uint pinAbsX, pinAbsY;
  uint rowHeight;

  minx = 0; miny = 0; 
  maxx = (*clusterCell).CellGetWidth(); maxy = (*clusterCell).CellGetHeight();

  /* Set the boundary of the plot region */
  clusterPlot.PlotSetBoundary(minx, miny, maxx, maxy);
  numBoundaryCells = boundaryCells.size();
  rowHeight = DesignGetSingleRowHeight();
  for (idx = 0; idx < numBoundaryCells; idx++) {
    cellPtr = boundaryCells[idx];
    cellXpos = xPosInRow[idx];
    cellYpos = rowNum[idx] * rowHeight;
    (*cellPtr).CellSetXpos(cellXpos);
    (*cellPtr).CellSetYpos(cellYpos);
    clusterPlot.PlotAddCell(*cellPtr);
    CELL_FOR_ALL_PINS((*cellPtr), PIN_DIR_ALL, pinPtr) {
      clusterPlot.PlotAddPin(*pinPtr);
    } CELL_END_FOR;
  }

  /* Plot the highlighted pins */
  numClusterPins = clusterPins.size();
  for (idx = 0; idx < numClusterPins; idx++) {
    pinPtr = clusterPins[idx];
    clusterPlot.PlotAddHighlightedPin(*pinPtr);
  }

  clusterPlot.PlotWriteOutput();
}

/* Given a set of cells, decide what the initial height 
   and width of the cluster should be considered */
void
Design::DesignGetClusterDimensions(vector<Cell *> &listOfCells, 
				   uint &resultWidth, uint &resultHeight)
{
  Cell *cellPtr;
  uint minHeight, minWidth, maxHeight, maxWidth;
  uint cellHeight, cellWidth;
  uint totalHeight, totalWidth;
  uint numRows;
  bool discreteClusterHeight;
  bool discreteClusterWidth;
  double totalArea, rowHeight;
  double multiple;

  Env &DesignEnv = DesignGetEnv();
  /* Assuming that withGeometry is off here */
  /* Assuming aspect ratio is 1.0 */
  /* Some rules:                                           */
  /* Minimum height of the cluster should be equal to the  */
  /* max height among all cells                            */
  /* Minimum width of the cluster should be equal to the   */
  /* max width among all cells                             */
  discreteClusterHeight = DesignEnv.EnvGetDiscreteHeight();
  discreteClusterWidth = DesignEnv.EnvGetDiscreteWidth();
  numRows = DesignEnv.EnvGetClusterNumRows();
  rowHeight = DesignGetSingleRowHeight();
  minHeight = INT_MAX; minWidth = INT_MAX;
  maxHeight = 0; maxWidth = 0; totalWidth = 0; totalHeight = 0;
  totalArea = 0.0;
  VECTOR_FOR_ALL_ELEMS(listOfCells, Cell *, cellPtr) {
    cellHeight = (*cellPtr).CellGetHeight();
    cellWidth = (*cellPtr).CellGetWidth();
    if (cellHeight < minHeight) {
      minHeight = cellHeight;
    }
    if (cellHeight > maxHeight) {
      maxHeight = cellHeight;
    }
    if (cellWidth < minWidth) {
      minWidth = cellWidth;
    }
    if (cellWidth > maxWidth) {
      maxWidth = cellWidth;
    }
    totalWidth += cellWidth;
    totalHeight += cellHeight;
    totalArea += (cellWidth * cellHeight);
  } END_FOR;
  /* Inflate total area by 10% to incorporate 
     white space */
  //  totalArea += 0.1 * totalArea;
  if (numRows == 0) {
    resultWidth = (unsigned int)ceil(sqrt(totalArea));
    if (resultWidth < maxWidth) {
      resultWidth = maxWidth;
    }
    resultHeight = (unsigned int)ceil(sqrt(totalArea));
    if (resultHeight < maxHeight) {
      resultHeight = maxHeight;
    }
    /* Make result height a multiple of row height in the 
       design and width a multiple of row width*/
    if (discreteClusterHeight) {
      multiple = ceil(resultHeight / rowHeight);
      resultHeight = rowHeight * multiple;
    }
  } else if (numRows > 0) {
    resultHeight = numRows * rowHeight;
    resultWidth = floor(totalArea / resultHeight);
  }
}

void
Design::DesignAssignPinOffSets(Cell *clusterCell,
			       map<Cell *, uint> &boundaryCells, 
			       vector<Pin *> &clusterCellPins, 
			       map<Pin *, Pin*> &pinMap,
			       vector<uint> &rowNum, 
			       vector<uint> &xPosInRow)
{
  Cell *cellOfPin;
  Pin *pinPtr, *cellPin;
  map<Cell *, uint>::iterator itr;
  uint rowNumber, xPos;
  uint index, rowHeight;
  uint pinXOffset, pinYOffset;
  uint absOffset;
  uint maxx, maxy;

  maxx = (*clusterCell).CellGetWidth(); maxy = (*clusterCell).CellGetHeight();
  rowHeight = (double)DesignGetSingleRowHeight();
  VECTOR_FOR_ALL_ELEMS(clusterCellPins, Pin *, pinPtr) {
    cellPin = pinMap[pinPtr];
    cellOfPin = &(*cellPin).PinGetParentCell();
    index = 0;
    _KEY_EXISTS_WITH_VAL(boundaryCells, cellOfPin, itr) {
      index = itr->second;
    } else {
      cout << "Error: Cluster pin does not have its cell in the " 
	   << "boundary cell map!" << endl;
      exit(0);
    }
    rowNumber = rowNum[index];
    xPos = xPosInRow[index];
    pinXOffset = (*cellPin).PinGetXOffset();
    pinYOffset = (*cellPin).PinGetYOffset();

    absOffset = rowNumber * rowHeight + pinYOffset;
    if (absOffset > maxy) {
      cout << "For cell : " << (*cellOfPin).CellGetName() << endl;
      _ASSERT_TRUE("Error: Pin y-offset out of cluster bounds");
    }
    (*pinPtr).PinSetYOffset(absOffset);
    absOffset = xPos + pinXOffset;
    if (absOffset > maxx) {
      cout << "For cell : " << (*cellOfPin).CellGetName() << endl;
      _ASSERT_TRUE("Error: Pin x-offset out of cluster bounds");
    }
    (*pinPtr).PinSetXOffset(absOffset);
  } END_FOR;
}

void
Design::DesignAssignPinOffSetsInRows(Cell *clusterCell, vector<Pin *> &clusterCellPins,
				     map<Pin *, Pin*> &pinMap)
{
  Cell *cellOfPin;
  Pin *pinPtr, *cellPin;
  uint pinXOffset, pinYOffset;
  uint absOffset;
  uint maxx, maxy;

  maxx = (*clusterCell).CellGetWidth(); maxy = (*clusterCell).CellGetHeight();
  VECTOR_FOR_ALL_ELEMS(clusterCellPins, Pin *, pinPtr) {
    cellPin = pinMap[pinPtr];
    cellOfPin = &(*cellPin).PinGetParentCell();
    pinXOffset = (*cellPin).PinGetXOffset();
    pinYOffset = (*cellPin).PinGetYOffset();
    absOffset = pinYOffset + (*cellOfPin).CellGetYposDbl();
    if (absOffset > maxy) {
      cout << "For cell : " << (*cellOfPin).CellGetName() << endl;
      _ASSERT_TRUE("Error: Pin y-offset out of cluster bounds");
    }
    absOffset = pinXOffset + (*cellOfPin).CellGetXposDbl();
    if (absOffset > maxx) {
      cout << "For cell : " << (*cellOfPin).CellGetName() << endl;
      _ASSERT_TRUE("Error: Pin x-offset out of cluster bounds");
    }
    (*pinPtr).PinSetXOffset(absOffset);
  } END_FOR;
}

/* Proportionally expand a cluster if cells can be placed */
void
Design::DesignExpandCluster(Cell *clusterCell, uint &resultHeight, 
			    uint &resultWidth)
{
  uint resultHeightNew;
  double aspectRatio;
  double rowHeight;
  double resultWidthNew;
  /* Add a row and expand the width so that the aspect ratio is the same */
  /* Height can only be expanded at a discrete rate of number of rows */
  rowHeight = (double)DesignGetSingleRowHeight();
  resultHeightNew = resultHeight + rowHeight;
  
  aspectRatio = ((double)resultHeight) / resultWidth;
  resultWidthNew = ceil(resultHeightNew / aspectRatio);
  
  resultWidth = (uint)resultWidthNew;
  resultHeight = (uint)resultHeightNew;
  
  (*clusterCell).CellSetHeight(resultHeight);
  (*clusterCell).CellSetWidth(resultWidth);
}

/* Places all boundary cells at the center of the cluster without
   regard for legality of the cells */
void
Design::DesignPlaceCellsInClusterInCenter(vector<Cell *> &boundaryCellVec, 
					  vector<Pin *> &clusterCellPins, 
					  vector<uint> &rowNum, vector<uint> &xPosInRow,
					  uint resultHeight, uint resultWidth)
{
  double rowHeight;
  uint numRows, midRow;
  uint xPos;
  uint i;

  uint numBoundaryCells = boundaryCellVec.size();
  rowHeight = (double)DesignGetSingleRowHeight();

  /* Compute the number of rows */
  numRows = resultHeight / rowHeight;

  midRow = numRows / 2;
  for (i = 0; i < numBoundaryCells; i++) {
    Cell &thisCell = *(Cell *)boundaryCellVec[i];
    xPos = (resultWidth - thisCell.CellGetWidth()) / 2 ;
    rowNum[i] = midRow;
    xPosInRow[i] = xPos;
  }
}

bool
Design::DesignPlaceCellsInClusterNoLegal(vector<Cell *> &boundaryCellVec, 
					 vector<uint> &rowNum, vector<uint> &xPosInRow,
					 uint resultHeight, uint resultWidth)
{
  Cell *cellPtr;
  uint rowHeight;
  int cellWidth, i, idx;
  uint numRows, firstRow, lastRow;
  int remWidthFirstRow, remWidthLastRow;
  uint currMiddleRow;
  uint firstRowPrevX, lastRowPrevX;
  int rowLeftLast, rowRightLast;
  uint numBoundaryCells = boundaryCellVec.size();
  vector<int> rowCapacity;
  vector<uint> rowLeftX;
  vector<uint> rowRightX;
  vector<bool> rowLeftPlaced;
  bool cellPlaced;

  /* Divide the height into rows */
  rowHeight = (double)DesignGetSingleRowHeight();
  
  /* Compute the number of rows */
  numRows = resultHeight / rowHeight;

  /* Initialize all capacities to resultWidth */
  for (i = 0; i < numRows; i++) {
    rowLeftX.push_back(0);
    rowRightX.push_back(resultWidth);
    rowCapacity.push_back(resultWidth);
    rowLeftPlaced.push_back(false);
  }

  cellPlaced = true;
  /* Assign row number and x-position to each cell */
  firstRow = 0; lastRow = numRows - 1; firstRowPrevX = 0; lastRowPrevX = 0;
  remWidthFirstRow = resultWidth; remWidthLastRow = resultWidth;
  for (i = 0; i < numBoundaryCells; i++) {
    Cell &thisCell = *(Cell *)boundaryCellVec[i];
    cellWidth = thisCell.CellGetWidth();
    cellPlaced = false;
    /* Try to place cells in first row until the entire width 
       is taken: Allow overlaps */
    if (remWidthFirstRow > 0) {
      rowNum[i] = 0;
      if ((firstRowPrevX + cellWidth) > resultWidth) {
	xPosInRow[i] = resultWidth - cellWidth;
      } else {
	xPosInRow[i] = firstRowPrevX;
      }
      firstRowPrevX += cellWidth;
      remWidthFirstRow -= cellWidth;
      rowCapacity[firstRow] -= cellWidth;
      cellPlaced = true;
      thisCell.CellSetXpos(xPosInRow[i]);
      thisCell.CellSetYpos((rowNum[i] * rowHeight));
      //      cout << "PLC: Placed boundary cell " << thisCell.CellGetName() 
      //	   << " at " << xPosInRow[i] 
      //	   << " in row " << rowNum[i] 
      //	   << endl;
      continue;
    }
    /* Try to place cells in last row until the entire width 
       is taken: Allow overlaps */
    if (remWidthLastRow > 0) {
      rowNum[i] = lastRow;
      if ((lastRowPrevX + cellWidth) > resultWidth) {
	xPosInRow[i] = resultWidth - cellWidth;
      } else {
	xPosInRow[i] = lastRowPrevX;
      }
      lastRowPrevX += cellWidth;
      remWidthLastRow -= cellWidth;
      rowCapacity[lastRow] -= cellWidth;
      cellPlaced = true;
      thisCell.CellSetXpos(xPosInRow[i]);
      thisCell.CellSetYpos((rowNum[i] * rowHeight));
      //      cout << "PLC: Placed boundary cell " << thisCell.CellGetName() 
      //	   << " at " << xPosInRow[i] 
      //	   << " in row " << rowNum[i] 
      //	   << endl;
      continue;
    }
    /* For middle rows, place cells on the sides along the vertical boundaries */
    currMiddleRow = firstRow + 1;
    cellPlaced = false;
    while (currMiddleRow != lastRow) {
      rowLeftLast = rowLeftX[currMiddleRow];
      rowRightLast = rowRightX[currMiddleRow];
      if (rowLeftPlaced[currMiddleRow]) {
	if (rowRightLast - cellWidth > rowLeftLast) {
	  rowRightLast -= cellWidth;
	  rowNum[i] = currMiddleRow;
	  xPosInRow[i] = rowRightLast;
	  rowLeftPlaced[currMiddleRow] = false;
	  rowCapacity[currMiddleRow] -= cellWidth;
	  rowRightX[currMiddleRow] = rowRightLast;
	  cellPlaced = true;
	  thisCell.CellSetXpos(xPosInRow[i]);
	  thisCell.CellSetYpos((rowNum[i] * rowHeight));
	  break;
	} else {
	  currMiddleRow++;
	}
      } else {
	if (rowLeftLast + cellWidth < rowRightLast) {
	  rowNum[i] = currMiddleRow;
	  xPosInRow[i] = rowLeftLast;
	  rowLeftPlaced[currMiddleRow] = true;
	  rowLeftLast += cellWidth;
	  rowCapacity[currMiddleRow] -= cellWidth;
	  rowLeftX[currMiddleRow] = rowLeftLast;
	  cellPlaced = true;
	  thisCell.CellSetXpos(xPosInRow[i]);
	  thisCell.CellSetYpos((rowNum[i] * rowHeight));
	  break;
	} else {
	  currMiddleRow++;
	}
      }
    }
    if (cellPlaced) {
      //      cout << "PLC: Placed boundary cell " << thisCell.CellGetName() 
      //	   << " at " << xPosInRow[i] 
      //	   << " in row " << rowNum[i] 
      //	   << endl;
    }
    if (!cellPlaced) {
      /* Place the cell in the row with most available capacity */
      int highestRowCap;
      uint highestRowCapIdx;
      highestRowCap = -INT_MAX;
      highestRowCapIdx = 0;
      for (idx = 0; idx < numRows; idx++) {
	if (rowCapacity[idx] > highestRowCap) {
	  highestRowCap = rowCapacity[idx];
	  highestRowCapIdx = idx;
	}
      }
      xPosInRow[i] = 0;
      rowNum[i] = highestRowCapIdx;
      thisCell.CellSetXpos(xPosInRow[i]);
      thisCell.CellSetYpos((rowNum[i] * rowHeight));
      //      cout << "PLC: Placed boundary cell " << thisCell.CellGetName() 
      //	   << " at " << xPosInRow[i] 
      //	   << " in row " << rowNum[i] 
      //	   << endl;
      //      string msg = "Could not place cell " + thisCell.CellGetName() + " inside the cluster";
      //      _ASSERT_TRUE(msg);
    }
  }
  return (cellPlaced);
}

bool
Design::DesignPlaceCellsInCluster(vector<Cell *> &boundaryCellVec, 
				  vector<uint> &rowNum, vector<uint> &xPosInRow,
				  uint resultHeight, uint resultWidth)
{
  Cell *cellPtr;
  uint rowHeight, rowCapacity;
  int cellWidth, i;
  uint numRows, firstRow, lastRow;
  int remWidthFirstRow, remWidthLastRow;
  uint currMiddleRow;
  uint firstRowPrevX, lastRowPrevX;
  uint rowLeftLast, rowRightLast;
  uint numBoundaryCells = boundaryCellVec.size();
  vector<uint> rowLeftX;
  vector<uint> rowRightX;
  vector<bool> rowLeftPlaced;
  bool cellPlaced;

  /* Divide the height into rows */
  rowHeight = (double)DesignGetSingleRowHeight();
  
  /* Compute the number of rows */
  numRows = resultHeight / rowHeight;

  /* Initialize all capacities to resultWidth */
  for (i = 0; i < numRows; i++) {
    rowLeftX.push_back(0);
    rowRightX.push_back(resultWidth);
    rowLeftPlaced.push_back(false);
  }

  cellPlaced = true;
  /* Assign row number and x-position to each cell */
  firstRow = 0; lastRow = numRows - 1; firstRowPrevX = 0; lastRowPrevX = 0;
  remWidthFirstRow = resultWidth; remWidthLastRow = resultWidth;
  for (i = 0; i < numBoundaryCells; i++) {
    Cell &thisCell = *(Cell *)boundaryCellVec[i];
    cellWidth = thisCell.CellGetWidth();
    cellPlaced = false;
    if (remWidthFirstRow > 0 &&
	(remWidthFirstRow - cellWidth >= 0)) {
      rowNum[i] = 0;
      xPosInRow[i] = firstRowPrevX;
      firstRowPrevX += cellWidth;
      remWidthFirstRow -= cellWidth;
      cellPlaced = true;
      continue;
    }
    if (remWidthLastRow > 0 &&
	(remWidthLastRow - cellWidth >= 0)) {
      rowNum[i] = lastRow;
      xPosInRow[i] = lastRowPrevX;
      lastRowPrevX += cellWidth;
      remWidthLastRow -= cellWidth;
      cellPlaced = true;
      continue;
    }
    /* For middle rows, place cells on the sides 
       along the vertical boundaries */
    currMiddleRow = firstRow + 1;
    cellPlaced = false;
    while (currMiddleRow != lastRow) {
      rowLeftLast = rowLeftX[currMiddleRow];
      rowRightLast = rowRightX[currMiddleRow];
      if (rowLeftPlaced[currMiddleRow]) {
	if (rowRightLast - cellWidth > rowLeftLast) {
	  rowNum[i] = currMiddleRow;
	  rowRightLast -= cellWidth;
	  xPosInRow[i] = rowRightLast;
	  rowLeftPlaced[currMiddleRow] = false;
	  cellPlaced = true;
	  rowRightX[currMiddleRow] = rowRightLast;
	  break;
	} else {
	  currMiddleRow++;
	}
      } else {
	if (rowLeftLast + cellWidth < rowRightLast) {
	  rowNum[i] = currMiddleRow;
	  xPosInRow[i] = rowLeftLast;
	  rowLeftPlaced[currMiddleRow] = true;
	  rowLeftLast += cellWidth;
	  rowLeftX[currMiddleRow] = rowLeftLast;
	  cellPlaced = true;
	  break;
	} else {
	  currMiddleRow++;
	}
      }
    }
    if (!cellPlaced) {
      //      cout << "Could not place boundary cell along the boundary "
      //	   << "of the cell! This case is not handled by our code!!" 
      //	   << endl;
      break;
    }
  }
  return (cellPlaced);
}

/*******************************************************************************
  PLACED CELLS OF THE CLUSTER IN THE GIVEN NUMBER OF ROWS
*******************************************************************************/
void
Design::DesignPlaceCellsInClusterInRows(vector<Cell *> &cellsOfCluster,
					uint resultHeight, uint resultWidth)
{
  Cell *cellPtr;
  vector<uint> lastLeftPos;
  uint rowHeight, numRows;
  uint cellWidth;
  uint cellXPos, cellYPos;
  uint numCells, idx, placedRows;
  uint curRow, curRowLeftPos;
  bool cellPlaced;
  
  rowHeight = DesignGetSingleRowHeight();
  numRows = resultHeight / rowHeight;
  for (idx = 0; idx < numRows; idx++) {
    lastLeftPos.push_back(0);
  }
  
  curRow = 0;
  numCells = cellsOfCluster.size();
  for (idx = 0; idx < numCells; idx++) {
    Cell &thisCell = *(cellsOfCluster[idx]);
    cellWidth = thisCell.CellGetWidth();
    cellPlaced = false;

    cellXPos = 0; 
    while (cellPlaced) {
      curRowLeftPos = lastLeftPos[curRow];
      if (curRowLeftPos + cellWidth > resultWidth) {
	if (curRow == numRows - 1) {
	  cellXPos = resultWidth - cellWidth;
	  cellPlaced = true;
	} else {
	  curRow++;
	  cellYPos = curRow * rowHeight;
	}
      } else {
	cellXPos = curRowLeftPos;
	cellPlaced = true;
      }
    }
    cellYPos = curRow * rowHeight;
    thisCell.CellSetPosDbl(cellXPos, cellYPos);
  }
}

/****************************************************************
  GIVEN A CLUSTER CELL, COLLAPSE IT AND RETURN ITS CLUSTER
  DATA
****************************************************************/
void
Design::DesignSimpleCollapseCluster(Cell *cellToCollapse, 
				    vector<Net *> &internalNets,
				    vector<Cell *> &cellsOfCluster)
{
  Cluster *clusterOfCell;
  Pin *pinPtr, *clusterPinPtr;
  Net *netPtr;

  internalNets.clear();
  cellsOfCluster.clear();
  clusterOfCell = (Cluster *)CellGetCluster(cellToCollapse);
  if (clusterOfCell == NIL(Cluster *)) {
    return;
  }

  /* Get the internal cells */
  internalNets = (*clusterOfCell).ClusterGetInternalNets();
  /* Get the internal nets of the cluster */
  cellsOfCluster = (*clusterOfCell).ClusterGetCellsOfCluster();
  /* Remove the cluster data */
  CellClearCluster(cellToCollapse);
  /* Remove all connections of nets to the pins of the cluster */
  map<Pin*, Pin*> &pinMap = (*clusterOfCell).ClusterGetPinMap();
  MAP_FOR_ALL_ELEMS(pinMap, Pin *, Pin *, clusterPinPtr, pinPtr) {
    Net &connectedNet = (*clusterPinPtr).PinGetNet();
    connectedNet.NetRemovePin(*clusterPinPtr);
    /* Unhide the original pin */
    (*pinPtr).PinSetIsHidden(false);
    /* Destroy the pin that has been created on the cluster */
    DesignDeletePin(clusterPinPtr);
  } END_FOR;

  /* Unhide internal cells */
  VECTOR_FOR_ALL_ELEMS(internalNets, Net*, netPtr) {
    DesignUnhideNet(netPtr);
  } END_FOR;

  /* Remove the cell from the design database */
  DesignRemoveOneClusterFromDesignDB(cellToCollapse);
  
  DesignDeleteCell(cellToCollapse);
  delete clusterOfCell;
}

/*******************************************************************************
  LIGHTER VERSION OF THE TOP LEVEL CLUSTERING FUNCTION. DOES NOT CHECK 
  INTERNAL NETS, PINS, ETC.. JUST CREATES A CLUSTER CELL WITH A LIST OF CELLS
  THAT WOULD BE ASSOCIATED WITH IT. COMMITTING OF THE CLUSTER IS DONE LATER 
*******************************************************************************/
Cell*
Design::DesignClusterCellsSimple(vector<Cell *> &listOfCells) 
{
  Cell *clusterCell, *cellPtr;
  Net *netPtr;
  void *netObj;
  map<void *, bool> internalNetObjs, externalNetObjs;
  map<Net *, bool> internalNets, affectedNets;
  map<Pin *, Pin *> pinMap;
  vector<Net *> internalNetsFromChildClusters, internalNetVec;
  vector<Cell *> cellsFromChildClusters;
  vector<uint> bCellIndices;
  vector<uint> rowNum;
  vector<uint> xPosInRow;
  string clusterCellName;
  uint resultHeight, resultWidth;
  uint numCells, numClusters, idx;
  bool rowBased;

  Env &DesignEnv = DesignGetEnv();
  HyperGraph &myGraph = DesignGetGraph();

  numCells = listOfCells.size();
  numClusters = NumClustersSeenSofar++;
  clusterCellName = "c" + getStrFromInt(numClusters + 1);
  if (clusterCellName == "c1005") {
    //    cout << "break here" << endl;
  }
  if (debug) {
    cout << "Cluster cell name : " << clusterCellName << " " << endl;
    cout << "Number of cells to cluster : " << numCells << " " << endl;
  }
  /* Update the hypergraph */
  if (debug) {
    cout << "------------------------------------" << endl
	 << " UPDATING HYPERGRAPH                " << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  /* Create a new cell */
  clusterCell = new Cell(0, 0, clusterCellName);
  (*clusterCell).CellSetIsCluster(true);

  /* Make corresponding changes in the hypergraph */
  vector<void *> listOfNodes(listOfCells.begin(), listOfCells.end());
  myGraph.ClusterNodes(listOfNodes, (void *)clusterCell, externalNetObjs,
		       internalNetObjs);
  MAP_FOR_ALL_KEYS(internalNetObjs, void*, bool, netObj) {
    netPtr = (Net *)netObj;
    internalNets[netPtr] = 1;
    affectedNets[netPtr] = 1;
    DesignHideNet(netPtr);
  } END_FOR;

  for (idx = 0; idx < numCells; idx++) {
    cellPtr = listOfCells[idx];
    if ((*cellPtr).CellIsCluster()) {
      vector<Net *> tmpInternalNets;
      vector<Cell *> tmpChildCells;
      DesignSimpleCollapseCluster(cellPtr, tmpInternalNets, tmpChildCells);
      internalNetsFromChildClusters.insert(internalNetsFromChildClusters.end(),
					   tmpInternalNets.begin(), tmpInternalNets.end());
      cellsFromChildClusters.insert(cellsFromChildClusters.end(),
				    tmpChildCells.begin(), tmpChildCells.end());
      listOfCells.erase(listOfCells.begin()+idx);
      numCells--;
      idx--;
      continue;
    } else {
      DesignHideCell(cellPtr);
    }
  }

  /* Add child obtained from child clusters into list of cells */
  VECTOR_FOR_ALL_ELEMS(cellsFromChildClusters, Cell *, cellPtr) {
    DesignHideCell(cellPtr);
    listOfCells.push_back(cellPtr);
  } END_FOR;

  /* Add internal nets obtained from child clusters into list of nets */
  VECTOR_FOR_ALL_ELEMS(internalNetsFromChildClusters, Net *, netPtr) {
    affectedNets[netPtr] = true;
    internalNets[netPtr] = true;
  } END_FOR;
  
  /* Get the height and width of new cell */
  DesignGetClusterDimensions(listOfCells, resultWidth, resultHeight);
  if (debug) {
    cout << "Inferred size of cluster, Width: " << resultWidth 
	 << " Height: " << resultHeight << endl;
  }
  (*clusterCell).CellSetWidth(resultWidth);
  (*clusterCell).CellSetHeight(resultHeight);

  /* Build the internal nets vector */
  MAP_FOR_ALL_KEYS(internalNets, Net*, bool, netPtr) {
    internalNetVec.push_back(netPtr);
  } END_FOR;
  
  /* Create the cluster object */
  Cluster *thisCluster = new Cluster(listOfCells, bCellIndices, rowNum,
				     xPosInRow, pinMap);
  
  /* Check if row based clustering has been specified */
  uint numRows = DesignEnv.EnvGetClusterNumRows();
  rowBased = false;
  if (numRows > 0) {
    rowBased = true;
    (*thisCluster).ClusterSetRowBased(rowBased);
  }
  /* Create a vector of internal nets */
  (*thisCluster).ClusterSetBCellsPlaced(false);
  (*thisCluster).ClusterSetInternalNets(internalNetVec);
  CellSetCluster(clusterCell, thisCluster);

  /* Add the cluster to design database */
  DesignAddOneClusterToDesignDB(clusterCell);
  
  return (clusterCell);
}

/*******************************************************************************
  AN EXTENSION OF THE FUNCTION DesignCommitCluster. INSTEAD OF PLACING BOUNDARY 
  CELLS, PLACES ALL THE CELLS IN THE FORM OF ROWS
*******************************************************************************/
void
Design::DesignCommitClusterRowBased(Cell *clusterCell, Cluster *clusterOfCell)
{
  Cell *cellPtr;
  Pin *pinPtr, *clusterPinPtr, *newPinPtr;
  Net *netPtr;
  string clusterCellName, newPinName;
  bool bCellsPlaced, rowBased;
  uint numCells, numBoundaryCells;
  uint clusterCellNumPins;
  uint resultHeight, resultWidth;
  uint edgeIdx;
  uint idx;

  if (clusterOfCell == NIL(Cluster *)) {
    _ASSERT_TRUE("Error: Clustered cell does not have an associated cluster data structure");
    return;
  }

  HyperGraph &myGraph = DesignGetGraph();
  clusterCellName = (*clusterCell).CellGetName();

  /* Get the internal nets of the cluster */
  vector<Net *> &internalNetVec = (*clusterOfCell).ClusterGetInternalNets();
  /* Get the cells of the cluster */
  vector<Cell *> &cellsOfCluster = (*clusterOfCell).ClusterGetCellsOfCluster();
  /* Get the pin map of the cluster: Empty */
  map<Pin*, Pin*> &pinMap = (*clusterOfCell).ClusterGetPinMap();

  map<Cell *, bool> cellLookupMap;
  map<Net *, bool> internalNets, externalNets;
  map<Cell *, uint> boundaryCells;
  vector<Cell *> boundaryCellVec;
  vector<Pin *> clusterCellPins;
  vector<uint> bCellIndices;

  resultHeight = (*clusterCell).CellGetHeight();
  resultWidth = (*clusterCell).CellGetWidth();

  numCells = cellsOfCluster.size();
  /* Build a list of cells for lookup later */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = cellsOfCluster[idx];
    _KEY_EXISTS(cellLookupMap, cellPtr) {
      _ASSERT_TRUE("Error: Duplicate cells found in cluster list!!");
    }
    cellLookupMap[cellPtr] = true;
  }
  /* Build external nets */
  vector<uint> externalNetsVec = myGraph.GetEdgesOfObject((void *)clusterCell);
  VECTOR_FOR_ALL_ELEMS(externalNetsVec, uint, edgeIdx) {
    Net *netPtr = (Net *)myGraph.GetEdgeObject(edgeIdx);
    externalNets[netPtr] = true;
  } END_FOR;
  /* Infer boundary cells */
  MAP_FOR_ALL_KEYS(cellLookupMap, Cell *, bool, cellPtr) {
    CELL_FOR_ALL_NETS((*cellPtr), PIN_DIR_ALL, netPtr) {
      _KEY_EXISTS(externalNets, netPtr) {
        boundaryCells[cellPtr] = true;
        break;
      }
    } CELL_END_FOR;
  } END_FOR;
  /* Create a vector of boundary cells */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = cellsOfCluster[idx];
    _KEY_EXISTS(boundaryCells, cellPtr) {
      boundaryCellVec.push_back(cellPtr);
      bCellIndices.push_back(idx);
    }
  }
  /* Create of pins on the cluster which are mapped to actual pins of the boundary cells */
  /* All pins of these cells connected to external nets will have */
  clusterCellNumPins = 1;
  MAP_FOR_ALL_KEYS(boundaryCells, Cell *, uint, cellPtr) {
    CELL_FOR_ALL_PINS((*cellPtr), PIN_DIR_ALL, pinPtr) {
      netPtr = &((*pinPtr).PinGetNet());
      _KEY_EXISTS(externalNets, netPtr) {
	newPinPtr = new Pin();
	(*newPinPtr).PinSetParentCell(*clusterCell);
	newPinName = clusterCellName + "_" + getStrFromInt(clusterCellNumPins);
	(*newPinPtr).PinSetName(newPinName);
	(*newPinPtr).PinSetDirection((*pinPtr).PinGetDirection());
	(*clusterCell).CellAddPin(newPinPtr);
	clusterCellPins.push_back(newPinPtr);
	pinMap[newPinPtr] = pinPtr;
	clusterCellNumPins++;
      }
    } CELL_END_FOR;
  } END_FOR;
  /* Create pin offsets for the clustered cell from boundary cells */
  /***** TODO: Quick dirty placement of boundary cells inside the cluster ******/
  bCellsPlaced = false;
  vector<uint> rowNum;
  vector<uint> xPosInRow;
  DesignPlaceCellsInClusterInRows(cellsOfCluster, resultHeight, resultWidth);
  /* Hide internal nets */
  VECTOR_FOR_ALL_ELEMS(internalNetVec, Net*, netPtr) {
    DesignHideNet(netPtr);
  } END_FOR;
  /* Connect the net connected to existing pins, to corresponding pins of 
     the cluster and hide the original pins */
  MAP_FOR_ALL_ELEMS(pinMap, Pin *, Pin *, newPinPtr, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    (*pinPtr).PinSetIsHidden(true);
    (*newPinPtr).Connect(connectedNet);
    connectedNet.NetAddPin(*newPinPtr);
  } END_FOR;
  /* Hide all the cells and update indices of cells in the 
     vector "listOfCells" in the map  */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = (Cell *)cellsOfCluster[idx];
    DesignHideCell(cellPtr);
  }
  for (idx = 0; idx < boundaryCellVec.size(); idx++) {
    cellPtr = (Cell *)boundaryCellVec[idx];
    _KEY_EXISTS(boundaryCells, cellPtr) {
      boundaryCells[cellPtr] = idx;
    }
  }
  DesignAssignPinOffSetsInRows(clusterCell, clusterCellPins, pinMap);
  /* Add data to the cluster object */
  (*clusterOfCell).ClusterSetNumCells(numCells);
  (*clusterOfCell).ClusterSetCellsOfCluster(cellsOfCluster);
  (*clusterOfCell).ClusterSetBCellIndices(bCellIndices);
  (*clusterOfCell).ClusterSetRowNums(rowNum);
  (*clusterOfCell).ClusterSetXPosInRows(xPosInRow);
  (*clusterOfCell).ClusterSetPinMap(pinMap);
  (*clusterOfCell).ClusterSetBCellsPlaced(bCellsPlaced);
  (*clusterOfCell).ClusterSetInternalNets(internalNetVec);
  /* Write out a placement plot for the cluster */
  if (debug) {
    DesignPlotCluster("cluster formed", (clusterCellName + ".plt"),
                      clusterCell, boundaryCellVec, rowNum, xPosInRow,
                      clusterCellPins);
  }
  debug = false;
}

/*******************************************************************************
  COMMITS A CLUSTER WHICH HAS NOT BEEN COMPLETELY FORMED. IN COMMITTING 
  THE CLUSTER, THE FOLLOWING THINGS ARE ASSUMED:
  1) The list of cells is available
  2) The list of internal nets is available
  3) Cluster size has been inferred

  WHAT HAS TO BE DONE:
  1) Boundary cells have to be inferred 
  2) Boundary cells have to placed
  3) External nets have to be derived 
  4) Pins of the cluster have to be created
  5) Boundary cells have to placed 
  6) Pin offsets for the cluster have to be assigned 
*******************************************************************************/
void
Design::DesignCommitCluster(Cell *clusterCell)
{
  Cluster *clusterOfCell;
  Cell *cellPtr;
  Pin *pinPtr, *clusterPinPtr, *newPinPtr;
  Net *netPtr;
  string clusterCellName, newPinName;
  bool bCellsPlaced, rowBased;
  uint numCells, numBoundaryCells;
  uint clusterCellNumPins;
  uint resultHeight, resultWidth;
  uint edgeIdx;
  uint idx;
  debug = true;

  clusterOfCell = (Cluster *)CellGetCluster(clusterCell);
  if (clusterOfCell == NIL(Cluster *)) {
    _ASSERT_TRUE("Error: Clustered cell does not have an associated cluster data structure");
    return;
  }

  /* Check if the cluster is a row based cluster */
  rowBased = (*clusterOfCell).ClusterGetRowBased();
  if (rowBased) {
    DesignCommitClusterRowBased(clusterCell, clusterOfCell);
    return;
  }

  HyperGraph &myGraph = DesignGetGraph();
  clusterCellName = (*clusterCell).CellGetName();
  /* Get the internal nets of the cluster */
  vector<Net *> &internalNetVec = (*clusterOfCell).ClusterGetInternalNets();
  /* Get the cells of the cluster */
  vector<Cell *> &cellsOfCluster = (*clusterOfCell).ClusterGetCellsOfCluster();
  /* Get the pin map of the cluster: Empty */
  map<Pin*, Pin*> &pinMap = (*clusterOfCell).ClusterGetPinMap();

  map<Cell *, bool> cellLookupMap;
  map<Net *, bool> internalNets, externalNets;
  map<Cell *, uint> boundaryCells;
  vector<Cell *> boundaryCellVec;
  vector<Pin *> clusterCellPins;
  vector<uint> bCellIndices;

  resultHeight = (*clusterCell).CellGetHeight();
  resultWidth = (*clusterCell).CellGetWidth();

  numCells = cellsOfCluster.size();
  /* Build a list of cells for lookup later */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = cellsOfCluster[idx];
    _KEY_EXISTS(cellLookupMap, cellPtr) {
      _ASSERT_TRUE("Error: Duplicate cells found in cluster list!!");
    }
    cellLookupMap[cellPtr] = true;
  }

  /* Build external nets */
  vector<uint> externalNetsVec = myGraph.GetEdgesOfObject((void *)clusterCell);
  VECTOR_FOR_ALL_ELEMS(externalNetsVec, uint, edgeIdx) {
    Net *netPtr = (Net *)myGraph.GetEdgeObject(edgeIdx);
    externalNets[netPtr] = true;
  } END_FOR;

  /* Infer boundary cells */
  MAP_FOR_ALL_KEYS(cellLookupMap, Cell *, bool, cellPtr) {
    CELL_FOR_ALL_NETS((*cellPtr), PIN_DIR_ALL, netPtr) {
      _KEY_EXISTS(externalNets, netPtr) {
        boundaryCells[cellPtr] = true;
        break;
      }
    } CELL_END_FOR;
  } END_FOR;

  /* Create a vector of boundary cells */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = cellsOfCluster[idx];
    _KEY_EXISTS(boundaryCells, cellPtr) {
      boundaryCellVec.push_back(cellPtr);
      bCellIndices.push_back(idx);
    }
  }
  /* Create of pins on the cluster which are mapped to actual pins of the boundary cells */
  /* All pins of these cells connected to external nets will have */
  if (debug) {
    cout << "------------------------------------" << endl
	 << " CREATING PINS FOR CLUSTER          " << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  clusterCellNumPins = 1;
  MAP_FOR_ALL_KEYS(boundaryCells, Cell *, uint, cellPtr) {
    CELL_FOR_ALL_PINS((*cellPtr), PIN_DIR_ALL, pinPtr) {
      netPtr = &((*pinPtr).PinGetNet());
      _KEY_EXISTS(externalNets, netPtr) {
	newPinPtr = new Pin();
	(*newPinPtr).PinSetParentCell(*clusterCell);
	newPinName = clusterCellName + "_" + getStrFromInt(clusterCellNumPins);
	(*newPinPtr).PinSetName(newPinName);
	(*newPinPtr).PinSetDirection((*pinPtr).PinGetDirection());
	(*clusterCell).CellAddPin(newPinPtr);
	clusterCellPins.push_back(newPinPtr);
	pinMap[newPinPtr] = pinPtr;
	if (debug) {
	  cout << "\tAdded " << (*newPinPtr).PinGetName() 
	       << " as cluster pin " << endl; 
	}
	clusterCellNumPins++;
      }
    } CELL_END_FOR;
  } END_FOR;
  
  /* Create pin offsets for the clustered cell from boundary cells */
  /***** TODO: Quick dirty placement of boundary cells inside the cluster ******/
  bCellsPlaced = false;
  numBoundaryCells = boundaryCellVec.size();
  vector<uint> rowNum(numBoundaryCells);
  vector<uint> xPosInRow(numBoundaryCells);
  if (DesignEnv.EnvGetClusterPlacementType() == ENV_CLUSTER_PLACE_BOUNDARY) {
    DesignPlaceCellsInClusterNoLegal(boundaryCellVec, rowNum, xPosInRow, resultHeight,
				     resultWidth);
    bCellsPlaced = true;
  } else {
    DesignPlaceCellsInClusterInCenter(boundaryCellVec, clusterCellPins, rowNum,
				      xPosInRow, resultHeight, resultWidth);
  }
  if (debug) {
    cout << "---------------------------------------" << endl
         << " HIDING INTERNAL NETS                  " << endl
         << "---------------------------------------" << endl
         << endl;
  }

  /* Hide internal nets */
  VECTOR_FOR_ALL_ELEMS(internalNetVec, Net*, netPtr) {
    if (debug) {
      cout << "  Hiding Net: " << (*netPtr).NetGetName() << endl;
    }
    DesignHideNet(netPtr);
  } END_FOR;

  /* Connect the net connected to existing pins, to corresponding pins of 
     the cluster and hide the original pins */
  MAP_FOR_ALL_ELEMS(pinMap, Pin *, Pin *, newPinPtr, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    (*pinPtr).PinSetIsHidden(true);
    if (debug) {
      cout << "Connected net " << connectedNet.NetGetName()
           << " to pin " << (*newPinPtr).PinGetName() << endl;
    }
    (*newPinPtr).Connect(connectedNet);
    connectedNet.NetAddPin(*newPinPtr);
    if (debug) {
      cout << "Added " << (*newPinPtr).PinGetName()
           << " Corresponding to pin " << (*pinPtr).PinGetName()
           << " to external net " << connectedNet.NetGetName() << " " << endl;
    }
  } END_FOR;

  /* Hide all the cells and update indices of cells in the 
     vector "listOfCells" in the map  */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = (Cell *)cellsOfCluster[idx];
    DesignHideCell(cellPtr);
  }

  for (idx = 0; idx < boundaryCellVec.size(); idx++) {
    cellPtr = (Cell *)boundaryCellVec[idx];
    _KEY_EXISTS(boundaryCells, cellPtr) {
      boundaryCells[cellPtr] = idx;
    }
  }

  /* There is a correspondence between boundaryCellVec, rowNum
     and xPosInRow */
  if (debug) {
    cout << "---------------------------------------" << endl
         << " ASSIGN PIN OFFSETS                    " << endl
         << "---------------------------------------" << endl
         << endl;
  }
  DesignAssignPinOffSets(clusterCell, boundaryCells, clusterCellPins,
                         pinMap, rowNum, xPosInRow);

  /* Store all the relevant information for the cluster */
  /* Need to store the following relelvant information for a cluster */
  /*  -- Cells of the cluster: VECTOR                    */
  /*  -- Boundary cell indices: VECTOR                   */
  /*  -- Pin map: MAP                                    */
  /*  -- Relative positions of cells inside the cluster  */
  /*     -- 2 VECTORs: row number and x position in each */
  /*                   row                               */

  /* Add data to the cluster object */
  (*clusterOfCell).ClusterSetNumCells(numCells);
  (*clusterOfCell).ClusterSetCellsOfCluster(cellsOfCluster);
  (*clusterOfCell).ClusterSetBCellIndices(bCellIndices);
  (*clusterOfCell).ClusterSetRowNums(rowNum);
  (*clusterOfCell).ClusterSetXPosInRows(xPosInRow);
  (*clusterOfCell).ClusterSetPinMap(pinMap);
  (*clusterOfCell).ClusterSetBCellsPlaced(bCellsPlaced);
  (*clusterOfCell).ClusterSetInternalNets(internalNetVec);

  /* Write out a placement plot for the cluster */
  if (debug) {
    DesignPlotCluster("cluster formed", (clusterCellName + ".plt"),
                      clusterCell, boundaryCellVec, rowNum, xPosInRow,
                      clusterCellPins);
  }
  debug = false;
}

/*******************************************************************************
  THE TOP LEVEL CLUSTERING FUNCTION. COMPLETELY FORMS A CLUSTER
*******************************************************************************/
Cell*
Design::DesignClusterCells(vector<Cell *> &listOfCells, bool collapseClustersInList,
			   bool placeBoundaryCells)
{
  Cell *cellPtr, *clusterCell;
  void *netObj;
  Net *netPtr;
  Pin *pinPtr;
  Pin *newPinPtr;
  map<Cell *, bool> cellLookupMap;
  map<Cell *, uint> boundaryCells;
  map<void *, bool> internalNetObjs, externalNetObjs;
  map<Net *, bool> internalNets, externalNets, affectedNets;
  map<Pin *, Pin *> pinMap;
  vector<Pin *> clusterCellPins;
  vector<Cell *> boundaryCellVec, cellsFromChildClusters;
  vector<Net *> internalNetsFromChildClusters;
  vector<Net *> internalNetVec;
  vector<uint> bCellIndices;
  uint clusterHeight, clusterWidth;
  uint numCells, numBoundCells;
  uint numClusters, clusterCellNumPins;
  uint resultHeight, resultWidth;
  int idx;
  bool bCellsPlaced;
  string clusterCellName;
  string newPinName;

  debug = false;
  /* Get the hypergraph from the Design */
  HyperGraph &myGraph = DesignGetGraph();
  /* Get the environment from the Design */
  Env &DesignEnv = DesignGetEnv();

  numCells = listOfCells.size();
  numClusters = NumClustersSeenSofar++;
  clusterCellName = "c" + getStrFromInt(numClusters + 1);
  if (debug) {
    cout << "Cluster cell name : " << clusterCellName << " " << endl;
    cout << "Number of cells to cluster : " << numCells << " " << endl;
  }

  /* Update the hypergraph */
  if (debug) {
    cout << "------------------------------------" << endl
	 << " UPDATING HYPERGRAPH                " << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  /* Create a new cell */
  clusterCell = new Cell(0, 0, clusterCellName);
  (*clusterCell).CellSetIsCluster(true);

  /* Make corresponding changes in the hypergraph */
  vector<void *> listOfNodes(listOfCells.begin(), listOfCells.end());
  myGraph.ClusterNodes(listOfNodes, (void *)clusterCell, externalNetObjs,
		       internalNetObjs);

  if (debug) {
    cout << "------------------------------------" << endl
	 << " CREATING INTERNAL AND EXTERNAL NETS" << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  /* Build a list of internal, external and affected nets */
  MAP_FOR_ALL_KEYS(internalNetObjs, void*, bool, netObj) {
    internalNets[(Net*)netObj] = 1;
    affectedNets[(Net*)netObj] = 1;
  } END_FOR;
  MAP_FOR_ALL_KEYS(externalNetObjs, void*, bool, netObj) {
    externalNets[(Net*)netObj] = 1;
    affectedNets[(Net*)netObj] = 1;
  } END_FOR;

  /* Build a list of cells for lookup later */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = listOfCells[idx];
    if (collapseClustersInList) {
      if ((*cellPtr).CellIsCluster()) {
	vector<Net *> tmpInternalNets;
	vector<Cell *> tmpChildCells;
	DesignSimpleCollapseCluster(cellPtr, tmpInternalNets, tmpChildCells);
	internalNetsFromChildClusters.insert(internalNetsFromChildClusters.end(),
					     tmpInternalNets.begin(), tmpInternalNets.end());
	cellsFromChildClusters.insert(cellsFromChildClusters.end(),
				      tmpChildCells.begin(), tmpChildCells.end());
	listOfCells.erase(listOfCells.begin()+idx);
	numCells--;
	idx--;
	continue;
      }
    }
    _KEY_DOES_NOT_EXIST(cellLookupMap, cellPtr) {
      cellLookupMap[cellPtr] = true;
    }
  } 
  
  if (collapseClustersInList) {
    /* Add the internal cells obtained from child clusters into 
       list of cells */
    VECTOR_FOR_ALL_ELEMS(cellsFromChildClusters, Cell *, cellPtr) {
      DesignUnhideCell(cellPtr);
      cellLookupMap[cellPtr] = cellPtr;
      listOfCells.push_back(cellPtr);
    } END_FOR;
    /* Add internal nets obtained from child clusters into list of 
       nets */
    VECTOR_FOR_ALL_ELEMS(internalNetsFromChildClusters, Net *, netPtr) {
      affectedNets[netPtr] = true;
      internalNets[netPtr] = true;
    } END_FOR;
  }

  if (debug) {
    cout << "Number of affected nets: " << affectedNets.size() << endl;
    cout << "Internal nets: " << internalNets.size() << endl;
    MAP_FOR_ALL_KEYS(internalNets, Net*, bool, netPtr) {
      cout << (*netPtr).NetGetName() << " ";
    } END_FOR;
    cout << endl;
    cout << "External nets: " << externalNets.size() << endl;
    MAP_FOR_ALL_KEYS(externalNets, Net*, bool, netPtr) {
      cout << (*netPtr).NetGetName() << " ";
    } END_FOR;
    cout << endl << endl;
    cout << "------------------------------------" << endl
	 << " BUILD BOUNDARY CELLS               " << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  numCells = listOfCells.size();
  /* Create a map of boundary cells */
  MAP_FOR_ALL_KEYS(cellLookupMap, Cell *, bool, cellPtr) {
    CELL_FOR_ALL_NETS((*cellPtr), PIN_DIR_ALL, netPtr) {
      _KEY_EXISTS(externalNets, netPtr) {
	boundaryCells[cellPtr] = true;
	if (debug) {
	  cout << "\tAdded cell " << (*cellPtr).CellGetName() << " as boundary cell" << endl;
	}
	break;
      }
    } CELL_END_FOR;
  } END_FOR;

  /* Create a vector of boundary cells */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = listOfCells[idx];
    _KEY_EXISTS(boundaryCells, cellPtr) {
      boundaryCellVec.push_back(cellPtr);
      bCellIndices.push_back(idx);
    }
  }

  if (debug) {
    cout << "Number of boundary cells: " << boundaryCellVec.size() << endl;
    cout << "------------------------------------" << endl
	 << " GETTING CLUSTER DIMENSIONS         " << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  /* Get the height and width of new cell */
  DesignGetClusterDimensions(listOfCells, resultWidth, resultHeight);
  if (debug) {
    cout << "Inferred size of cluster, Width: " << resultWidth 
	 << " Height: " << resultHeight << endl;
  }
  (*clusterCell).CellSetWidth(resultWidth);
  (*clusterCell).CellSetHeight(resultHeight);

  /* Create of pins on the cluster which are mapped to actual pins of the boundary cells */
  /* All pins of these cells connected to external nets will have */
  if (debug) {
    cout << "------------------------------------" << endl
	 << " CREATING PINS FOR CLUSTER          " << endl
	 << "------------------------------------" << endl
	 << endl;
  }

  clusterCellNumPins = 1;
  MAP_FOR_ALL_KEYS(boundaryCells, Cell *, uint, cellPtr) {
    CELL_FOR_ALL_PINS((*cellPtr), PIN_DIR_ALL, pinPtr) {
      netPtr = &((*pinPtr).PinGetNet());
      _KEY_EXISTS(externalNets, netPtr) {
	newPinPtr = new Pin();
	(*newPinPtr).PinSetParentCell(*clusterCell);
	newPinName = clusterCellName + "_" + getStrFromInt(clusterCellNumPins);
	(*newPinPtr).PinSetName(newPinName);
	(*newPinPtr).PinSetDirection((*pinPtr).PinGetDirection());
	(*clusterCell).CellAddPin(newPinPtr);
	clusterCellPins.push_back(newPinPtr);
	pinMap[newPinPtr] = pinPtr;
	if (debug) {
	  cout << "\tAdded " << (*newPinPtr).PinGetName() 
	       << " as cluster pin " << endl; 
	}
	clusterCellNumPins++;
      }
    } CELL_END_FOR;
  } END_FOR;
  
  if (debug) {
    cout << "Number of external pins on cluster : " << clusterCellPins.size() << endl;  
    cout << "---------------------------------------" << endl
	 << " CREATING PLACEMENT FOR BOUNDARY CELLS " << endl
	 << "---------------------------------------" << endl
	 << endl;
  }
  /* Create pin offsets for the clustered cell from boundary cells */
  /***** TODO: Quick dirty placement of boundary cells inside the cluster ******/
  numBoundCells = boundaryCellVec.size();
  vector<uint> rowNum(numBoundCells);
  vector<uint> xPosInRow(numBoundCells);
  bCellsPlaced = false;
  if (DesignEnv.EnvGetClusterPlacementType() == ENV_CLUSTER_PLACE_BOUNDARY) {
    DesignPlaceCellsInClusterNoLegal(boundaryCellVec, rowNum, xPosInRow, resultHeight, 
				     resultWidth);
    bCellsPlaced = true;
  } else {
    DesignPlaceCellsInClusterInCenter(boundaryCellVec, clusterCellPins, rowNum, 
				      xPosInRow, resultHeight, resultWidth);
  }

  if (debug) {
    cout << "---------------------------------------" << endl
	 << " HIDING INTERNAL NETS                  " << endl
	 << "---------------------------------------" << endl
	 << endl;
  }
  /* Hide internal nets */
  MAP_FOR_ALL_KEYS(internalNets, Net*, bool, netPtr) {
    if (debug) {
      cout << "  Hiding Net: " << (*netPtr).NetGetName() << endl;
    }
    internalNetVec.push_back(netPtr);
    DesignHideNet(netPtr);
  } END_FOR;

  /* Connect the net connected to existing pins, to corresponding pins of 
     the cluster and hide the original pins */
  MAP_FOR_ALL_ELEMS(pinMap, Pin *, Pin *, newPinPtr, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    (*pinPtr).PinSetIsHidden(true);
    if (debug) {
      cout << "Connected net " << connectedNet.NetGetName() 
	   << " to pin " << (*newPinPtr).PinGetName() << endl;
    }
    (*newPinPtr).Connect(connectedNet);
    connectedNet.NetAddPin(*newPinPtr);
    if (debug) {
      cout << "Added " << (*newPinPtr).PinGetName() 
	   << " Corresponding to pin " << (*pinPtr).PinGetName()  
	   << " to external net " << connectedNet.NetGetName() << " " << endl;
    }
  } END_FOR;

  if (debug) {
    cout << "---------------------------------------" << endl
	 << " CREATING DATA TO STORE ON CLUSTERS    " << endl
	 << "---------------------------------------" << endl
	 << endl;
  }

  for (idx = 0; idx < boundaryCellVec.size(); idx++) {
    cellPtr = (Cell *)boundaryCellVec[idx];
    _KEY_EXISTS(boundaryCells, cellPtr) {
      boundaryCells[cellPtr] = idx;
    }
  }

  /* Hide all the cells and update indices of cells in the 
     vector "listOfCells" in the map  */
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = (Cell *)listOfCells[idx];
    DesignHideCell(cellPtr);
  }
  
  /* There is a correspondence between boundaryCellVec, rowNum
     and xPosInRow */
  if (debug) {
    cout << "---------------------------------------" << endl
	 << " ASSIGN PIN OFFSETS                    " << endl
	 << "---------------------------------------" << endl
	 << endl;
  }
  DesignAssignPinOffSets(clusterCell, boundaryCells, clusterCellPins,
			 pinMap, rowNum, xPosInRow);

  /* Store all the relevant information for the cluster */
  /* Need to store the following relelvant information for a cluster */
  /*  -- Cells of the cluster: VECTOR                    */
  /*  -- Boundary cell indices: VECTOR                   */
  /*  -- Pin map: MAP                                    */
  /*  -- Relative positions of cells inside the cluster  */
  /*     -- 2 VECTORs: row number and x position in each */
  /*                   row                               */

  /* Create the cluster object */
  Cluster *thisCluster = new Cluster(listOfCells, bCellIndices, rowNum, 
				     xPosInRow, pinMap);
  /* Create a vector of internal nets */
  (*thisCluster).ClusterSetBCellsPlaced(bCellsPlaced);
  (*thisCluster).ClusterSetInternalNets(internalNetVec);
  CellSetCluster(clusterCell, thisCluster);

  /* Add the cluster to design database */
  DesignAddOneClusterToDesignDB(clusterCell);

  /* Write out a placement plot for the cluster */
  if (debug) {
    DesignPlotCluster("cluster formed", (clusterCellName + ".plt"),
		      clusterCell, boundaryCellVec, rowNum, xPosInRow,
		      clusterCellPins);
  }
  debug = false;
  
  return (clusterCell);
}

void
computeForceOnCellOfCluster(Cell *clusterCell, map<Cell *, uint> cellLookupMap,
			    bool &left, bool &right, bool &top, bool &bot)
{
  Cell *extCell;
  double extAbsXpos, extAbsYpos, absXpos, absYpos;
  double cellXpos, cellYpos;
  double totalXForce, totalYForce;
  Pin *pinPtr, *extPinPtr;

  totalXForce = 0.0; totalYForce = 0.0;
  left = false; right = false; top = false; bot = false;
  cellXpos = (*clusterCell).CellGetXpos();
  cellYpos = (*clusterCell).CellGetYpos();
  CELL_FOR_ALL_PINS((*clusterCell), PIN_DIR_ALL, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    absXpos = cellXpos + (*pinPtr).PinGetXOffset();
    absYpos = cellYpos + (*pinPtr).PinGetYOffset();
    NET_FOR_ALL_PINS(connectedNet, extPinPtr) {
      extCell = &((*extPinPtr).PinGetParentCell());
      _KEY_EXISTS(cellLookupMap, extCell) {
	continue;
      }
      if (extCell == clusterCell) {
	continue;
      }
      extAbsXpos = (*extCell).CellGetXpos();
      extAbsYpos = (*extCell).CellGetYpos();
      extAbsXpos += (*pinPtr).PinGetXOffset();
      extAbsYpos += (*pinPtr).PinGetYOffset();
      totalXForce += (extAbsXpos - absXpos);
      totalYForce += (extAbsYpos - absYpos);
    } NET_END_FOR;
  } CELL_END_FOR;
  
  if (totalXForce < 0) {
    left = true;
  } else if (totalXForce > 0) {
    right = true;
  }
  if (totalYForce > 0) {
    top = true;
  } else if (totalYForce < 0) {
    bot = true;
  }
}

void
findFeasibleRowAndXPosition(vector<uint>& rowCapacities, vector<uint>& leftLast,
			    vector<uint>& rightFirst, 
			    uint cellWidth, uint clusterCellWidth,
			    bool left, bool right, bool top, bool bot, 
			    uint &rowNum, uint &xPosInRow)
{
  vector<int> excessWidth;
  uint numRows;
  int currRowCapacity, newRowWidth;
  int minExcessWidth, overflow;
  uint rowLeftLast, rowRightFirst;
  int idx;
  int selectedRow;
  
  numRows = rowCapacities.size();
  selectedRow = -1;
  xPosInRow = 0; rowNum = 0;
  if (top == true) {
    for (idx = numRows - 1; idx >= 0; idx--) {
      currRowCapacity = rowCapacities[idx];
      newRowWidth = currRowCapacity - cellWidth;
      if (newRowWidth >= 0) {
	selectedRow = idx;
	rowCapacities[idx] -= cellWidth;
	rowNum = idx;
	break;
      } else {
	excessWidth.push_back((newRowWidth - clusterCellWidth));
      }
    }
  } else if (bot == true) {
    for (idx = 0; idx < numRows; idx++) {
      currRowCapacity = rowCapacities[idx];
      newRowWidth = currRowCapacity - cellWidth;
      if (newRowWidth >= 0) {
	selectedRow = idx;
	rowCapacities[idx] -= cellWidth;
	rowNum = idx;
	break;
      } else {
	excessWidth.push_back((newRowWidth - clusterCellWidth));	
      }
    }
  }
  if (selectedRow == -1) {
    minExcessWidth = INT_MAX;
    for (idx = 0; idx < numRows; idx++) {
      if (excessWidth[idx] < minExcessWidth) {
	minExcessWidth = excessWidth[idx];
	selectedRow = idx;
	rowCapacities[idx] -= cellWidth;
      }
    }
  }
  if (left == true) {
    rowLeftLast = leftLast[selectedRow];
    overflow = rowLeftLast + cellWidth;
    if (overflow > clusterCellWidth) {
      xPosInRow = rowLeftLast - (overflow - clusterCellWidth);
    } else {
      xPosInRow = rowLeftLast;
      leftLast[selectedRow] += cellWidth;
    }
  } else if (right == true) {
    rowRightFirst = rightFirst[selectedRow];
    overflow = rowRightFirst - cellWidth;
    if (overflow < 0) {
      xPosInRow = 0;
    } else {
      xPosInRow = overflow;
      rightFirst[selectedRow] -= cellWidth;
    }
  }
}

void
computeForceUpdateHessianForBCells(Design &myDesign, Cell *clusterCell, 
				   vector<Cell *> &cellsToSolve, 
				   map<Net *, uint> &internalNets,
				   map<Cell *, uint> &bCells,
				   vector<uint> &subi_vecx, vector<uint> &subj_vecx, 
				   vector<uint> &sub_vecx, vector<double> &val_vecx,
				   vector<double> &valij_vecx,
				   vector<uint> &subi_vecy, vector<uint> &subj_vecy, 
				   vector<uint> &sub_vecy, vector<double> &val_vecy,
				   vector<double> &valij_vecy)
{
  Cell *bCell;
  double boundaryPointX, boundaryPointY;
  double cellXpos, cellYpos;
  double totalXForce, totalYForce, magnitude;
  double boundRight, boundLeft, boundTop, boundBot;
  double netWeight;
  uint cellWidth, cellHeight;
  uint clusterCellWidth, clusterCellHeight;
  uint bCellIndex;
  uint tempInt;
  char forceDir;

  clusterCellWidth = (*clusterCell).CellGetWidth();
  clusterCellHeight = (*clusterCell).CellGetHeight();

  MAP_FOR_ALL_ELEMS(bCells, Cell *, uint, bCell, tempInt) {
    cellWidth = (*bCell).CellGetWidth();
    cellHeight = (*bCell).CellGetHeight();
    cellXpos = ((double)clusterCellWidth) / 2 - ((double)cellWidth) / 2;
    cellYpos = ((double)clusterCellHeight) / 2 - ((double)cellHeight) / 2;
    myDesign.DesignComputeForceOnCell(bCell, totalXForce, totalYForce, magnitude,
				      forceDir, boundRight, boundLeft, boundTop, boundBot,
				      0, 0, clusterCellWidth, clusterCellHeight, internalNets);
    myDesign.DesignGetBoundaryPoints(cellXpos, cellYpos, magnitude,
				     totalXForce, totalYForce, forceDir, boundLeft, boundRight, 
				     boundTop, boundBot, netWeight, 0, 0, clusterCellWidth, 
				     clusterCellHeight, boundaryPointX, boundaryPointY);
    bCellIndex = cellsToSolve.size();
    subi_vecx.push_back(bCellIndex);
    subj_vecx.push_back(bCellIndex);
    valij_vecx.push_back(netWeight);
    subi_vecy.push_back(bCellIndex);
    subj_vecy.push_back(bCellIndex);
    valij_vecy.push_back(netWeight);
    sub_vecx.push_back(bCellIndex);
    val_vecx.push_back((netWeight * (boundaryPointX / GRID_COMPACTION_RATIO)));
    sub_vecy.push_back(bCellIndex);
    val_vecy.push_back((netWeight * (boundaryPointY / GRID_COMPACTION_RATIO)));
    cellsToSolve.push_back(bCell);
  } END_FOR;
}

void
solveForClusterCells(Design &myDesign, Cell *clusterCell, vector<Cell *> &cellsToSolve, 
		     vector<Net *> &internalNets, map<Cell *, uint> bCells,
		     EnvUnclusterType unClusterType)
{
  Cell *cellPtr;
  Net *netPtr;
  vector<uint> subi_vecx, subj_vecx, sub_vecx;
  vector<uint> subi_vecy, subj_vecy, sub_vecy;
  vector<double> valij_vecx, valij_vecy, val_vecx, val_vecy;
  map<Net *, uint> netsOfClusterMap;
  double constantx, constanty;
  double *bx, *by;
  uint clusterCellXpos, clusterCellYpos;
  uint numCells, i;

  if (unClusterType == ENV_UNCLUSTER_PLACE_INTERNAL) {
    getObjectiveCliqueModelXYForCluster(internalNets, cellsToSolve, subi_vecx, 
       subj_vecx, valij_vecx, sub_vecx, val_vecx, subi_vecy, subj_vecy, valij_vecy, 
       sub_vecy, val_vecy, constantx, constanty);
  } else {
    /* Add the cells in the boundary cell list to the cells to solve list and 
       add ports for boundary cells into the objective function */
    VECTOR_FOR_ALL_ELEMS(internalNets, Net *, netPtr) {
      netsOfClusterMap[netPtr] = 1;
    } END_FOR;
    computeForceUpdateHessianForBCells(myDesign, clusterCell, cellsToSolve, netsOfClusterMap, bCells, 
                          subi_vecx, subj_vecx, sub_vecx, val_vecx, valij_vecx, subi_vecy, subj_vecy, 
	                  sub_vecy, val_vecy, valij_vecy);
  }

  numCells = cellsToSolve.size();
  clusterCellXpos = (*clusterCell).CellGetXpos();
  clusterCellYpos = (*clusterCell).CellGetYpos();
  SpMat matX(subi_vecx, subj_vecx, valij_vecx);
  SpMat matY(subi_vecy, subj_vecy, valij_vecy);
  bx = (double *) malloc(sizeof(double) * numCells);
  by = (double *) malloc(sizeof(double) * numCells);

  for (i = 0; i < numCells; i++) {
    bx[i] = 0.0;
    by[i] = 0.0;
  }
  for (i = 0; i < sub_vecx.size(); i++) {
    bx[sub_vecx[i]] = -val_vecx[i];
  }
  for (i = 0; i < sub_vecy.size(); i++) {
    by[sub_vecy[i]] = -val_vecy[i];
  }
 
  double x[numCells];
  double y[numCells];
  double xpos, ypos;
  double eps=1e-13;
  uint max_iterations = 2000;
  for (i = 0; i < numCells; i++) {
    x[i] = 0.0;
    y[i] = 0.0;
  }

  cghs(numCells, matX, bx, x, eps, false, max_iterations);
  cghs(numCells, matY, by, y, eps, false, max_iterations);
  
  for (i = 0; i < numCells; i++) {
    cellPtr = cellsToSolve[i];
    xpos = x[i] * GRID_COMPACTION_RATIO;
    ypos = y[i] * GRID_COMPACTION_RATIO;
    (*cellPtr).CellSetXpos((dtoi(xpos) + clusterCellXpos));
    (*cellPtr).CellSetYpos((dtoi(ypos) + clusterCellYpos));
  }
}

void
Design::DesignUnclusterCellRowBased(Cell *clusterCell, Cluster *clusterCellPtr)
{
  Cell *thisCell;
  Pin *pinPtr, *connPinPtr;
  Net *netPtr;
  uint clusterXpos, clusterYpos;
  double cellXPos, cellYPos;
  uint cellIndex, numRows, numCells;
  uint idx;

  /* Get the cluster structure corresponding to the current cluster cell */
  Cluster &thisCluster = *clusterCellPtr;
  vector<Cell *> &cellsOfCluster = thisCluster.ClusterGetCellsOfCluster();
  vector<Net *> &netsOfCluster = thisCluster.ClusterGetInternalNets();
  map<Pin*, Pin*> &pinMap = thisCluster.ClusterGetPinMap();
  clusterXpos = (*clusterCell).CellGetXpos();
  clusterYpos = (*clusterCell).CellGetYpos();
  
  numCells = cellsOfCluster.size();
  for (idx = 0; idx < numCells; idx++) {
    Cell &thisCell = *(cellsOfCluster[idx]);
    cellXPos = thisCell.CellGetXposDbl();
    cellYPos = thisCell.CellGetYposDbl();
    thisCell.CellSetXposDbl(clusterXpos + cellXPos);
    thisCell.CellSetYposDbl(clusterYpos + cellYPos);
  }

  /* Un-hide the cells of the cluster and the internal cells and all the pins 
     of the cells of the cluster */
  VECTOR_FOR_ALL_ELEMS(cellsOfCluster, Cell *, thisCell) {
    DesignUnhideCell(thisCell);
    CELL_FOR_ALL_PINS((*thisCell), PIN_DIR_ALL, pinPtr) {
      (*pinPtr).PinSetIsHidden(false);
    } CELL_END_FOR;
  } END_FOR;

  /* Un-hide all the internal nets */
  VECTOR_FOR_ALL_ELEMS(netsOfCluster, Net *, netPtr) {
    DesignUnhideNet(netPtr);
  } END_FOR;

  /* Remove pins of the cluster from the external nets */
  MAP_FOR_ALL_KEYS(pinMap, Pin *, Pin *, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    connectedNet.NetRemovePin(*pinPtr);
    /* Destroy the pin that has been created on the cluster */
    DesignDeletePin(pinPtr);
  } END_FOR;

  /* Clear flag on cluster cell */
  CellClearCluster(clusterCell);
  
  /* Remove the cluster cell from the design database */
  DesignRemoveOneClusterFromDesignDB(clusterCell);
  
  /* Remove the data "Cluster" stored on the cell */
  delete (&thisCluster);

  /* Delete the cluster cell */
  DesignDeleteCell(clusterCell);
}

void
Design::DesignUnclusterCell(Cell *clusterCell, bool noDissolve)
{
  Cell *thisCell;
  Pin *pinPtr, *connPinPtr;
  Net *netPtr;
  map<Cell *, uint> cellLookupMap;
  int singleRowHeight;
  uint clusterHeight, clusterWidth;
  uint lBoundX, uBoundX, lBoundY, uBoundY;
  uint clusterXpos, clusterYpos;
  uint bCellIndex, numRows, numBoundaryCells;
  uint bCellWidth;
  uint rowNum, xPosInRow;
  bool left, right, top, bot;
  bool bCellsPlaced, rowBased;
  uint idx, bCellIdx;
  Env &DesignEnv = DesignGetEnv();

  debug = false;
  /* Get the cluster structure corresponding to the current 
     cluster cell */
  Cluster *clusterCellPtr = (Cluster *)CellGetCluster(clusterCell);
  Cluster &thisCluster = *clusterCellPtr;
  rowBased = thisCluster.ClusterGetRowBased();

  if (rowBased) {
    DesignUnclusterCellRowBased(clusterCell, clusterCellPtr);
    return;
  }

  singleRowHeight = DesignGetSingleRowHeight();
  if (singleRowHeight < 0) {
    _ASSERT_TRUE("All rows in the design should have the same height");
  }

  if (debug) {
    cout << "--------------------------------------" << endl;
    cout << "   UNCLUSTER: " << (*clusterCell).CellGetName() << endl;
    cout << "--------------------------------------" << endl;
  }

  /* Get all the boundary cell and other cell related info */
  clusterWidth = (*clusterCell).CellGetWidth();
  clusterHeight = (*clusterCell).CellGetHeight();
  clusterXpos = (*clusterCell).CellGetXpos();
  clusterYpos = (*clusterCell).CellGetYpos();
  numRows = clusterHeight / singleRowHeight;

  /* Set the bounds for the movement of the cells */
  lBoundX = clusterXpos; lBoundY = clusterYpos;
  uBoundX = clusterXpos + clusterWidth;
  uBoundY = clusterYpos + clusterHeight;

  vector<uint> rowCapacities;
  vector<uint> leftLastX;
  vector<uint> rightFirstX;
  vector<Cell *> &cellsOfCluster = thisCluster.ClusterGetCellsOfCluster();
  vector<Net *> &netsOfCluster = thisCluster.ClusterGetInternalNets();
  vector<Cell *> cellsToSolve;
  vector<uint> &bCellIndices = thisCluster.ClusterGetBCellIndices();
  vector<uint> &rowNums = thisCluster.ClusterGetRowNums();
  vector<uint> &xPosInRows = thisCluster.ClusterGetXPosInRows();
  bCellsPlaced = thisCluster.ClusterGetBCellsPlaced();
  numBoundaryCells = bCellIndices.size();
  double totalXForce, totalYForce;

  /* Get the pin map from the cluster */
  map<Pin*, Pin*> &pinMap = thisCluster.ClusterGetPinMap();
  map<Cell *, uint> boundaryCells;

  /* Initialize the row capacities to the width of the cluster */
  for(idx = 0; idx < numRows; idx++) {
    rowCapacities.push_back(clusterWidth);
    rightFirstX.push_back(clusterWidth);
    leftLastX.push_back(0);
  }

  /* Create a map of boundary cells */
  VECTOR_FOR_ALL_ELEMS(bCellIndices, uint, bCellIndex) {
    Cell *bCell = cellsOfCluster[bCellIndex];
    boundaryCells[bCell] = 1;
  } END_FOR;

  /* Add all cells to a lookup map */
  VECTOR_FOR_ALL_ELEMS(cellsOfCluster, Cell*, thisCell) {
    cellLookupMap[thisCell] = 1;
    _KEY_DOES_NOT_EXIST(boundaryCells, thisCell) {
      cellsToSolve.push_back(thisCell);
    }
  } END_FOR;

  //  if (!bCellsPlaced) {
  if (0) {
    /* Place the boundary cells of the cluster here */
    if (debug) {
      cout << "--------------------------------------" << endl;
      cout << "   PLACE BOUNDARY CELLS               " << endl;
      cout << "--------------------------------------" << endl;
    }
    if (DesignEnv.EnvGetUnclusterType() == ENV_UNCLUSTER_PLACE_BOUNDARY) {
      for (idx = 0; idx < numBoundaryCells; idx++) {
	Cell &bCell = *(cellsOfCluster[idx]);
	bCellWidth = bCell.CellGetWidth();
	rowNum = 0; xPosInRow = 0;
	/* Compute force on the boundary cell */
	computeForceOnCellOfCluster(clusterCell, cellLookupMap, left, right, top, 
				    bot);
	/* Find a feasible position for the cell inside the cluster */
	findFeasibleRowAndXPosition(rowCapacities, leftLastX, rightFirstX,
				    bCellWidth, clusterWidth, left, right, top, 
				    bot, rowNum, xPosInRow);
	bCell.CellSetXpos((clusterXpos + xPosInRow));
	bCell.CellSetYpos((clusterYpos + (rowNum * singleRowHeight)));
      }
    } 
  } else {
    if (debug) {
      cout << "--------------------------------------" << endl;
      cout << "   SHIFT CELLS TO ACTUAL POSITIONS    " << endl;
      cout << "--------------------------------------" << endl;
    }
    for (idx = 0; idx < numBoundaryCells; idx++) {
      bCellIdx = bCellIndices[idx];
      Cell &bCell = *(cellsOfCluster[bCellIdx]);
      rowNum = rowNums[idx]; xPosInRow = xPosInRows[idx]; 
      bCell.CellSetXpos((clusterXpos + xPosInRow));
      bCell.CellSetYpos((clusterYpos + (rowNum * singleRowHeight)));
    }   
  }

  if (!noDissolve) {
    /* Un-hide the cells of the cluster and the internal cells and all the pins 
       of the cells of the cluster */
    VECTOR_FOR_ALL_ELEMS(cellsOfCluster, Cell *, thisCell) {
      DesignUnhideCell(thisCell);
      CELL_FOR_ALL_PINS((*thisCell), PIN_DIR_ALL, pinPtr) {
	(*pinPtr).PinSetIsHidden(false);
      } CELL_END_FOR;
    } END_FOR;

    /* Un-hide all the internal nets */
    VECTOR_FOR_ALL_ELEMS(netsOfCluster, Net *, netPtr) {
      DesignUnhideNet(netPtr);
    } END_FOR;

    /* Solve for internal cells of the cluster */
    //    solveForClusterCells((*this), clusterCell, cellsToSolve, netsOfCluster, 
    //			 boundaryCells, DesignEnv.EnvGetUnclusterType());

    /* Remove pins of the cluster from the external nets */
    MAP_FOR_ALL_KEYS(pinMap, Pin *, Pin *, pinPtr) {
      Net &connectedNet = (*pinPtr).PinGetNet();
      connectedNet.NetRemovePin(*pinPtr);
      /* Destroy the pin that has been created on the cluster */
      DesignDeletePin(pinPtr);
    } END_FOR;


    /* Clear flag on cluster cell */
    CellClearCluster(clusterCell);
  
    /* Remove the cluster cell from the design database */
    DesignRemoveOneClusterFromDesignDB(clusterCell);
  
    /* Remove the data "Cluster" stored on the cell */
    delete (&thisCluster);

    /* Delete the cluster cell */
    DesignDeleteCell(clusterCell);
  }

  debug = false;
}

