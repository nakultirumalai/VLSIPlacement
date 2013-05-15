# include <Legalize.h>

using namespace std;

void 
LegalizeGetAllBoundingBoxes(vector<PhysRow*> &allPhysRows, 
			    vector<vector<int> > &allRows)
{
  PhysRow* Obj;
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    vector<int> v;
    Obj->PhysRowGetBoundingBox(v);
    allRows.push_back(v);
  }END_FOR;
}

void 
LegalizeCheckLegality(Cell* cell, vector<PhysRow*> &allPhysRows,
		      vector<vector<int> > &allRows)
{
  /* Get the values the coordinates, height and width of the cells */
  int cellX = cell->CellGetXpos();
  int cellY = cell->CellGetYpos();
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();
  vector<int> Obj; 
  rowOrientation rowType = (allPhysRows[0]->PhysRowGetType());
  
	
  VECTOR_FOR_ALL_ELEMS(allRows, vector<int>, Obj){
      /* Indicates one subRow present in the entire row */
    if ((Obj.size() == 4)){
      if (rowType == HORIZONTAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellWidth;
	int cellYend = cellY + cellHeight;
	/* Checking X and Y Legality in the row,
	   also checking for macros which may span multiple rows */
	if ((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	    (cellXend <= topX))
	  cell->CellSetXIsLegal(true);
	  
	if((cellY == botY) && (cellYend > botY) && 
	   ((cellHeight % (topY - botY)) == 0))
	  cell->CellSetYIsLegal(true);
      }
      else if (rowType == VERTICAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellHeight;
	int cellYend = cellY + cellWidth;
	if ((cellX == botX) && (cellXend > botX) && 
	    ((cellHeight % (topX-botX)) == 0))
	  cell->CellSetXIsLegal(true);
	
	if ((cellY >= botY) && (cellY < topY) &&
	    (cellYend > cellY) && (cellYend <= topY))
	  cell->CellSetYIsLegal(true);
	
      }
    }
  }END_FOR;
}

void 
LegalizeSnapToNearestRows(Cell* cell, vector<PhysRow*> &allPhysRows, rowOrientation rowType)
{
  PhysRow* Obj;
  vector<int> RowCoordinates;
  vector<int> RowOtherBounds;
  int min = 0;
  int max;
  int nearRows[2];
  int otherCoordinateBegin;
  int otherCoordinateEnd;
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();

  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    RowCoordinates.push_back(Obj->PhysRowGetCoordinate());
    map<unsigned int, unsigned int> subRows = (Obj->PhysRowGetSubRows());
    unsigned int Key,Value;
    unsigned int siteSpacing = (Obj->PhysRowGetSiteSpacing());
    MAP_FOR_ALL_ELEMS(subRows, unsigned int, unsigned int, Key, Value){
      RowOtherBounds.push_back(Key);
      RowOtherBounds.push_back(Value*siteSpacing);
    }END_FOR;      
  }END_FOR;
  otherCoordinateBegin = RowOtherBounds.front();
  otherCoordinateEnd = RowOtherBounds.back();
  
  max = RowCoordinates.size();
  
  //cout<<"Cell was at ("<<(cell->CellGetXpos())<<","<<(cell->CellGetYpos())<<")"<<endl;
  if(rowType == HORIZONTAL){
     int key = cell->CellGetYpos();
     int cellX = cell->CellGetXpos();
     int cellXend = cellX + (cell->CellGetWidth());
     findRowCoordinates(RowCoordinates, min, max, key, nearRows);
     
     /* Cell has violated X-bounds */
     
     if(!(cell->CellXIsLegal())){
       if(cellX < otherCoordinateBegin){
	 cellX = cellX + (otherCoordinateBegin - cellX);
	 cell->CellSetXpos(cellX);
	 cout<<"XLegalization...done"<<endl;
       }
       else if(cellXend > otherCoordinateEnd){
	 cellX = cellX - (cellXend - otherCoordinateEnd);
	 cell->CellSetXpos(cellX);
	 cout<<"XLegalization...done"<<endl;
       }
       else if((cellX < otherCoordinateBegin) &&
	       (cellXend > otherCoordinateEnd))
	 cout<<"Illegal cell found. Width of cell exceeds max."<<endl;
     }
     
     /* Cell has violated Y-bounds */

     if(!(cell->CellYIsLegal())){
       if(fabs(key-nearRows[1])>=fabs(key-nearRows[0]))
	 cell->CellSetYpos(nearRows[0]);
       else
	 cell->CellSetYpos(nearRows[1]);
     }
       
  } else if (rowType==VERTICAL){
     int key=cell->CellGetXpos();
     int cellY = cell->CellGetYpos();
     int cellYend = cellY + (cell->CellGetHeight());
     
     /* Cell has violated X-bounds */

     if (!(cell->CellXIsLegal())){
       findRowCoordinates(RowCoordinates, min, max, key, nearRows);
       if (fabs(key-nearRows[1])>=fabs(key-nearRows[0])){
	 cell->CellSetXpos(nearRows[0]);
       } else{
	 cell->CellSetXpos(nearRows[1]);
       }
     }
     
     /* Cell has violated Y-bounds */
     
     if(!(cell->CellYIsLegal())){
       if(cellY < otherCoordinateBegin){
	 cellY = cellY + (otherCoordinateBegin - cellY);
	 cell->CellSetYpos(cellY);
       }
       else if(cellYend > otherCoordinateEnd){
	 cellY = cellY - (otherCoordinateEnd - cellYend);
	 cell->CellSetYpos(cellY);
       }
       else if((cellY < otherCoordinateBegin) &&
	       (cellYend > otherCoordinateEnd))
	 cout<<"Illegal cell found. Width of cell exceeds max."<<endl;
     }
  }
  //cout<<"Cell is now at ("<<(cell->CellGetXpos())<<","<<(cell->CellGetYpos())<<")"<<endl;
}

