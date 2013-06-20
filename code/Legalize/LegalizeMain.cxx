# include <Legalize.h>

using namespace std;

# define WEIGHT 0.0

int rowOfInterest = 56;
int binOfInterest = 7;
int currentRow;
bool doFractReassign = true;
bool ascendingX(Cell*, Cell*);
bool debugMode = false;


/* Sorting functions */
bool
ascendingX(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetXpos()) < (cell2->CellGetXpos()));
}

bool
maxSupply(LegalizeBin *binOne, LegalizeBin *binTwo)
{
  int binOneSupp = (*binOne).BinGetSupply();
  int binTwoSupp = (*binTwo).BinGetSupply();
  return (binOneSupp < binTwoSupp);
}
bool
ascendingBins(const pair<int, unsigned int> &p1, const pair<int, unsigned int> &p2)
{
  return ((p1.first) < (p2.first));
}
bool
cmpZoneFunc(const pair<int, Zone*> &p1, const pair<int, Zone*> &p2)
{
  return ((p1.first) < (p2.first));
}

void 
LegalizeGetAllBoundingBoxes(vector<PhysRow*> &allPhysRows, 
			    vector<vector<int> > &rowBounds)
{
  PhysRow *Obj;
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    vector<int> v;
    (*Obj).PhysRowGetBoundingBox(v);
    rowBounds.push_back(v);
  } END_FOR;
}

void 
LegalizeCheckLegality(Cell* cell, vector<PhysRow*> &allPhysRows,
		      vector<vector<int> > &allRows)
{
  /* Get the values the coordinates, height and width of the cells */
  int cellX, cellY;
  int cellHeight, cellWidth;
  vector<int> Obj; 
  rowOrientation rowType;
  
  cellX = (*cell).CellGetXpos();
  cellY = (*cell).CellGetYpos();
  cellHeight = (*cell).CellGetHeight();
  cellWidth = (*cell).CellGetWidth();
  rowType = (allPhysRows[0]->PhysRowGetType());
	
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
	    (cellXend <= topX)) {
	  cell->CellSetXIsLegal(true);
	}
	
	if((cellY == botY) && (cellYend > botY) && 
	   ((cellHeight % (topY - botY)) == 0))
	  cell->CellSetYIsLegal(true);
      } else if (rowType == VERTICAL) {
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
  PhysRow *Obj;
  vector<int> RowCoordinates;    // Y coordinates of rows
  vector<int> RowOtherBounds;    // X coordinates of rows
  int nearRows[2];  
  int min, max, rowY;
  int otherCoordinateBegin;
  int otherCoordinateEnd;
  int cellX, cellXend;
  int cellHeight, cellWidth;
  cellHeight = (*cell).CellGetHeight();
  cellWidth = (*cell).CellGetWidth();
  
  min = 0;
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    rowY = (*Obj).PhysRowGetCoordinate();
    RowCoordinates.push_back(rowY);
    map<unsigned int, unsigned int> subRows = (Obj->PhysRowGetSubRows());
    unsigned int Key,Value;
    unsigned int siteSpacing = (Obj->PhysRowGetSiteSpacing());

    /* If a row had more than one sub rows */
    MAP_FOR_ALL_ELEMS(subRows, unsigned int, unsigned int, Key, Value){
      RowOtherBounds.push_back(Key);
      RowOtherBounds.push_back(Value * siteSpacing);
    }END_FOR;      
  }END_FOR;
  
  /* Get the least and the max x-values and that represent the
     X - bounds of the row */
  otherCoordinateBegin = RowOtherBounds.front();
  otherCoordinateEnd = RowOtherBounds.back();
  
  max = RowCoordinates.size();
  if(rowType == HORIZONTAL){
    int key = cell->CellGetYpos();
    cellX = cell->CellGetXpos();
    cellXend = cellX + (cell->CellGetWidth());
    findRowCoordinates(RowCoordinates, min, max, key, nearRows);
     
     /* Cell has violated X-bounds */
     if(!(cell->CellXIsLegal())){
       cout << "WARNING: Cell " << (*cell).CellGetName() << " has violated X-bounds "
	    << "X = " << cellX << endl;
       cout << "FIXING VIOLATIONS ... " << endl;
       if(cellX < otherCoordinateBegin){
	 cellX = cellX + (otherCoordinateBegin - cellX);
	 cell->CellSetXpos(cellX);
	 cout<<"LEGALIZED CELL IN X - AXIS "<<endl;
       } else if(cellXend > otherCoordinateEnd){
	 cellX = cellX - (cellXend - otherCoordinateEnd) - cellWidth;
	 cell->CellSetXpos(cellX);
	 cout<<"LEGALIZED CELL IN X - AXIS "<<endl;
       } else if((cellX < otherCoordinateBegin) &&
		 (cellXend > otherCoordinateEnd))
	 cout << "ERROR: Cell Width Exceeded Row Bounds!!"<<endl;
     }
     
     /* Cell has violated Y-bounds */
     if (!(cell->CellYIsLegal())) {
       if(fabs(key-nearRows[1])>=fabs(key-nearRows[0]))
	 (*cell).CellSetYpos(nearRows[0]);
       else
	 (*cell).CellSetYpos(nearRows[1]);
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
}


/* Function to find the y coordinates of the row that is closest to
   the GP location of the cell */
void 
findRowCoordinates(vector<int> &RowCoordinates, int min, int max,
		   int key, int nearRows[])
{
  /* DO binary search */
  int mid = (min + max) / 2;
  sort(RowCoordinates.begin(), RowCoordinates.end());
  while(max >= min){
    if (key > RowCoordinates.back()) {
      nearRows[0] = RowCoordinates.back();
      nearRows[1] = RowCoordinates.back();
      break;
    } else if (key < RowCoordinates[0]){
      nearRows[0] = RowCoordinates[0];
      nearRows[1] = RowCoordinates[0];
      break;
    } else if (RowCoordinates[mid] < key) {
      min = mid + 1;
      /* Set Lower Bound */
      nearRows[0] = RowCoordinates[mid];
      mid = (min + max) / 2;
    } else if (RowCoordinates[mid] > key) {
      max = mid - 1;
      /* Set Higher Bound */
      nearRows[1] = RowCoordinates[mid];
      mid = (min + max) / 2;
    }
  }
}


void
LegalizeFindZonesInRow(PhysRow* row, vector<Zone*> &zones, 
		       int rowBegin, int rowEnd)
{
  vector<Cell*> allCells;
  vector<Cell*> fixedCells;
  vector<Cell*> movableCells;
  Zone *thisZone, *prevZone, *zoneObj;
  Cell *Obj;
  int prevBlockedZoneEnd;
  int blockedZoneEnd;
  int thisCellEnd;
  int xPos, newXpos;
  int cellBegin, cellEnd;
  int zoneBegin, zoneEnd;
  int leftDist, rightDist;
  bool firstBlockedZone;
  unsigned int numZones;
  (*row).PhysRowGetCellsInRow(allCells);

  /* Get all the fixed cells in the row */
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj){
    if (CellIsFixed(Obj)) {
      fixedCells.push_back(Obj);
    } else {
      movableCells.push_back(Obj);
    }
  }END_FOR;
  
  firstBlockedZone = true;
  
  /* Enter this block only if fixed cells or blockages are present */
  if(fixedCells.size()){
    /* Sort fixed cells in ascending order of x-pos */
    sort(fixedCells.begin(),fixedCells.end(),ascendingX);
    /* Iterate over all fixed cells to find the zones */
    VECTOR_FOR_ALL_ELEMS(fixedCells, Cell*, Obj){
      xPos = (*Obj).CellGetXpos();
      if(firstBlockedZone){
	if (xPos == rowBegin) {
	  thisZone = new Zone(xPos, row, false);
	  zones.push_back(thisZone);
	  blockedZoneEnd = xPos + ((*Obj).CellGetWidth());
	  prevZone = thisZone;
	} else if (xPos > rowBegin) {
	  thisZone = new Zone(rowBegin, row, true);
	  zones.push_back(thisZone);
	  (*thisZone).ZoneSetEnd(xPos);
	  thisZone = new Zone(xPos, row, false);
	  zones.push_back(thisZone); 
	  prevZone = thisZone;
	  blockedZoneEnd = xPos + (*Obj).CellGetWidth();
	}
	firstBlockedZone = false;
      } else {
	/* Indicates more than one cell begins at the origin */
	if (xPos == rowBegin){
	  thisCellEnd = xPos + (*Obj).CellGetWidth();
	  if (thisCellEnd > prevBlockedZoneEnd){
	    blockedZoneEnd = thisCellEnd;
	  }
	} else if (xPos <= prevBlockedZoneEnd){
	  /* Overlapping cells */
	  thisCellEnd = xPos + (*Obj).CellGetWidth();
	  if (thisCellEnd > prevBlockedZoneEnd){
	    blockedZoneEnd = thisCellEnd;
	  }
	} else if (xPos > prevBlockedZoneEnd){
	  /* Create the empty zone before the next blocked zone */
	  prevZone->ZoneSetEnd(prevBlockedZoneEnd);
	  thisZone = new Zone(prevBlockedZoneEnd, row, true);
	  zones.push_back(thisZone);
	  thisZone->ZoneSetEnd(xPos);
	  /* Create the next blocked zone */
	  thisZone = new Zone(xPos, row, false);
	  prevZone =  thisZone;
	  zones.push_back(thisZone);
	  blockedZoneEnd = (xPos + (*Obj).CellGetWidth());
	}
      }
      prevBlockedZoneEnd = blockedZoneEnd;
    } END_FOR;
    /* Indicates last zone is an empty zone */
    if (prevBlockedZoneEnd != rowEnd){
      prevZone->ZoneSetEnd(prevBlockedZoneEnd);
      thisZone = new Zone(prevBlockedZoneEnd, row, true);
      thisZone->ZoneSetEnd(rowEnd);
      zones.push_back(thisZone);
    } else if (prevBlockedZoneEnd == rowEnd){
      prevZone->ZoneSetEnd(prevBlockedZoneEnd);
    }
  }
  /* If row does not have any cell(s) that qualfies as fixed */
  if (!(fixedCells.size())){
    thisZone = new Zone(rowBegin, row, true);
    thisZone->ZoneSetEnd(rowEnd);
    zones.push_back(thisZone);
  }
  
  numZones = zones.size();
  if (numZones > 1) {
    /* First zone in a row is always bounded on the left and
       last zone in a row is always bounded on the right */
    zones[0]->ZoneSetLRBound(0);
    zones[(numZones-1)]->ZoneSetLRBound(1);
    VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, Obj){
      cellBegin = Obj->CellGetXpos();
      cellEnd = cellBegin + (Obj->CellGetWidth());
      /* If cell is inside a blocked zone push it to an empty zone */
      VECTOR_FOR_ALL_ELEMS(zones, Zone*, zoneObj){
	if (!(zoneObj->ZoneGetEmpty())){
	  zoneBegin =  (*zoneObj).ZoneGetBegin();
	  zoneEnd =  (*zoneObj).ZoneGetEnd();      
	  newXpos = cellBegin;
	  if ((cellBegin > zoneBegin) && (cellBegin < zoneEnd)){
	    /* Indicates the zone is on the left boundary of the row */
	    if ((zoneObj->ZoneGetLRBound()) == 0){
	      newXpos = zoneEnd;
	      Obj->CellSetXpos(newXpos);
	    } else if ((zoneObj->ZoneGetLRBound()) == 1){
	      /* Indicates the zone is on the right boundary of the row */
	      newXpos = zoneBegin - (*Obj).CellGetWidth();
	      Obj->CellSetXpos(newXpos);
	    } else if ((zoneObj->ZoneGetLRBound()) == 2){
	      leftDist = cellBegin - zoneBegin;
	      rightDist = zoneEnd - (cellBegin + (*Obj).CellGetWidth());
	      if (leftDist > rightDist){
		newXpos = zoneEnd;
	      } else{
		newXpos = zoneBegin - (*Obj).CellGetWidth();
	      }
	    }
	    Obj->CellSetXpos(newXpos);
	  }
	}	    
      } END_FOR;
    } END_FOR;
  } else{
    /* Write processing logic for cells which are in a
       completely blocked row */
  }
}


