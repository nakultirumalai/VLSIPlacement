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
			  int siteWidth, int &X, int &Y)
{
  int modVal, halfVal;
  int quotient;
  
  /* Optimal rounded off value for X */
  quotient = tempX / siteWidth;
  modVal = (int)tempX % siteWidth;
  halfVal = siteWidth / 2;
  if (modVal > halfVal) {
    X = (quotient + 1) * siteWidth;
  } else {
    X = quotient * siteWidth;
  }
  
  /* Optimal rounded off value for Y */
  quotient = tempY / rowHeight;
  modVal = (int)tempY % rowHeight;
  halfVal = rowHeight / 2;
  if (modVal > halfVal) {
    Y = (quotient + 1) * rowHeight;
  } else {
    Y = quotient * rowHeight;
  }
}

void
FDPGetOptLocation(vector<void*> &connectedNodes, vector<Cell*> &movableCells,
		  vector<Cell*> &fixedCells, map<Cell *, bool> &mapOfCells,
		  HyperGraph &myGraph, int rowHeight,
		  int siteWidth, Cell *thisCell, int &x, int &y)
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
  VECTOR_FOR_ALL_ELEMS(connectedNodes, void*, thisNode) {
    thisCell = (Cell*)thisNode;
    _KEY_DOES_NOT_EXIST(mapOfCells, thisCell) {
      continue;
    } else {
      Xj = (int)(*thisCell).CellGetXpos();
      Yj = (int)(*thisCell).CellGetYpos();
      edgeIdx = myGraph.NodesAreAdjacent(thisNode, seedNode);
      edgeWeight = myGraph.GetEdgeWeight(edgeIdx);
      sumWDX += (Xj * edgeWeight);
      sumWDY += (Yj * edgeWeight);
      sumW += edgeWeight;
    }
  } END_FOR;
  
  tempX = sumWDX / sumW;
  tempY = sumWDY /sumW;
  
  /* round off the x and y values to the nearest site location */
  FDPGetRoundedSiteLocation(tempX, tempY, rowHeight, siteWidth, x, y);
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
  
  if (topY <= (numRows - 1)) {
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
		     int numSitesInRow, int numRows)
{
  queue<FDPSite*> myQueue;
  vector<int> fourNeighbours;
  FDPSite *selectedSite;
  bool foundVacantSite;
  int siteNum, rowNum;
  int newSiteNum, newRowNum;
  int i;

  siteNum = (*thisSite).FDPSiteGetSiteNum();
  rowNum = (*thisSite).FDPSiteGetRowNum();
  foundVacantSite = false;
  FDPGetFourNeighbours(siteNum, rowNum, numSitesInRow, numRows, fourNeighbours);
  while (!(fourNeighbours.size())) {
    for(i = 0; i < fourNeighbours.size(); i+=2) {
      newSiteNum = fourNeighbours[i];
      newRowNum = fourNeighbours[i+1];
      selectedSite = gridMatrix[newRowNum][newSiteNum];
      //if ((*selectedSite).FDPSiteGetIsLocked()) {
      if ((*selectedSite).FDPSiteGetHasCell()) {
	myQueue.push(selectedSite);
      } else {
	foundVacantSite = true;
	break;
      }
    } 

    if (foundVacantSite) {
      break;
    } 
    thisSite = myQueue.front();
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
		 int siteWidth, int rowHeight)
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
  uint i, j;
  uint fixedCellNum;
  uint cellNum;
  bool end_ripple;
  
  rowCapInRow = numSitesInRow * siteWidth;
  rowEnd = numSitesInRow * siteWidth;
  
  /* Create a grid containing all the sites for a design */
  for (i = 0; i<numRows; i++) {
    siteYpos = i * rowHeight;
    for (j = 0; j<numSitesInRow; j++) {
      siteXpos = j * siteWidth;
      thisSite = new FDPSite();
      (*thisSite).FDPSiteSetSiteNum(j);
      (*thisSite).FDPSiteSetRowNum(i);
      (*thisSite).FDPSiteSetXpos(siteXpos);
      (*thisSite).FDPSiteSetYpos(siteYpos);
      oneRow.push_back(thisSite);
    }
    gridMatrix.push_back(oneRow);
    rowCap.push_back(rowCapInRow);
    oneRow.clear();
  }
    
  
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

  VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, thisCell) {
    mapOfCells[thisCell] = true;
  } END_FOR;

  if (!(topLevel)) {
    lastPlacedCell = 0;
    /* Initial placement for movable cells */
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
  }
  
  iter_count = 0;
  iter_limit = 10;
  abort_count = 0;
  /* Do iterative refinement on the initial placement using a force directed heuristic */
  counter = 0;
  while(iter_count < iter_limit) {
    if (counter >= movableCells.size()) {
      counter = 0;
      usedCells.clear();
    } 
    /* Select any cell from the list of movable cells */
    thisCell = movableCells[counter++];
    _KEY_EXISTS(usedCells, thisCell) {
      continue;
    } else {
      /* Add cell to a map to skip in next iteration */
      usedCells[thisCell] = true;
      
      /* Change the location of that cell to vacant */
      thisSite = FDPGetSiteOfCell(gridMatrix, thisCell, siteWidth, rowHeight);
      (*thisSite).FDPSiteRemoveCell();
      while(!end_ripple) {
	thisNode = (void*)thisCell;
	connectedNodes = myGraph.GetConnectedNodes(thisNode);
      
	/* Get equilibrium position of cell */
	FDPGetOptLocation(connectedNodes, movableCells, fixedCells, mapOfCells,
			  myGraph, rowHeight, siteWidth, thisCell, siteXpos, siteYpos);
	siteNum = siteXpos / siteWidth;
	rowNum = siteYpos / rowHeight;
	thisSite = gridMatrix[rowNum][siteNum];
      
	if ((*thisSite).FDPSiteGetIsLocked()) {
	  /* The equilibrium position for the cell is locked. So choose a vacant
	     site nearby 
	  */
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
	    end_ripple = true;
	    abort_count++;
	    if (abort_count > abort_limit) {
	      FDPClearAllLocks(movableCells, gridMatrix, rowHeight, siteWidth);
	      iter_count++;
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
	
	  if (nextCell != NIL(Cell*)) {
	    /* Indicates site is occupied. So choose the displaced cell to moved next */
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
    }
  }
}