void 
findRowCoordinates(vector<int> &RowCoordinates, int min, int max, int key, int nearRows[])
{
  sort(RowCoordinates.begin(), RowCoordinates.end());
  int mid=(min+max)/2;
  while(max>=min){
    if(key>RowCoordinates.back()){
      nearRows[0]=RowCoordinates.back();
      nearRows[1]=RowCoordinates.back();
      break;
    }
    else if(key<RowCoordinates[0]){
      nearRows[0]=RowCoordinates[0];
      nearRows[1]=RowCoordinates[0];
      break;
    } 
    else if(RowCoordinates[mid]<key){
      min=mid+1;
      /* Set Lower Bound */
      nearRows[0]=RowCoordinates[mid];
      mid=(min+max)/2;
    }
    else if(RowCoordinates[mid]>key){
      max=mid-1;
      /* Set Higher Bound */
      nearRows[1]=RowCoordinates[mid];
      mid=(min+max)/2;
    }
  }
}

bool
descendingSupp(PhysRow* row1, PhysRow* row2)
{
  return((row1->PhysRowGetSupply()) > (row2->PhysRowGetSupply()));
}

PhysRow* 
LegalizeGetRowByIndex(vector<PhysRow*> &allPhysRows, int index)
{
  PhysRow* Obj;
  PhysRow* foundRow;
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    if (Obj->PhysRowGetIndex() == index){
      foundRow = Obj;
      break;
    }
  } END_FOR;
  return foundRow;
}
      
bool
ascendingX(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetXpos()) > (cell2->CellGetXpos()));
}