void
LegalizeFindBinsInRow(vector<Zone*> &allZones, 
		      vector<LegalizeBin*> &allBins, PhysRow* &thisRow,
		      int columnWidth, int rowBegin, int rowEnd)
{
  /* Finding the column boundaries */
  vector<pair<int, unsigned int> > bins;
  for (int i = rowBegin; i < rowEnd; i += columnWidth){
    bins.push_back(make_pair(i,2));
  }
  /* Forming a vector of pairs by converting Zone objects */
  vector<pair<int, unsigned int> > zones;
  vector<pair<int, Zone*> > zoneObjMap;

  Zone* zoneObj;
  VECTOR_FOR_ALL_ELEMS(allZones, Zone*, zoneObj){
    int zBegin = zoneObj->ZoneGetBegin();
    unsigned int flag = (unsigned int)(zoneObj->ZoneGetEmpty());
    zones.push_back(make_pair(zBegin, flag));
    zoneObjMap.push_back(make_pair(zBegin, zoneObj));
  } END_FOR;
  
  /* Finding the union of zones and column boundaries
     to give bins */  
  bins.insert(bins.end(), zones.begin(), zones.end());
  sort(bins.begin(),bins.end(), ascendingBins);
  sort(zoneObjMap.begin(), zoneObjMap.end(), cmpZoneFunc);
  
  /* Creating a map to distinguish between zones and column boundaries */
  pair<int, unsigned int> Obj;
  map<int, unsigned int> zoneMap;
  VECTOR_FOR_ALL_ELEMS(zones, pair<int MCOMMA unsigned int>, Obj){
    zoneMap.insert(Obj);
  }END_FOR;
  
  pair<int, unsigned int> lastZone = *(zoneMap.begin());
  for (int idx = 0; idx < bins.size(); idx++) {
    pair<int, unsigned int> &Obj = bins[idx];
    if (Obj.second == 2){
      _KEY_EXISTS(zoneMap, Obj.first) {
	lastZone = make_pair(Obj.first, zoneMap[Obj.first]);
	bins.erase(remove(bins.begin(), bins.end(), Obj), bins.end());
	idx--;
      } else {
	Obj.second = lastZone.second;
      }
    } else {
      lastZone = Obj;
    }
    
  }

  /* Insert bin objects into a vector */
  LegalizeBin* bin;
  LegalizeBin* prevBin; 
  bool firstBin = true;
  uint numZones = zoneObjMap.size();
  prevBin = NIL(LegalizeBin*);
  VECTOR_FOR_ALL_ELEMS(bins, pair<int MCOMMA unsigned int>, Obj){
    if (firstBin) {
      bin = new LegalizeBin(Obj.first, thisRow, Obj.second);
      /* The first bin is always bounded on the left */
      bin->BinSetLBound(true);      
      int height = (int)thisRow->PhysRowGetHeight();
      int bot = (int)thisRow->PhysRowGetCoordinate();
      bin->BinSetHeight(height);
      bin->BinSetBot(bot);
      allBins.push_back(bin);
      firstBin = false;
    } else {
      prevBin->BinSetEnd(Obj.first);
      prevBin->BinCalculateWidth();
      bin = new LegalizeBin(Obj.first, thisRow, Obj.second);
      int height = (int)thisRow->PhysRowGetHeight();
      int bot = (int)thisRow->PhysRowGetCoordinate();
      bin->BinSetHeight(height);
      bin->BinSetBot(bot);
      /* If previous bin was blocked, set LBound on the present bin */
      if (!(prevBin->BinGetEmpty())){
	if (bin->BinGetEmpty()){
	  bin->BinSetLBound(true);
	}
      } else if (prevBin->BinGetEmpty()){
      /* If the previous bin was empty and present bin is blocked,
	 Set RBound on the previous bin */
	if (!(bin->BinGetEmpty())){
	  prevBin->BinSetRBound(true);
	}
      }	
      allBins.push_back(bin);
    }
    /* Set the zone each bin belongs to */
    {
      int xl, xg;
      uint idxl, idxg;
      uint xz;
      uint x = Obj.first;
      xl = (zoneObjMap[0]).first;
      xg = 0; idxl = 0; idxg = 0;
      Zone *binZone = NIL(Zone *);
      for (int i = 0; i < numZones; i++) {
	xz = (zoneObjMap[i]).first;
	if (xz < x) {
	  idxl = i;
	} else if (xz > x) {
	  break;
	} else if (xz == x) {
	  binZone = (zoneObjMap[i]).second;
	}
      }
      if (binZone == NIL(Zone *)) {
	binZone = (zoneObjMap[idxl]).second;
      }
      (*bin).BinSetZone(binZone);
    }
    prevBin = bin;
  }END_FOR;
  prevBin->BinSetEnd(rowEnd);
  prevBin->BinCalculateWidth();
  /* The last bin is always bounded on the right */
  prevBin->BinSetRBound(true);
  
}


