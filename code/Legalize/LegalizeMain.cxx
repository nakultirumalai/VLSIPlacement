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
  return((PhysRowGetSupply(row1)) > (PhysRowGetSupply(row2)));
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

bool
ascendingY(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetYpos()) > (cell2->CellGetYpos()));
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

bool 
incForce(const pair<Cell*, int> &i, const pair<Cell*, int> &j)
{
  return i.second < j.second;
}

void
LegalizeDesign(Design &myDesign)
{
  /* Get all the physical rows in Design */
  vector<PhysRow*> allPhysRows;
  vector<PhysRow*> copyAllRows;
  allPhysRows  = myDesign.DesignGetRows();
  copyAllRows = allPhysRows;

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
  vector<Cell*> allCells;
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);
    allCells.insert(allCells.end(),cellsInRow.begin(), cellsInRow.end());
  } END_FOR;
  //cout << "Total numCell : " << allCells.size() << endl;
  vector<Cell*> allCellsCopy = allCells;
  int k1 = 2;
  int k2 = 2;
  sort(allCells.begin(), allCells.end(), ascendingY);
  int totalWidth = 0;
  int size = allPhysRows.size();
  
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    
    Obj->PhysRowRemoveAllCells();
    int rowWidth = Obj->PhysRowGetBoundingBoxWidth();
    Cell* cellObj;
    vector<Cell*> toPlace;
    VECTOR_FOR_ALL_ELEMS (allCells, Cell*, cellObj) {
      int cellWidth = cellObj->CellGetWidth();
      totalWidth += cellWidth;
      if (totalWidth < (k2 * rowWidth)){
	toPlace.push_back(cellObj);
      } else {
	break;
      }
    } END_FOR;
    totalWidth = 0;
    /* Calculate cost for all the candidate cells */
    vector<pair<Cell*, int> > costForCell;
    VECTOR_FOR_ALL_ELEMS (toPlace, Cell*, cellObj) {
      double newXPos = (double)cellObj->CellGetXpos();
      double newYPos = (double)Obj->PhysRowGetCoordinate();
      double magnitude, totalXForce, totalYForce;
      LegalizeCellGetCost(*cellObj, newXPos, newYPos, magnitude, totalXForce, totalYForce);
      pair<Cell*, int> thisCell;
      thisCell.first = cellObj;
      thisCell.second = totalYForce;
      costForCell.push_back(thisCell);
    } END_FOR;
    sort(costForCell.begin(), costForCell.end(), incForce);
    int insertWidth = 0;
    for (int idx = 0; idx < costForCell.size(); idx++) {
      Cell* thisCell = costForCell[idx].first;
      int cellWidth = thisCell->CellGetWidth();
      insertWidth += cellWidth;
      if (insertWidth > rowWidth){
	break;
      } else {
	int yPos = Obj->PhysRowGetCoordinate();
	thisCell->CellSetYpos(yPos);
	Obj->PhysRowAddCellToRow(thisCell);
	allCells.erase(remove(allCells.begin(), allCells.end(), thisCell),allCells.end());

      }
    }
    //cout << "After remove " << allCells.size() << endl;
  } END_FOR;
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj){
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);
    sort(cellsInRow.begin(), cellsInRow.end(), ascendingX);
    Cell* cellObj;
    bool firstCell = true;
    int nextBegin;
    VECTOR_FOR_ALL_ELEMS (cellsInRow, Cell*, cellObj){
      int cellX = cellObj->CellGetXpos();
      if (firstCell) {
	cellObj->CellSetXpos(0);
	nextBegin = cellObj->CellGetWidth();
	firstCell = false;
      } else{
	cellObj->CellSetXpos(nextBegin);
	nextBegin += cellObj->CellGetWidth();
      }
    } END_FOR;
  } END_FOR;

  int totalCellCount = 0;
  int cellCount = 0;
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj) {
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);
    sort(cellsInRow.begin(), cellsInRow.end(), ascendingX);
    cellCount = cellsInRow.size();
    totalCellCount += cellCount;
    Cell* cellObj;
    //cout << "Cells n Row " << i << " are : ";
    VECTOR_FOR_ALL_ELEMS (cellsInRow, Cell*, cellObj){
      //cout << cellObj->CellGetName() << ", " << cellObj->CellGetXpos() << ", " << cellObj->CellGetYpos();
    } END_FOR;
    //    cout << endl;
    //    cout << " NUMBER OF CELLS: " << cellCount << endl;
  } END_FOR;
  cout << " Total Cell Count : " << totalCellCount << endl;
}