void
LegalizeCellGetCost(Cell &thisCell, double& newXPos, double& newYPos,
	    double &magnitude, double &totalXForce, double &totalYForce)
{
  Pin *pini, *pinj;
  double celliXPos, celliYPos, celljXPos, celljYPos;
  double piniXOffset, piniYOffset, pinjXOffset, pinjYOffset;
  double diffCellXPos, diffCellYPos, diffPinXOffPos, diffPinYOffPos;
  double totalXDiffDist, totalYDiffDist;
  double netWeight;
  uint maxx, maxy;
  
  totalXForce = 0.0;
  totalYForce = 0.0;
  magnitude = 0.0;
  diffCellXPos = 0.0;
  diffCellYPos = 0.0;

  CELL_FOR_ALL_PINS(thisCell, PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    netWeight = connectedNet.NetGetWeight();
    piniXOffset = (*pini).PinGetXOffset();
    piniYOffset = (*pini).PinGetYOffset();
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      Cell &connectedCell = ((*pinj).PinGetParentCell());
      if (pinj == pini) continue;
      pinjXOffset = (*pinj).PinGetXOffset();
      pinjYOffset = (*pinj).PinGetYOffset();
      diffCellXPos -= netWeight * (celliXPos + piniXOffset);
      diffCellYPos -= netWeight * (celliYPos + piniYOffset);
      celljXPos = connectedCell.CellGetXpos();
      celljYPos = connectedCell.CellGetYpos();
      diffCellXPos += netWeight * (celljXPos + pinjXOffset);
      diffCellYPos += netWeight * (celljYPos + pinjYOffset);
    } NET_END_FOR;
  } CELL_END_FOR;

  totalXForce = diffCellXPos; 
  totalYForce = diffCellYPos;
  int intXForce, intYForce;
  intXForce = dtoi(totalXForce); intYForce = dtoi(totalYForce);
  magnitude = sqrt((totalXForce * totalXForce + totalYForce * totalYForce));
}

 
bool 
decForce(const pair<Cell*, int> &i, const pair<Cell*, int> &j)
{
  return i.second > j.second;
}