void
LegalizeMergeBins(vector<LegalizeBin*> &allBins, int columnWidth)
{
  LegalizeBin* lastBin;
  vector<LegalizeBin*> emptyBins;
  bool firstBin = true;
  for (int idx = 0; idx < allBins.size(); idx++){
    if (firstBin){
      /* If the first bin in the row is an empty bin */
      if (allBins[idx]->BinGetEmpty()){
	emptyBins.push_back(allBins[idx]);
      } 
      /* If the first bin is blocked, nothing is done */
      firstBin = false;
    } else{
      if (!(allBins[idx]->BinGetEmpty()) && (emptyBins.size() > 1)){
	/* Found a blocked bin and a set of consecutive unblocked bins found to check for possible merges */
	LegalizeBin* first = emptyBins[0];
	LegalizeBin* second = emptyBins[1];
	int firstWidth = first->BinGetWidth();
	int secondWidth = second->BinGetWidth();
	
	/* Condition to merge two bins */
	if ((firstWidth + secondWidth) < (2 * columnWidth)){
	  int firstBegin = first->BinGetBegin();
	  second->BinSetBegin(firstBegin);
	  second->BinCalculateWidth();
	  emptyBins.erase(emptyBins.begin());
	  allBins.erase(remove(allBins.begin(), allBins.end(), first), allBins.end());
	  idx--;
	  if (0) {
	    cout << "Bin merged " << endl;
	  }
	}
	/* Keeps checking for merges if the number of unblocked bins is more than two */
	if (emptyBins.size() >= 2){
	  int size = emptyBins.size();
	  LegalizeBin* last = emptyBins[(size-1)];
	  LegalizeBin* secondLast = emptyBins[(size-2)];
	  int lastWidth = last->BinGetWidth();
	  int secondLastWidth = secondLast->BinGetWidth();
	  if ((lastWidth + secondLastWidth) < (2 * columnWidth)){
	    int lastEnd = last->BinGetEnd();
	    secondLast->BinSetEnd(lastEnd);
	    secondLast->BinCalculateWidth();
	    emptyBins.erase((emptyBins.begin() + (size - 1)));
	    allBins.erase(remove(allBins.begin(), allBins.end(), last), allBins.end());
	    idx--;
	    if (0) {
	      cout << "Bin merged " << endl;
	    }
	  }
	}
	emptyBins.clear();
      } else if (!(allBins[idx]->BinGetEmpty()) && (emptyBins.size() == 1)){
	/* Found a block bin but the set of unblocked bins contain only one bin
	   So empty the vector */
	emptyBins.clear();
      } else if (allBins[idx]->BinGetEmpty()){
	/* Found an empty bin, store it for a possible interval formation */
	emptyBins.push_back(allBins[idx]);
      }
      lastBin = allBins[idx];
    }
  }
  /* If atleast the last two bins were empty form an interval */
  if ((lastBin->BinGetEmpty()) && (emptyBins.size() > 1)){
    /* Found a blocked bin and a set of consecutive unblocked bins found to check for possible merges */
    LegalizeBin* first = emptyBins[0];
    LegalizeBin* second = emptyBins[1];
    int firstWidth = first->BinGetWidth();
    int secondWidth = second->BinGetWidth();
    
    /* Condition to merge two bins */
    if ((firstWidth + secondWidth) < (2 * columnWidth)){
      int firstBegin = first->BinGetBegin();
      second->BinSetBegin(firstBegin);
      second->BinCalculateWidth();
      emptyBins.erase(emptyBins.begin());
      allBins.erase(remove(allBins.begin(), allBins.end(), first), allBins.end());
      if (0) {
	cout << "Bin merged " <<endl;
      }
    }
    if (emptyBins.size() >= 2){
      int size = emptyBins.size();
      LegalizeBin* last = emptyBins[(size-1)];
      LegalizeBin* secondLast = emptyBins[(size-2)];
      int lastWidth = last->BinGetWidth();
      int secondLastWidth = secondLast->BinGetWidth();
      if ((lastWidth + secondLastWidth) < (2 * columnWidth)){
	int lastEnd = last->BinGetEnd();
	secondLast->BinSetEnd(lastEnd);
	secondLast->BinCalculateWidth();
	emptyBins.erase((emptyBins.begin() + (size - 1)));
	allBins.erase(remove(allBins.begin(), allBins.end(), last), allBins.end());
	if (0) {
	  cout << "Bin merged " << endl;
	}
      }
    }
    emptyBins.clear();
  }
  LegalizeBin* binObj;
  VECTOR_FOR_ALL_ELEMS (allBins, LegalizeBin*, binObj){
    int binIndex = i;
    binObj->BinSetIndex(i);
  } END_FOR;

}

void
LegalizeAddCellsToBins(vector<Cell*> &cellsInRow, vector<LegalizeBin*> &allBins)
{
  int cellBegin, cellEnd;
  Cell* cellObj;
  LegalizeBin* binObj;
  Zone *thisZone;
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
    cellBegin = cellObj->CellGetXpos();
    VECTOR_FOR_ALL_ELEMS(allBins, LegalizeBin*, binObj){
      if (binObj->BinGetEmpty()){
	thisZone = (*binObj).BinGetZone();
	int begin = binObj->BinGetBegin();
	int end = binObj->BinGetEnd();
	if ((cellBegin >= begin) && (cellBegin < end)){
	    binObj->BinAddCellToBin(cellObj);
	    (*thisZone).ZoneAddCellToZone(cellObj);
	}
      }
    } END_FOR;
  } END_FOR;
}


void
LegalizeCalcSuppForAllBins(vector<vector<LegalizeBin*> > &designBins, 
			  vector<LegalizeBin*> &supplyBins)
{
  supplyBins.clear();
  vector<LegalizeBin*> binsInRow;
  LegalizeBin *thisBin;
  int binsInRowSize;
  double wCells, supply;
  int wBin;
  vector<Cell*> cellsInBin;
  VECTOR_FOR_ALL_ELEMS(designBins, vector<LegalizeBin*>, binsInRow){
    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, thisBin) {
      if ((*thisBin).BinGetEmpty()) {
	cellsInBin = (*thisBin).BinGetCellsInBin();
	Cell *thisCell;
	int cellWidth;
	wCells = 0.0;
	VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell) {
	  cellWidth = (*thisCell).CellGetWidth();
	  wCells += cellWidth;
	} END_FOR;
	wBin = (*thisBin).BinGetWidth();
	supply = wCells - (double)wBin;
	if (supply > 0.0) {
	  supplyBins.push_back(thisBin);
	} 
	(*thisBin).BinSetSupply(supply);
      }
    } END_FOR;
  } END_FOR;
}


void
LegalizeReCalcSuppForAllBins(vector<vector<LegalizeBin*> > &designBins, 
			  vector<LegalizeBin*> &supplyBins)
{
  map<Cell*, double> fractCells;
  vector<LegalizeBin*> binsInRow;
  LegalizeBin *thisBin;
  Cell *key;
  double wCells, supply, value;
  double wBin;
  double cellFractWidth;
  int binsInRowSize, wbin;
  int cellWidth;

  supplyBins.clear();
  VECTOR_FOR_ALL_ELEMS(designBins, vector<LegalizeBin*>, binsInRow){
    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, thisBin) {
      if ((*thisBin).BinGetEmpty()) {
	fractCells = (*thisBin).BinGetAllFractCells();
	wCells = 0.0;
	MAP_FOR_ALL_ELEMS(fractCells, Cell*, double, key, value){
	  cellWidth = (*key).CellGetWidth();
	  cellFractWidth = value * (double)cellWidth;
	  wCells += cellFractWidth;
	} END_FOR;
	wBin = (*thisBin).BinGetWidth();
	supply = wCells - (double)wBin;
	if (supply > 0.0) {
	  supplyBins.push_back(thisBin);
	} 
	(*thisBin).BinSetSupply(supply);
      }
    } END_FOR;
  } END_FOR;
}

double
LegalizeThetaFunc(uint L1, uint R1, uint L2, uint R2)
{
  uint left, right;
  double overlap;
  right = R2;

  if (R1 < R2) right = R1;
  left = L2;
  if (L1 > L2) left = L1;

  overlap = (int)(right - left);
  if (overlap < 0) overlap = 0.0;
  //  int cellWidth = R2 - L2;
  //  double fraction = (double)overlap / cellWidth;
  return (overlap);
}

void
LegalizeGetNeighbours(LegalizeBin* &binOne, LegalizeBin* &binTwo,
		      LegalizeBin* &binThree, vector<LegalizeBin*> &allBins)
{
  int idxOne = (*binOne).BinGetIndex();
  int idxTwo = idxOne + 1;
  int idxThree = idxOne + 2;
  if (idxTwo < allBins.size()) {
    binTwo = allBins[idxTwo];
    if (idxThree < allBins.size()) {
      binThree = allBins[idxThree];
    }
  }
}