void
LegalizeDesign(Design &myDesign)
{
  /* Get all the physical rows in Design */
  vector<PhysRow*> allPhysRows;
  allPhysRows  = myDesign.DesignGetRows();
  
  /* Get bounding boxes for all subrows in all rows in Design */
  vector<vector<int> > allRowBounds;
  LegalizeGetAllBoundingBoxes(allPhysRows, allRowBounds);
  rowOrientation rowType=((allPhysRows[0])->PhysRowGetType());
  string CellName;
  Cell* CellPtr;
  
  /* Checking the Legality of all Cells and setting the variable isLegal
     for all cells accordingly */ 
  
  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal()))
      LegalizeCheckLegality(CellPtr, allPhysRows, allRowBounds);
  }DESIGN_END_FOR;
  
  /* Legalizing design so that every cell is contained completely in some row */

  
  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal())){
      if((CellPtr->CellXIsLegal()) && (CellPtr->CellYIsLegal())){
	// cout<<"LEGALCell: "<<CellName<<"is legal"<<endl;
      }
      else{
	//cout<<"Cell: "<<CellName<< "at ("<< CellPtr->CellGetXpos() 
	//  <<","<<CellPtr->CellGetYpos()<<" ) is notlegal"<<endl;
	//cout<<"Legalizing..."<<endl;
	LegalizeSnapToNearestRows(CellPtr, allPhysRows, rowType);
	//cout<<"Cell: "<<CellName<< "is now at ("<< CellPtr->CellGetXpos() 
	//  <<","<<CellPtr->CellGetYpos()<<" )"<<endl;
      }
    }
  }DESIGN_END_FOR;
  
  myDesign.DesignAddAllCellsToPhysRows();

  /* Get the zones of each row */
  int rowBegin = ((allPhysRows[0])->PhysRowGetRowBegin());
  int rowEnd = ((allPhysRows[0])->PhysRowCalculateRowEnd());
  PhysRow* Obj;				  
  /*
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    cout << "Row" << i << " has supply " << Obj->PhysRowGetSupply() << " and index " << Obj->PhysRowGetIndex() << endl; 
  } END_FOR;
  */
  
  /******************************************************************************/
  
  Cell* cObj;
  /* Sort all the rows in non-increasing order of supply */
  sort(allPhysRows.begin(), allPhysRows.end(), descendingSupp);
  int lastIdx = allPhysRows.size() - 1;
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);\
    
    sort(cellsInRow.begin(), cellsInRow.end(), ascendingX);

    int supply = Obj->PhysRowGetSupply();
    int rowIndex = Obj->PhysRowGetIndex();
    int rowWidth = Obj->PhysRowGetBoundingBoxWidth();
    
    if (supply > 0){
      if ((rowIndex != 0) && (rowIndex != lastIdx)) {
	/* Calculate the cost of cells to move to prev and next row */
	PhysRow* prevRow = LegalizeGetRowByIndex(allPhysRows, (rowIndex - 1));
	PhysRow* nextRow = LegalizeGetRowByIndex(allPhysRows, (rowIndex + 1));
	int prevSupply = prevRow->PhysRowGetSupply();
	int nextSupply = nextRow->PhysRowGetSupply();
	if (nextSupply < prevSupply){
	  /* Push cells to next row */
	  int lockedWidth = 0;
	  vector<pair<Cell*, int> > forceOnCells;
	  VECTOR_FOR_ALL_ELEMS (cellsInRow, Cell*, cObj) {
	    string cellName = cObj->CellGetName();
	    
	    if (!CellIsLocked(cObj)) {
	      double newXPos, newYPos, magnitude, totalXForce, totalYForce;
	      newXPos = cObj->CellGetXpos();
	      newYPos = nextRow->PhysRowGetCoordinate();
	      LegalizeCellGetCost(*cObj, newXPos, newYPos, magnitude, totalXForce, totalYForce);
	      forceOnCells.push_back(make_pair(cObj, totalYForce));
	    }
	    else {
	      int cellWidth = cObj->CellGetWidth();
	      lockedWidth += cellWidth;
	    }	      
	  } END_FOR;
	  
	  /* Sort the vector of forces in decreasing order */
	  sort(forceOnCells.begin(), forceOnCells.end(), decForce);
	  int availWidth = rowWidth - lockedWidth;
	  int widthInserted = 0;
	  int idx = 0;
	  while (forceOnCells.size() > 0) {
	    Cell* thisCell = forceOnCells[idx].first;
	    int cellWidth = thisCell->CellGetWidth();
	    widthInserted += cellWidth;
	    if (widthInserted > availWidth){
	      break;
	    } else {
	      CellSetIsLocked(thisCell);
	      forceOnCells.erase(forceOnCells.begin() + idx);
	      idx--;
	    }
	    idx++;
	  } 
	  if (forceOnCells.size()) {
	    for (int idx = 0; idx < forceOnCells.size(); idx++) {
	      Cell* thisCell = forceOnCells[idx].first;
	      int yPos = nextRow->PhysRowGetCoordinate();
	      thisCell->CellSetYpos(yPos);
	      nextRow->PhysRowAddCellToRow(thisCell);
	      CellSetIsLocked(thisCell);
	      Obj->PhysRowRemoveCellFromRow(thisCell);
	    }
	  }
	} else if (nextSupply >= prevSupply){
	  /* Push cells to next row */
	  int lockedWidth = 0;
	  vector<pair<Cell*, int> > forceOnCells;
	  VECTOR_FOR_ALL_ELEMS (cellsInRow, Cell*, cObj) {
	    if (!CellIsLocked(cObj)) {
	      double newXPos, newYPos, magnitude, totalXForce, totalYForce;
	      newXPos = cObj->CellGetXpos();
	      newYPos = prevRow->PhysRowGetCoordinate();
	      LegalizeCellGetCost(*cObj, newXPos, newYPos, magnitude, totalXForce, totalYForce);
	      forceOnCells.push_back(make_pair(cObj, totalYForce));
	    }
	    else {
	      int cellWidth = cObj->CellGetWidth();
	      lockedWidth += cellWidth;
	    }	      
	  } END_FOR;
	  
	  /* Sort the vector of forces in decreasing order */
	  sort(forceOnCells.begin(), forceOnCells.end(), decForce);
	  int availWidth = rowWidth - lockedWidth;
	  int widthInserted = 0;
	  int idx = 0;
	  while (forceOnCells.size() > 0) {
	    Cell* thisCell = forceOnCells[idx].first;
	    int cellWidth = thisCell->CellGetWidth();
	    widthInserted += cellWidth;
	    if (widthInserted > availWidth){
	      break;
	    } else {
	      CellSetIsLocked(thisCell);
	      forceOnCells.erase(forceOnCells.begin() + idx);
	      idx--;
	    }
	    idx++;
	  } 
	  if (forceOnCells.size()) {
	    for (int idx = 0; idx < forceOnCells.size(); idx++) {
	      Cell* thisCell = forceOnCells[idx].first;
	      int yPos = prevRow->PhysRowGetCoordinate();
	      thisCell->CellSetYpos(yPos);
	      prevRow->PhysRowAddCellToRow(thisCell);
	      CellSetIsLocked(thisCell);
	      Obj->PhysRowRemoveCellFromRow(thisCell);
	    }
	  }
	}
      } else if (rowIndex == 0) {
	/* Calculate the cost of cells to move to prev and next row */
	PhysRow* nextRow = LegalizeGetRowByIndex(allPhysRows, (rowIndex + 1));
	int nextSupply = nextRow->PhysRowGetSupply();
	
	int lockedWidth = 0;
	vector<pair<Cell*, int> > forceOnCells;
	VECTOR_FOR_ALL_ELEMS (cellsInRow, Cell*, cObj) {
	  if (!CellIsLocked(cObj)) {
	    double newXPos, newYPos, magnitude, totalXForce, totalYForce;
	    newXPos = cObj->CellGetXpos();
	    newYPos = nextRow->PhysRowGetCoordinate();
	    LegalizeCellGetCost(*cObj, newXPos, newYPos, magnitude, totalXForce, totalYForce);
	    forceOnCells.push_back(make_pair(cObj, totalYForce));
	  }
	  else {
	    int cellWidth = cObj->CellGetWidth();
	    lockedWidth += cellWidth;
	  }	      
	} END_FOR;
	
	/* Sort the vector of forces in decreasing order */
	sort(forceOnCells.begin(), forceOnCells.end(), decForce);
	int availWidth = rowWidth - lockedWidth;
	int widthInserted = 0;
	int idx = 0;
	while (forceOnCells.size() > 0) {
	  Cell* thisCell = forceOnCells[idx].first;
	  int cellWidth = thisCell->CellGetWidth();
	  widthInserted += cellWidth;
	  if (widthInserted > availWidth){
	    break;
	  } else {
	    CellSetIsLocked(thisCell);
	    forceOnCells.erase(forceOnCells.begin() + idx);
	    idx--;
	  }
	  idx++;
	}
	if (forceOnCells.size()) {
	  for (int idx = 0; idx < forceOnCells.size(); idx++) {
	    Cell* thisCell = forceOnCells[idx].first;
	    int yPos = nextRow->PhysRowGetCoordinate();
	    thisCell->CellSetYpos(yPos);
	    nextRow->PhysRowAddCellToRow(thisCell);
	    CellSetIsLocked(thisCell);
	    Obj->PhysRowRemoveCellFromRow(thisCell);
	  }
	}
      } else if(rowIndex == lastIdx){
	PhysRow* prevRow = LegalizeGetRowByIndex(allPhysRows, (rowIndex - 1));
	int prevSupply = prevRow->PhysRowGetSupply();
	int lockedWidth = 0;
	vector<pair<Cell*, int> > forceOnCells;
	VECTOR_FOR_ALL_ELEMS (cellsInRow, Cell*, cObj) {
	  if (!CellIsLocked(cObj)) {
	    double newXPos, newYPos, magnitude, totalXForce, totalYForce;
	    newXPos = cObj->CellGetXpos();
	    newYPos = prevRow->PhysRowGetCoordinate();
	    LegalizeCellGetCost(*cObj, newXPos, newYPos, magnitude, totalXForce, totalYForce);
	    forceOnCells.push_back(make_pair(cObj, totalYForce));
	  }
	  else {
	    int cellWidth = cObj->CellGetWidth();
	    lockedWidth += cellWidth;
	  }	      
	} END_FOR;
	
	/* Sort the vector of forces in decreasing order */
	sort(forceOnCells.begin(), forceOnCells.end(), decForce);
	int availWidth = rowWidth - lockedWidth;
	int widthInserted = 0;
	int idx = 0;
	while (forceOnCells.size() > 0) {
	  Cell* thisCell = forceOnCells[idx].first;
	  int cellWidth = thisCell->CellGetWidth();
	  widthInserted += cellWidth;
	  if (widthInserted > availWidth){
	    break;
	  } else {
	    CellSetIsLocked(thisCell);
	    forceOnCells.erase(forceOnCells.begin() + idx);
	    idx--;
	  }
	  idx++;
	} 
	if (forceOnCells.size()) {
	  for (int idx = 0; idx < forceOnCells.size(); idx++) {
	    Cell* thisCell = forceOnCells[idx].first;
	    int yPos = prevRow->PhysRowGetCoordinate();
	    thisCell->CellSetYpos(yPos);
	    prevRow->PhysRowAddCellToRow(thisCell);
	    CellSetIsLocked(thisCell);
	    Obj->PhysRowRemoveCellFromRow(thisCell);
	  }
	}
      }
      //cout << "In a supply row " << endl;
    } else {
      //cout << "In a demand row " << Obj->PhysRowGetIndex() <<endl;
    }
  } END_FOR;
  
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);
    sort(cellsInRow.begin(), cellsInRow.end(), ascendingX);
    Cell* cell;
    bool firstCell = true;
    int nextCellBegin;
    VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cell){
      if (firstCell){
	int cellBegin = cell->CellGetXpos();
	nextCellBegin = cellBegin + cell->CellGetWidth();
	firstCell = false;
      } else {
	cell->CellSetXpos(nextCellBegin);
	int cellWidth = cell->CellGetWidth();
	nextCellBegin += cellWidth;
      }
    } END_FOR;
       
  } END_FOR;

  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    vector<Cell*> cellsInRow;
    
    Obj->PhysRowGetCellsInRow(cellsInRow);
    sort(cellsInRow.begin(), cellsInRow.end(), ascendingX);
    if (cellsInRow.size()){
      cout << " ROW: " << i << "," << endl;
      Cell* cell;
      VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cell){
	cout << cell->CellGetName() << " (" << cell->CellGetXpos() << "," << cell->CellGetYpos() << ")   ";
      } END_FOR;
      cout << endl;
    }
  } END_FOR;
}



  /*
  int count = 0;
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);
    count += cellsInRow.size() ;
    //cout << "Row " << i <<  " has supply " << Obj->PhysRowGetSupply() << endl;
    cout << "Row Width = " << Obj->PhysRowGetBoundingBoxWidth() << endl;
    Cell* cell;
    int cellWidth = 0;
    VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cell){
      cellWidth += cell->CellGetWidth();
      cout << cell->CellGetName() <<  ", " << cell->CellGetWidth() << "   " << endl;
    } END_FOR;
    cout << " Total cell Width = " << cellWidth << endl;
   
  } END_FOR;
  
  cout << "Total number of cells in design : " << count << endl;
  */
        