void 
LegalizeDoFractReassign(vector<vector<LegalizeBin*> > &designBins)
{
  vector<LegalizeBin*> binsInRow;
  vector<Cell*> cellsInBin;
  LegalizeBin *tempBin;
  LegalizeBin* binOne;
  LegalizeBin* binTwo = NIL(LegalizeBin*);
  LegalizeBin* binThree = NIL(LegalizeBin*);
  Cell* cellObj, *thisCell;
  PhysRow *currRow;
  double gamma1, gamma2, gamma3;
  double binSupply;
  double fract;
  int binOneBegin, binTwoBegin, binThreeBegin;
  int binOneEnd, binTwoEnd, binThreeEnd;
  int cellBegin, cellEnd, cellWidth;
  int binOneSupply, binTwoSupply;
  int newCellWidth;
  int binWidth;
  int bIndex, bOneTotalCellWidth;
  int currentRow;
  bool fractAssign;

  VECTOR_FOR_ALL_ELEMS(designBins, vector<LegalizeBin*>, binsInRow){
    
    /* Clear the map containing fractional cells */
    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, tempBin) {
      (*tempBin).BinRemoveAllFractCells();
    } END_FOR;

    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, binOne){
      binSupply = (*binOne).BinGetSupply();
      
      /* If demand bin, assign all the cell(s) if any a fraction of 1.0*/
      /* This is done to accurate calculate supplies after fract reassignment*/
      if (binSupply <= 0) {
	cellsInBin = (*binOne).BinGetCellsInBin();
	VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
	  (*binOne).BinSetCellFract(thisCell, 1.0);
	} END_FOR;
      } else {
	/* If supply bin, enter this block*/
	currRow = (*binOne).BinGetRow();
	bIndex = (*binOne).BinGetIndex();
	bOneTotalCellWidth = (*binOne).BinGetTotalCellWidth();
	currentRow = (*currRow).PhysRowGetIndex();
	cellsInBin = (*binOne).BinGetCellsInBin();
	binOneBegin = (*binOne).BinGetBegin();
	binOneEnd = (*binOne).BinGetEnd();
	/* Get the two bins immediately to the right of the 
	   current bin if they exist */
	LegalizeGetNeighbours(binOne, binTwo, binThree, binsInRow);
	VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, cellObj){
	  /* Assigning default values to fraction in bin two and three */
	  gamma2 = 0; gamma3 = 0; binTwoSupply = 0;
	  cellBegin = (*cellObj).CellGetXpos();
	  cellWidth = (*cellObj).CellGetWidth();
	  cellEnd = cellBegin + cellWidth;
	  gamma1 = LegalizeThetaFunc(binOneBegin, binOneEnd, cellBegin, cellEnd);
	  
	  /* If bin two exists, get the fraction of the cell in bin two */
	  if (binTwo) {
	    binTwoSupply = (*binTwo).BinGetSupply();
	    binTwoBegin = (*binTwo).BinGetBegin();
	    binTwoEnd = (*binTwo).BinGetEnd();
	    gamma2 = LegalizeThetaFunc(binTwoBegin, binTwoEnd, cellBegin, cellEnd);
	  }
	  
	  /* If bin three exists, get the fraction of the cell in bin three */
	  if (binThree) {
	    binThreeBegin = (*binThree).BinGetBegin();
	    binThreeEnd = (*binThree).BinGetEnd();
	    gamma3 = LegalizeThetaFunc(binThreeBegin, binThreeEnd, cellBegin, cellEnd);
	  }

	  /* DO NOT do any fractional reassignment even to bin two 
	     if a cell overflows into bin three irrespective of bin two
	     being a supply or a demand bin */
	  if (gamma3 > 0) {
	    fractAssign = false;
	  }

	  /* DO NOT do any fractional reassignment to bin two if it is a 
	     supply bin. Only a demand bin present to the right of a supply 
	     bin calls for fractional reassignment */
	  if (gamma2 == 0 || binTwoSupply > 0) {
	    fractAssign = false;
	  }

	  /* Assign cell fractionally to bin 1(the bin it belongs to
	   originally based on its global placement location */
	  fract = gamma1 / cellWidth;
	  
	  /* If bin two is a supply bin, then even if a cell in bin one
	     overflows into bin two, the cell is to assigned a fraction 
	     of 1.0 to the bin it originally belonged to */
	  if (!fractAssign) {
	    fract = 1.0;
	  }
	  newCellWidth = (*binOne).BinGetTotalCellWidth() - cellWidth + gamma1;
	  (*binOne).BinSetTotalCellWidth(newCellWidth);
	  (*binOne).BinSetCellFract(cellObj, fract);

	  if (fractAssign) {
	    /* Bin 1 is supply and Bin 2 is demand */
	    fract = gamma2 / cellWidth;
	    newCellWidth = (*binTwo).BinGetTotalCellWidth() + gamma2;
	    (*binTwo).BinSetTotalCellWidth(newCellWidth);
	    (*binTwo).BinSetCellFract(cellObj, fract);
	  }
	} END_FOR;
      }
    } END_FOR;
  } END_FOR;
}

void
LegalizeGetAdjacentBins(LegalizeBin* &thisBin, LegalizeBin* &leftBin, 
			LegalizeBin* &rightBin, LegalizeBin* &topBin, 
			LegalizeBin* &botBin, vector<vector<LegalizeBin*> > &designBins)
{
  int endRowIdx = designBins.size() - 1;
  int thisBinIdx = (*thisBin).BinGetIndex();
  int thisBinBegin = (uint)(*thisBin).BinGetBegin();
  int thisBinEnd = (uint)(*thisBin).BinGetEnd();
  int leftBinIdx = thisBinIdx - 1;
  int rightBinIdx = thisBinIdx + 1;
  PhysRow *thisRow = (*thisBin).BinGetRow();
  int rowIdx = (*thisRow).PhysRowGetIndex();
  int nextRowIdx = rowIdx + 1;
  int prevRowIdx = rowIdx - 1;
  vector<LegalizeBin*> thisRowBins = designBins[rowIdx];
  int endIdx = thisRowBins.size() - 1;  
  vector<LegalizeBin*> nextRowBins;
  vector<LegalizeBin*> prevRowBins;
  
  if (leftBinIdx < 0) {
    leftBin = NIL(LegalizeBin*);
  }
  if (leftBinIdx >= 0) {
    bool isEmpty = (*thisRowBins[leftBinIdx]).BinGetEmpty();
    if (isEmpty) {
      leftBin = thisRowBins[leftBinIdx];
    } else {
      leftBin = NIL(LegalizeBin*);
    }
  }
  
  if (rightBinIdx > endIdx) {
    rightBin = NIL(LegalizeBin*);
  }
  if (rightBinIdx <= endIdx) {
    bool isEmpty = (*thisRowBins[rightBinIdx]).BinGetEmpty();
    if (isEmpty) {
      rightBin = thisRowBins[rightBinIdx];
    } else {
      rightBin = NIL(LegalizeBin*);
    }    
  }
  
  if (nextRowIdx <= endRowIdx) {
    nextRowBins = designBins[nextRowIdx];
  }
  if (prevRowIdx >= 0) {
    prevRowBins = designBins[prevRowIdx];
  }
  
  topBin = NIL(LegalizeBin*);
  botBin = NIL(LegalizeBin*);
  double maxOverlap = 0.0;
  if (nextRowBins.size()) {
    LegalizeBin *binObj;
    VECTOR_FOR_ALL_ELEMS(nextRowBins, LegalizeBin*, binObj){
      int binBegin = (uint)(*binObj).BinGetBegin();
      int binEnd = (uint)(*binObj).BinGetEnd();
      bool isEmpty = (*binObj).BinGetEmpty();
      double overlap = LegalizeThetaFunc(thisBinBegin, thisBinEnd, binBegin, binEnd);
      if ((overlap > maxOverlap) && isEmpty) {
	maxOverlap = overlap;
	topBin = binObj;
      }
    } END_FOR;
  } 
  maxOverlap = 0.0;
  if (prevRowBins.size()) {
    LegalizeBin *binObj;
    VECTOR_FOR_ALL_ELEMS(prevRowBins, LegalizeBin*, binObj){
      int binBegin = (uint)(*binObj).BinGetBegin();
      int binEnd = (uint)(*binObj).BinGetEnd();
      bool isEmpty = (*binObj).BinGetEmpty();
      double overlap = LegalizeThetaFunc(thisBinBegin, thisBinEnd, binBegin, binEnd);
      if ((overlap > maxOverlap) && isEmpty) {
	maxOverlap = overlap;
	botBin = binObj;
      }
    } END_FOR;
  }
}

bool
sortLValues(const pair<LegalizeBin*, double> &i, const pair<LegalizeBin*, double> &j)
{
  return (i.second < j.second);
}

void
LegalizePrintBinData(LegalizeBin *thisBin)
{
  PhysRow *thisRow;
  int binIdx;
  int rowIdx;
  thisRow = (*thisBin).BinGetRow();
  binIdx = (*thisBin).BinGetIndex();
  rowIdx = (*thisRow).PhysRowGetIndex();
  cout << rowIdx << "[" << binIdx << "]" << "\t";
}

void
printBin(LegalizeBin *thisBin)
{
  PhysRow *thisRow;
  int binIdx;
  int rowIdx;
  thisRow = (*thisBin).BinGetRow();
  binIdx = (*thisBin).BinGetIndex();
  rowIdx = (*thisRow).PhysRowGetIndex();
  cout << rowIdx << "[" << binIdx << "]" << "\t" << endl;
}

void
printZone(Zone *thisZone)
{
  PhysRow *thisRow;
  int zoneBegin;
  int rowIdx;
  thisRow = (*thisZone).ZoneGetRow();
  zoneBegin = (*thisZone).ZoneGetBegin();
  rowIdx = (*thisRow).PhysRowGetIndex();
  cout << rowIdx << "(" << zoneBegin << ")" << "\t" << endl;
}

void 
printCellsInBin(LegalizeBin *thisBin)
{
  vector<Cell*> cellsInBin;
  Cell *thisCell;
  cellsInBin = (*thisBin).BinGetCellsInBin();
  
  VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
    cout << "CELL: " << (*thisCell).CellGetName() << "\tWIDTH: " << (*thisCell).CellGetWidth() << endl;
  } END_FOR;
}

void
LegalizeAugPathAlgoNew(HyperGraph &myGraph, LegalizeBin *suppNode, vector<LegalizeBin *> &PathBins)
{
  map<Cell*, double> moveCells;
  map<Cell*, double> AofV, binCells;
  map<LegalizeBin*, bool> usedNodes;
  map<LegalizeBin*,LegalizeBin*> pathMap;
  map<Cell*, double> cellsToMove;
  vector<pair<LegalizeBin* , double> > binsLValues;
  vector<LegalizeBin *> adjacentBins, reversePath;
  pair<LegalizeBin*, double> binLValObj;
  LegalizeBin *destBin, *currentNode, *prevNode;
  LegalizeBin *adjacentNode;
  LegalizeBin *currentBin, *prevBin, *nextBin;
  PhysRow *currentRow, *nextRow;
  Zone *currBinZone, *nextBinZone;
  Cell *Key;
  void *NodeObj;
  double bValueCurrent, Value;
  double currentNodeLValue;
  double fractWidthSelected, weight;
  double supply;
  double currentLVal, adjLVal, modAdjLVal, moveCost;
  double totalFractWidth;
  uint numEdges, numNodes;
  uint NodeIdx;
  uint iteration;
  int edgeIdx, binX, binY;
  bool sameZone;

  numEdges = myGraph.GetNumEdges();
  numNodes = myGraph.GetNumNodes();
  iteration = 0;

  vector<map<Cell*, double> > chosenCells(numEdges);
  vector<double> totalFractWidths(numEdges);

  /* Clear all bins in PathBins */
  PathBins.clear();

  /* Assign all fractional cells of supply node to AofS */
  AofV = (*suppNode).BinGetAllFractCells();
  
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
      edgeIdx = (int)myGraph.NodesAreAdjacent((void*)prevNode, (void*)currentNode);
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
    NodeObj = (void *)currentNode;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(myGraph, NodeObj, edgeIdx, weight){
      adjacentNode = (LegalizeBin*)myGraph.GetOtherNodeOfEdge(edgeIdx, NodeObj);
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
      binCells = AofV;
      moveCost = LegalizeGetCellsToMove((*currentNode), (*adjacentNode), binCells, 
					sameZone, cellsToMove, totalFractWidth);
      chosenCells[edgeIdx] = cellsToMove;
      totalFractWidths[edgeIdx] = totalFractWidth;
      
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
    iteration++;
  }
  
  if (destBin == NIL(LegalizeBin *)) {
    cout << "ERROR: Demand bin not found!!" << endl;
    return;
  } else {
    //cout << "Destination bin found: ";
    //LegalizePrintBinData(destBin);
    //cout << endl;
  }

  currentBin = destBin;
  reversePath.push_back(currentBin);
  PathBins.push_back(currentBin);

  /* Tracing the path from the destination to the source */
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
    
    currBinZone = (*currentBin).BinGetZone();
    nextBinZone = (*nextBin).BinGetZone();
    
    sameZone = true;
    if (currBinZone != nextBinZone) {
      sameZone = false;
    }

    binX = (*nextBin).BinGetBegin();
    binY = (*nextBin).BinGetBot();

    edgeIdx = myGraph.NodesAreAdjacent((void*)currentBin, (void*)nextBin);
    cellsToMove = chosenCells[edgeIdx];
    vector<Cell*> cellsCurrBin = (*currentBin).BinGetCellsInBin();
    Cell *thisCell;
    double cellFract;
    int thisCellY;
    int currBinSize = cellsCurrBin.size();

    if (currBinSize) {
      for(int cellCount = 0; cellCount < currBinSize; cellCount++) {
	thisCell = cellsCurrBin[cellCount];
	thisCellY = (*thisCell).CellGetYpos();
	_KEY_EXISTS(cellsToMove, thisCell) {
	  (*currentBin).BinRemoveCellFromBin(thisCell);
	  (*nextBin).BinAddCellToBin(thisCell);
	  
	  if (!sameZone) {
	    (*currBinZone).ZoneRemoveCellFromZone(thisCell);
	    (*nextBinZone).ZoneAddCellToZone(thisCell);
	  }

	  if (thisCellY != binY) {
	    (*currentRow).PhysRowRemoveCellFromRow(thisCell);
	    (*nextRow).PhysRowAddCellToRow(thisCell);
	  }
	}
      }
    } else {
      cout << "SEVERE ERROR: No cells were moved to current bin " << endl;
    }

    /* Moving cells to the left corner of each dest bin*/
    MAP_FOR_ALL_ELEMS(cellsToMove, Cell*, double, Key, Value){
      (*Key).CellSetXpos(binX);
      (*Key).CellSetYpos(binY);
    } END_FOR;
    currentBin = nextBin;

  } while(currentBin != destBin);

  LegalizeBinClearAllLValue();
  LegalizeBinClearAllBValue();
}

void
LegalizeAssignLValues(vector<vector<LegalizeBin*> > &designBins)
{
  vector<LegalizeBin*> binsObj;
  LegalizeBin *thisBin;
  bool isUnblocked;
  VECTOR_FOR_ALL_ELEMS(designBins, vector<LegalizeBin*>, binsObj){
    VECTOR_FOR_ALL_ELEMS(binsObj, LegalizeBin*, thisBin) {
      isUnblocked = (*thisBin).BinGetEmpty();
      if (isUnblocked) {
	LegalizeBinSetLValue(thisBin, DBL_MAX);
      }
    } END_FOR;
  } END_FOR;
}


void
LegalizePlotData(string plotTitle, string plotFileName, Design &thisDesign, 
		 vector<vector<LegalizeBin*> > &DesignBins, vector<LegalizeBin *> &PathBins)
{
  Plot newPlot(plotTitle, plotFileName);
  newPlot.PlotSetBoundary(thisDesign);
  vector<LegalizeBin*> bins;
  LegalizeBin *binObj;
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, bins){
    VECTOR_FOR_ALL_ELEMS (bins, LegalizeBin*, binObj){
      int supply = (*binObj).BinGetSupply();
      if (supply > 0) {
	newPlot.PlotAddSupplyBin(*binObj, supply);
      } else {
	newPlot.PlotAddDemandBin(*binObj, supply);
      }
    } END_FOR;
  } END_FOR;

  /* DISABLE PATH HERE */
# if 0
  VECTOR_FOR_ALL_ELEMS (PathBins, LegalizeBin*, binObj){
    int supply = (*binObj).BinGetSupply();
    newPlot.PlotAddBinPathRect((*binObj), supply);
  } END_FOR;
# endif

  newPlot.PlotWriteOutput();
}

void
LegalizeCreateBins(Design &myDesign, int columnWidth,
		   vector<vector<LegalizeBin *> > &DesignBins,
		   vector<vector<Zone *> > &DesignZones) 
{
  PhysRow *Obj;
  vector<PhysRow*> allPhysRows;
  vector<Cell *> cellsInRow;
  int rowBegin, rowEnd;

  allPhysRows  = myDesign.DesignGetRows();
  rowBegin = ((allPhysRows[0])->PhysRowGetRowBegin());
  rowEnd = ((allPhysRows[0])->PhysRowCalculateRowEnd());

  DesignBins.clear();
  DesignBins.clear();
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj){
    (*Obj).PhysRowGetCellsInRow(cellsInRow);
    /* Find Zones in the row and push all movable cells into free zone */
    vector<Zone*> zones;
    LegalizeFindZonesInRow(Obj, zones, rowBegin, rowEnd);
    DesignZones.push_back(zones);
    
    /* Find all the regions in the row */
    vector<LegalizeBin*> allBins; // denotes all bins in a specific row 
    LegalizeFindBinsInRow(zones, allBins, Obj, columnWidth, rowBegin, rowEnd);
    /* Merge bins if they satisfy the condition for merging */
    LegalizeMergeBins(allBins, columnWidth);
    /* Add Cell To respective regions */
    LegalizeAddCellsToBins(cellsInRow, allBins);
    /* Adding all the bins to the vector<vector> of DesignBins */
    DesignBins.push_back(allBins); 
  } END_FOR;
}

void
LegalizeDesign(Design &myDesign)
{
  map<LegalizeBin*, uint> binsInGraph;
  map<Cell*, double> cellFracts;
  vector<vector<LegalizeBin*> > DesignBins;
  vector<vector<Zone*> > DesignZones;
  vector<PhysRow*> allPhysRows;
  vector<vector<int> > allRowBounds;
  vector<LegalizeBin*> binsInRow;
  vector<Cell*> cellsInBin;
  vector<Cell*> cellsInRow;
  vector<Cell*> cellsInZone;
  vector<LegalizeBin*> supplyBins;
  vector<LegalizeBin *> PathBins;
  vector<Zone*> zonesInRow;
  vector<void*> oneEdge;
  rowOrientation rowType;
  HyperGraph myGraph;
  LegalizeBin *thisBin;
  LegalizeBin *topBin, *botBin, *leftBin, *rightBin;
  LegalizeBin *maxSuppBin, *tempBin;
  PhysRow* Obj;				  
  Cell *thisCell, *CellPtr, *cellObj;
  Zone *thisZone;
  double value;
  int rowBegin, rowEnd, columnWidth;
  int zoneBegin, numCellsZone, zoneWidth;
  int totCellWidthZone;
  int maxCellWidth, cellWidth;
  int totalCellCount, rowCount;
  int nextCellBegin;
  int cellX, cellY;
  string CellName;
  uint iteration;
  bool isUnblocked;
  bool legal;

/* Get all the physical rows in Design */
  allPhysRows  = myDesign.DesignGetRows();
  
  /* Get bounding boxes for all subrows in all rows in Design */
  /* Will be modified later if no design has subrows */
  LegalizeGetAllBoundingBoxes(allPhysRows, allRowBounds);
  rowType=((allPhysRows[0])->PhysRowGetType());

  /* Checking the Legality of all Cells and setting the variable isLegal
     for all cells accordingly */ 
  maxCellWidth = 0;
  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal())) {
      cellWidth = (*CellPtr).CellGetWidth();
      if (cellWidth > maxCellWidth) {
	maxCellWidth = cellWidth;
      }
      LegalizeCheckLegality(CellPtr, allPhysRows, allRowBounds);
    }
  } DESIGN_END_FOR;
  
  /* The column width of each bin is initially double of the 
     maximum cell width */
  columnWidth = 2 * maxCellWidth;
  if (debugMode) {
    cout << "COLUMN WIDTH CHOSEN = " << columnWidth << endl;
  }
  
  /* Legalizing design so that every cell is contained completely in some row */
  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal())){
      if((CellPtr->CellXIsLegal()) && (CellPtr->CellYIsLegal())){
      } else{
	LegalizeSnapToNearestRows(CellPtr, allPhysRows, rowType);
      }
    }
  } DESIGN_END_FOR;
  
  /*  Adding all cells to their respective rows */
  myDesign.DesignAddAllCellsToPhysRows();
  
  cout << "INITIAL LEGALIZATION DONE" << endl;

  /* Get the row begin and row end values for each row */
  /* It is assumed that all rows begin and end at the same x-coord */
  rowBegin = ((allPhysRows[0])->PhysRowGetRowBegin());
  rowEnd = ((allPhysRows[0])->PhysRowCalculateRowEnd());

  /* Create the zones for each row and hence the bins */
  LegalizeCreateBins(myDesign, columnWidth, DesignBins, DesignZones);
  
  /* Calculate supply for all bins in design */
  /* Calculates the supply of bins taking the global
     placement locations of the cells into account */
  LegalizeCalcSuppForAllBins(DesignBins, supplyBins);
  
  if (debugMode) {
    cout << "NUMBER OF SUPPLY BINS BASED ON GP ALLOCATION " 
	 << supplyBins.size() << endl;
  }

  /* Assign cells fractionally to reduce the supply of supply nodes */
  if (doFractReassign) {
    LegalizeDoFractReassign(DesignBins);
  }
  
  /* RECALCULATING supply after fractional reassignment */
  LegalizeReCalcSuppForAllBins(DesignBins, supplyBins);
  
  /* CREATE GRAPH BY ADDING NODES AND EDGES */
  /* Add nodes to the graph */
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsInRow){
    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, thisBin) {
      /* If bin is unblocked add it to the graph */
      isUnblocked = (*thisBin).BinGetEmpty();
      if (isUnblocked) {
	myGraph.AddNode((void*)thisBin);
      }
    } END_FOR;
  } END_FOR;
  
  /* Assign default L(v) = DBL_MAX to all nodes in the graph */
  LegalizeAssignLValues(DesignBins);
  
  /* Add edges to the graph */
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsInRow){
    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, thisBin) {
      isUnblocked = (*thisBin).BinGetEmpty();
      if (isUnblocked) {
	/* Add the bin to map to indicate all edges to the 
	   bin has been formed */
	binsInGraph[thisBin] = 1; 
	
	/* Get the bins adjacent to this bin in all the four directions
	   Return a null pointer if no eligible adjacent bin is found in a 
	   specific direction */
	LegalizeGetAdjacentBins(thisBin, leftBin, rightBin, 
				topBin, botBin, DesignBins);
	if (topBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, topBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)topBin);
	    myGraph.AddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
	if (botBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, botBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)botBin);
	    myGraph.AddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
	if (leftBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, leftBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)leftBin);
	    myGraph.AddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
	if (rightBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, rightBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)rightBin);
	    myGraph.AddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
      }
    } END_FOR;
  } END_FOR;
  
  if (debugMode) {
    cout << " INITIAL NUMBER OF SUPPLY BINS  " << supplyBins.size() << endl;
  }

  cout << " INITIAL NUMBER OF SUPPLY BINS  " << supplyBins.size() << endl;
  /* Creating the max heap */
  make_heap(supplyBins.begin(), supplyBins.end(), maxSupply);

  /* Plot the supply and demand bins initially before 
     any legalization is done */
  LegalizePlotData("supply and demand bins", "pre.plt", myDesign, DesignBins, PathBins);
  
  iteration = 0;
  
  cout << "LEGALIZING DESIGN =>" << endl;
  /* BEGIN: Top Level while loop */
  
  
  while (supplyBins.size()) {
    
    /* Get the first element = max element from the heap */
    if (debugMode) {
      cout << "<============== ";
      cout << "TOP LEVEL ITERATION " << iteration;
      cout << " ==============> " << endl;
    }
    
    maxSuppBin = supplyBins.front();
    if (debugMode) {
      cout << "Supply bin: ";
      LegalizePrintBinData(maxSuppBin);
      cout << endl;
    }

    if (debugMode) {
      cout << "BEFORE AUG-PATH ALGO IS INVOKED, NUMBER OF SUPPLY BINS: " 
	   << supplyBins.size() << endl;
    }
    
    /* ################################################## */
    /* ######### RUN THE MAIN AUG-PATH ALGO ############# */
    /* ################################################## */

    LegalizeAugPathAlgoNew(myGraph, maxSuppBin, PathBins);
        
    /* Supply of all bins are calculated based on the cells' GP 
       allocation. This is required to identify supply and demand 
       bins every time before fractional reassignment is done */
    
    LegalizeCalcSuppForAllBins(DesignBins, supplyBins);
    if (debugMode) {
      cout << "AFTER AUG-PATH ALGO IS INVOKED, NUMBER OF SUPPLY BINS: " 
	   << supplyBins.size() << endl;
    }

    cout << "\rNUMBER OF BINS WITH EXCESS CELLS = " << supplyBins.size() << flush;
    
    /* Do fract re-assign in this step before the next iteration */
    if (doFractReassign) {
      LegalizeDoFractReassign(DesignBins);
    }
    
    
    /* ReCalculate Supplies after doing fractional reassignment of cells */
    /* This step is executed to get modified supplies based on fractional
       reassignment from fract maps */
    LegalizeReCalcSuppForAllBins(DesignBins, supplyBins);
    if (debugMode) {
      cout << "NUMBER OF SUPPLY BINS AFTER RECALCULATING SUPPLIES: " 
	   << supplyBins.size() << endl;
    }

    /* Reassign the default L-Values to all the bins before executing
       the next iteration of AUG-PATH ALGO */
    LegalizeAssignLValues(DesignBins);

    /* Recreate the heap as the supply values have been modified */
    make_heap(supplyBins.begin(), supplyBins.end(), maxSupply);
    iteration++;
    /*
    if ((iteration % 20) == 0) {
      cout << " => ";
    }
    */
  }
  
  // cout << "**" << endl;
  /* END : Top Level while loop  */
  
  /* LEGALIZE SELF CHECK: BEGIN */
  totalCellCount = 0;
  totCellWidthZone = 0;
  rowCount = 0;
  legal = true;
  VECTOR_FOR_ALL_ELEMS(DesignZones, vector<Zone*>, zonesInRow){
    if (debugMode) {
      cout << "ROW NUM: " << rowCount << endl;
    }
    VECTOR_FOR_ALL_ELEMS(zonesInRow, Zone*, thisZone){
      zoneWidth = (*thisZone).ZoneGetWidth();
      if (debugMode) {
	zoneBegin = (*thisZone).ZoneGetBegin();
	cout << "ZONE BEGINS AT: " << zoneBegin << endl;
      }
      cellsInZone = (*thisZone).ZoneGetCellsInZone();
      numCellsZone = cellsInZone.size();
      totalCellCount += numCellsZone;
      if (numCellsZone) {
	if (debugMode) {
	  cout << "CELL NAME\tWIDTH" << endl;
	}
	totCellWidthZone = 0;
	VECTOR_FOR_ALL_ELEMS(cellsInZone, Cell*, thisCell){
	  cellWidth = (*thisCell).CellGetWidth();
	  totCellWidthZone += cellWidth;
	  if (debugMode) {
	    cout << (*thisCell).CellGetName() << "\t"
		 << cellWidth << endl;
	  }
	} END_FOR;
	if (debugMode) {
	  cout << "TOTAL WIDTH OF ALL CELLS IN ZONE = " << totCellWidthZone 
	       << endl;
	}
      } else {
	if (debugMode) {
	  cout << "NO CELLS PRESENT IN ZONE " << endl;
	}
      }
      if (totCellWidthZone > zoneWidth) {
	cout << "ERROR: Design contains bins with excess supply " << endl;
	legal = false;
      } 
    } END_FOR;
    rowCount++;
  } END_FOR;
  if (legal) {
    cout << "==================> DESIGN LEGALIZED !!! " << endl;
  }
  cout << "TOTAL CELLS IN DESIGN " << totalCellCount << endl;
  /* LEGALIZE SELF CHECK: END */
  
  /* Plot the final supply and demand values for all bin to assert legality */
  LegalizePlotData("supply and demand bins", "post.plt", myDesign, DesignBins, PathBins);


  /* FINAL STEP OF LEGALIZATION */
  /* Removing overlaps of cells in each zone */
  VECTOR_FOR_ALL_ELEMS(DesignZones, vector<Zone*>, zonesInRow) {
    VECTOR_FOR_ALL_ELEMS(zonesInRow, Zone*, thisZone) {
      cellsInZone = (*thisZone).ZoneGetCellsInZone();
      sort(cellsInZone.begin(),cellsInZone.end(),ascendingX);
      nextCellBegin = (*thisZone).ZoneGetBegin();
      VECTOR_FOR_ALL_ELEMS(cellsInZone, Cell*, thisCell){
	(*thisCell).CellSetXpos(nextCellBegin);
	cellWidth = (*thisCell).CellGetWidth();
	nextCellBegin += cellWidth;
      } END_FOR;
      cellsInZone.clear();
    } END_FOR;
  } END_FOR;
  cout << "THANK YOU ..... Exiting " << endl;
}

/*###########################################################################################
  ################################### END OF CODE ###########################################
  ###########################################################################################
*/

