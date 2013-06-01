# include <Legalize.h>

using namespace std;

# define WEIGHT 0.0

int rowOfInterest = 56;
int binOfInterest = 7;
int currentRow;
bool doFractReassign = true;
bool ascendingX(Cell*, Cell*);
bool debugMode = true;


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


void
LegalizeFindZonesInRow(PhysRow* row, vector<Zone*> &zones, 
		       int rowBegin, int rowEnd)
{
  vector<Cell*> allCells;
  row->PhysRowGetCellsInRow(allCells);
  Cell* Obj;
  vector<Cell*> fixedCells;
  vector<Cell*> movableCells;
  /* Get all the fixed cells in the row */
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj){
    if (CellIsFixed(Obj)){
      fixedCells.push_back(Obj);
    } else {
      movableCells.push_back(Obj);
    }
  }END_FOR;
  
  vector<Zone*> blockedZones;
  bool firstBlockedZone = true;
  int prevBlockedZoneEnd = 0;
  int blockedZoneEnd = 0;
  Zone* thisZone;
  Zone* prevZone;

  /* Enter this block only if fixed cells or blockages are present */
  if(fixedCells.size()){
    /* Sort fixed cells in ascending order of x-pos */
    sort(fixedCells.begin(),fixedCells.end(),ascendingX);
    /* Iterate over all fixed cells to find the zones */
    VECTOR_FOR_ALL_ELEMS(fixedCells, Cell*, Obj){
      int xPos = (*Obj).CellGetXpos();
      if(firstBlockedZone){
	if (xPos == rowBegin) {
	  thisZone = new Zone(xPos, row, false);
	  zones.push_back(thisZone);
	  blockedZones.push_back(thisZone);
	  blockedZoneEnd = xPos + ((*Obj).CellGetWidth());
	  prevZone = thisZone;
	} else if (xPos > rowBegin) {
	  thisZone = new Zone(rowBegin, row, true);
	  zones.push_back(thisZone);
	  thisZone->ZoneSetEnd(xPos);
	  thisZone = new Zone(xPos, row, false);
	  zones.push_back(thisZone); 
	  blockedZones.push_back(thisZone);
	  prevZone = thisZone;
	  blockedZoneEnd = xPos + (*Obj).CellGetWidth();
	}
	firstBlockedZone = false;
      } else{
	/* Indicates more than one cell begins at the origin */
	if (xPos == rowBegin){
	  int thisCellEnd = xPos + (*Obj).CellGetWidth();
	  if (thisCellEnd > prevBlockedZoneEnd){
	    blockedZoneEnd = thisCellEnd;
	  }
	} else if (xPos <= prevBlockedZoneEnd){
	  /* Overlapping cells */
	  int thisCellEnd = xPos + (*Obj).CellGetWidth();
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
	  blockedZones.push_back(thisZone);
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
  
  unsigned int zoneEnd = zones.size();
  if (zoneEnd > 1) {
    /* First zone in a row is always bounded on the left and
       last zone in a row is always bounded on the right */
    zones[0]->ZoneSetLRBound(0);
    zones[(zoneEnd-1)]->ZoneSetLRBound(1);

    VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, Obj){
      int cellBegin = Obj->CellGetXpos();
      int cellEnd = cellBegin + (Obj->CellGetWidth());
      Zone* prevZone;
      Zone* zoneObj;

      /* If cell is inside a blocked zone push it to an empty zone */
      VECTOR_FOR_ALL_ELEMS(zones, Zone*, zoneObj){
	  if (!(zoneObj->ZoneGetEmpty())){
	    int zoneBegin =  (*zoneObj).ZoneGetBegin();
	    int zoneEnd =  (*zoneObj).ZoneGetEnd();      
	    if ((cellBegin > zoneBegin) && (cellBegin < zoneEnd)){
	    int newXpos;
	    /* Indicates the zone is on the left boundary of the row */
	    if ((zoneObj->ZoneGetLRBound()) == 0){
	      newXpos = zoneEnd;
	      Obj->CellSetXpos(newXpos);
	    } else if ((zoneObj->ZoneGetLRBound()) == 1){
	      /* Indicates the zone is on the right boundary of the row */
	      newXpos = zoneBegin - (*Obj).CellGetWidth();
	      Obj->CellSetXpos(newXpos);
	    } else if ((zoneObj->ZoneGetLRBound()) == 2){
	      int leftDist = cellBegin - zoneBegin;
	      int rightDist = zoneEnd - (cellBegin + (*Obj).CellGetWidth());
	      if (leftDist > rightDist){
		newXpos = zoneEnd;
	      } else{
		newXpos = zoneBegin - (*Obj).CellGetWidth();
	      }
	    }
	    Obj->CellSetXpos(newXpos);
	  }
	}	    
      }END_FOR;
    }END_FOR;
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
LegalizeDoFractReassign(vector<LegalizeBin*> &binsInRow)
{
  Cell* cellObj;
  int binOneBegin, binTwoBegin, binThreeBegin;
  int binOneEnd, binTwoEnd, binThreeEnd;
  int cellBegin, cellEnd, cellWidth;
  int binOneSupply, binTwoSupply;
  double gamma1, gamma2, gamma3;
  double binSupply;
  LegalizeBin *tempBin;
  LegalizeBin* binOne;
  LegalizeBin* binTwo = NIL(LegalizeBin*);
  LegalizeBin* binThree = NIL(LegalizeBin*);
  bool dPrint = false;
  bool fractAssign;
  double fract;
  int newCellWidth;
  int binWidth;

  VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, tempBin) {
    (*tempBin).BinRemoveAllFractCells();
  } END_FOR;
# if 0
  VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, tempBin) {
    map<Cell *, double> &fractCells = (*tempBin).BinGetAllFractCells();
    cout << "fractional cells after removal: " << fractCells.size();
  } END_FOR;
# endif

  VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, binOne){
    binSupply = (*binOne).BinGetSupply();
    if (binSupply <= 0) {
      vector<Cell*> cellsInBin;
      Cell *thisCell;
      cellsInBin = (*binOne).BinGetCellsInBin();
      VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
	(*binOne).BinSetCellFract(thisCell, 1.0);
      } END_FOR;
    } else {
      PhysRow *currRow = (*binOne).BinGetRow();
      int bIndex = (*binOne).BinGetIndex();
      int bOneTotalCellWidth = (*binOne).BinGetTotalCellWidth();
      int currentRow = (*currRow).PhysRowGetIndex();
      vector<Cell*> cellsInBin = (*binOne).BinGetCellsInBin();

      binOneBegin = (*binOne).BinGetBegin();
      binOneEnd = (*binOne).BinGetEnd();
      LegalizeGetNeighbours(binOne, binTwo, binThree, binsInRow);

      VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, cellObj){
	gamma2 = 0; gamma3 = 0; binTwoSupply = 0;
	cellBegin = (*cellObj).CellGetXpos();
	cellWidth = (*cellObj).CellGetWidth();
	cellEnd = cellBegin + cellWidth;
	gamma1 = LegalizeThetaFunc(binOneBegin, binOneEnd, cellBegin, cellEnd);
	if (binTwo) {
	  binTwoSupply = (*binTwo).BinGetSupply();
	  binTwoBegin = (*binTwo).BinGetBegin();
	  binTwoEnd = (*binTwo).BinGetEnd();
	  gamma2 = LegalizeThetaFunc(binTwoBegin, binTwoEnd, cellBegin, cellEnd);
	}
	if (binThree) {
	  binThreeBegin = (*binThree).BinGetBegin();
	  binThreeEnd = (*binThree).BinGetEnd();
	  /* If cells spans over 3 bins, then do not do any fractional reassignment */
	  gamma3 = LegalizeThetaFunc(binThreeBegin, binThreeEnd, cellBegin, cellEnd);
	}
	if (gamma3 > 0) {
	  fractAssign = false;
	}
	if (gamma2 == 0 || binTwoSupply > 0) {
	  fractAssign = false;
	}

	/* Assign cell fractionally to bin 1 */
	fract = gamma1 / cellWidth;
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
LegalizeAugPathAlgo(HyperGraph &myGraph, LegalizeBin *suppNode, vector<LegalizeBin *> &PathBins)
{

  PathBins.clear();
  void *NodeObj;
  uint NodeIdx;
  LegalizeBin *currentNode;
  LegalizeBin *destNode;
  map<LegalizeBin*, bool> usedNodes;
  map<LegalizeBin*,LegalizeBin*> pathMap;
  map<Cell*, double> cellsToMove;
  vector<pair<LegalizeBin* , double> > binsLValues;
  uint numEdges, numNodes;
  
  numEdges = myGraph.GetNumEdges();
  numNodes = myGraph.HyperGraphGetNumNodes();
  
  vector<map<Cell*, double> > chosenCells(numEdges);
  vector<double> totalFractWidths(numEdges);

  /* Get A(s) = All the fractional cells in source bin suppNode */
  map<Cell*, double> AofV = (*suppNode).BinGetAllFractCells();

  /* Set the L-value of source node to 0 */
  LegalizeBinSetLValue(suppNode, 0.0);

  /* Set the b(v) of the supply bin to be 0 */
  double supply = (*suppNode).BinGetSupply();
  LegalizeBinSetBValue(suppNode, supply);

  /* Set the b(v) of all other supply bins except the source bin to be 0 */
  HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
    double supply = (*(LegalizeBin*)NodeObj).BinGetSupply();
    if ((LegalizeBin*)NodeObj != suppNode) {
      if (supply <= 0.0) {
	LegalizeBinSetBValue((LegalizeBin*)NodeObj, supply);
      } else {
	LegalizeBinSetBValue((LegalizeBin*)NodeObj, 0.0);
      }
    }
  } HYPERGRAPH_END_FOR;
  
  
  double fractWidthSelected;
  double currentNodeLVal;
  uint binCount = 0;
  uint iteration = 0;
  LegalizeBin *prevNode;

  /* Set destination node to Null */
  destNode = NIL(LegalizeBin*);
  
  /* Set current node to source node */
  currentNode = suppNode;
  /* Get the L(v) of current node and push to the vector of L(v) */
  currentNodeLVal = LegalizeBinGetLValue(currentNode);
  binsLValues.push_back(make_pair(currentNode, currentNodeLVal));

  while(1) {
    if (debugMode) {
      cout << "########### INNER ITERATION " << iteration << " ######### " << endl;
      cout << endl;
      cout << "CURRENT NODE " << endl;
      LegalizePrintBinData(currentNode);
      cout << endl;
    }

    /* If v = s */
    if (currentNode == suppNode) {
      if (debugMode) {
	cout << " V EQUAL S " << endl;
      }
    } else if (currentNode != suppNode) {
      /* If v not equal s */
      if (debugMode) {
	cout << " V NOT EQUAL S  " << endl;
      }

      double bValueCurrent = LegalizeBinGetBValue(currentNode);
      if (debugMode) {
	cout << "B(V) OF CURRENT NODE BEFORE UPDATING " << bValueCurrent << endl;
      }

      prevNode = pathMap[currentNode];
      
    
      /* b(v) = b(v) + width(C(P(v),v)) */
      int edgeIdx = (int)myGraph.HyperGraphNodesAreAdjacent((void*)prevNode, (void*)currentNode);
      if (edgeIdx != -1) {
	fractWidthSelected = totalFractWidths[edgeIdx];
	if (debugMode) {
	  cout << " C(P(v),v) = " << fractWidthSelected << endl;
	  if (fractWidthSelected == 0) {
	    cout << "BREAK HERE" << endl;
	    cout << "ERROR!! NO CELLS TO MOVE" << endl;
	  }
	}
      }
      double modBValue = bValueCurrent + fractWidthSelected;
      LegalizeBinSetBValue(currentNode, modBValue);
      if (debugMode) {
	cout << "B(V) OF CURRENT NODE AFTER UPDATING " << LegalizeBinGetBValue(currentNode) << endl;
      }
      
      map<Cell*, double> moveCells;
      /* A(v) = GAMMA(v) U C(P(v),v)) */
      moveCells = chosenCells[edgeIdx];
      AofV = (*currentNode).BinGetAllFractCells();
      
      Cell* Key;
      double Value;
      cout << "\nCELLS ALREADY PRESENT IN BIN " << endl;
      MAP_FOR_ALL_ELEMS(AofV, Cell*, double, Key, Value){
	cout << (*Key).CellGetName() << ",";
      } END_FOR;
      cout << endl;
      
      cout << "\nCELLS TO BE MOVED INT0 THE BIN " << endl;
      MAP_FOR_ALL_ELEMS(moveCells, Cell*, double, Key, Value){
	_KEY_EXISTS(AofV, Key){
	  AofV[Key] = AofV[Key] + Value;
	} else {
	  AofV[Key] = Value;
	  cout << (*Key).CellGetName() << ",";
	}
      } END_FOR;
      cout << endl;

      cout << "\nCELLS AFTER MOVING IN THE BIN " << endl;
      MAP_FOR_ALL_ELEMS(AofV, Cell*, double, Key, Value){
	cout << (*Key).CellGetName() << ",";
      } END_FOR;
      cout << endl;
    }

    /* If B(v) <= 0, Set t = v. Break */
    double currentBVal = LegalizeBinGetBValue(currentNode);
    if (currentBVal <= 0) {
      if (debugMode) {
	cout << " B(V) = " << currentBVal << " LESS THAN EQUAL ZERO FOR ITERATION " << iteration << endl;
      }
      destNode = currentNode;
      break;
    }
      
    
    /* U := U\{v} */
    _KEY_EXISTS(usedNodes, currentNode) {
    } else {
      usedNodes[currentNode] = true;
      cout << "ADDED TO USED NODES ";
      LegalizePrintBinData(currentNode);
    }
    
    cout << "NUMNODES USED TILL THIS POINT " << usedNodes.size() << endl;
    if (usedNodes.size() == numNodes) {
      break;
    }
    
    NodeObj = (void*)currentNode;
    uint EdgeIdx;
    double weight;
      
    vector<LegalizeBin*> adjacentBins;
    int currentNodeY = (*currentNode).BinGetBot();
    int adjNodeY;
    double moveCost;
      
    //    map<Cell*, double> binCells = (*currentNode).BinGetAllFractCells();
    map<Cell*, double> binCells;
    binCells = AofV;
    double totalFractWidth;
    double currentLVal, adjLVal, modAdjLVal;
    currentLVal = LegalizeBinGetLValue(currentNode);
    cout << " CURRENT NODE L VALUE " << currentLVal << endl;
    adjacentBins.clear();
    
    /* for w belongs to V(G) with (v,w) belongs to E(G) do */ 
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(myGraph, NodeObj, EdgeIdx, weight){
      LegalizeBin *adjacentNode = (LegalizeBin*)myGraph.GraphGetOtherNodeOfEdge(EdgeIdx, NodeObj);
      _KEY_EXISTS(usedNodes, adjacentNode) {
	continue;
      } else {
	adjacentBins.push_back(adjacentNode);
	adjNodeY = (*adjacentNode).BinGetBot();
	/* Choose the set C(v,w) */
	if (currentNodeY == adjNodeY) {
	  moveCost = LegalizeGetCellsToMove((*currentNode), (*adjacentNode), binCells, true,
					    cellsToMove, totalFractWidth);
	  chosenCells[EdgeIdx] = cellsToMove;
	  totalFractWidths[EdgeIdx] = totalFractWidth;
	} else {
	  moveCost = LegalizeGetCellsToMove((*currentNode), (*adjacentNode), binCells, false,
					    cellsToMove, totalFractWidth);
	  chosenCells[EdgeIdx] = cellsToMove;
	  totalFractWidths[EdgeIdx] = totalFractWidth;
	}
      
      	if (debugMode) {
	  if (totalFractWidth == 0) {
	    PhysRow *thisRow;
	    int binIdx, rowIdx;
	    thisRow = (*currentNode).BinGetRow();
	    rowIdx = (*thisRow).PhysRowGetIndex();
	    binIdx = (*currentNode).BinGetIndex();
	    cout << "CELLS TO MOVE SIZE " << cellsToMove.size();
	    cout << "FRACT WIDTH " << totalFractWidth << endl;
	    cout << "BIN " << rowIdx << "[" << binIdx << "] ---> " ;
	    thisRow = (*adjacentNode).BinGetRow();
	    rowIdx = (*thisRow).PhysRowGetIndex();
	    binIdx = (*adjacentNode).BinGetIndex();
	    cout << rowIdx << "[" << binIdx << "]  HAS ZERO CELLS TO MOVE " << endl;
	  }
	}
	/* If L(w) > L(v) + c(v,w), L(w):= L(v) + c(v,w)
	   P(w) := v */
	adjLVal = LegalizeBinGetLValue(adjacentNode);
	modAdjLVal = currentLVal + moveCost;
	if (modAdjLVal < adjLVal) {
	  LegalizeBinSetLValue(adjacentNode, modAdjLVal);
	  /* Push all the L-values calculated in this step to a vector */
	  /* Sort the vector in ascending order to get the node with least L-value */
	  binsLValues.push_back(make_pair(adjacentNode, modAdjLVal));
	  pathMap[adjacentNode] = currentNode;
	  cout << "ADJACENT BIN L VALUE " << endl;
	  LegalizePrintBinData(adjacentNode);
	  cout << modAdjLVal <<  endl;
	
	}
      } 
    } HYPERGRAPH_END_FOR;
    
    /* Stop iterating if a node with no edges is found 
    if (!adjacentBins.size()) {
      break;
      } */
    sort(binsLValues.begin(), binsLValues.end(), sortLValues);
    
    /* Choose bin with minimum L(v) for next iteration */
    pair<LegalizeBin*, double> binLValObj;
    cout << " BINS ACTIVE SO FAR " << endl;
    VECTOR_FOR_ALL_ELEMS(binsLValues, pair<LegalizeBin* MCOMMA double>, binLValObj){
      _KEY_EXISTS(usedNodes,(binLValObj.first)) {
	continue;
      } else {
	LegalizePrintBinData(binLValObj.first);
	cout << " L-VAL " << binLValObj.second << endl;
      }
    } END_FOR;
       
    
    VECTOR_FOR_ALL_ELEMS(binsLValues, pair<LegalizeBin* MCOMMA double>, binLValObj){
      _KEY_EXISTS(usedNodes,(binLValObj.first)) {
	continue;
      } else {
	currentNode = binLValObj.first;
	cout << "BIN WITH LEAST L_VAL " << endl;
	LegalizePrintBinData(currentNode);
	cout << binLValObj.second << endl;
	break;
      }
    } END_FOR;
    
    iteration ++;
  }

  /* Found a demand bin */
  if (destNode != NIL(LegalizeBin*)) {  
    LegalizeBin *currentBin; 
    LegalizeBin *prevBin;
    LegalizeBin *nextBin;
    vector<LegalizeBin*> reversePath;
    int binX, binY;
    bool exitWhile;
    PhysRow *currentRow;
    PhysRow *nextRow;
    currentBin = destNode;
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
      

    currentBin = reversePath.back();
    reversePath.pop_back();
    do {
      nextBin = reversePath.back();
      reversePath.pop_back();
      currentRow = (*currentBin).BinGetRow();
      nextRow = (*nextBin).BinGetRow();

      binX = (*nextBin).BinGetBegin();
      binY = (*nextBin).BinGetBot();
      int edgeIdx = myGraph.HyperGraphNodesAreAdjacent((void*)currentBin, (void*)nextBin);
      cellsToMove = chosenCells[edgeIdx];
      vector<Cell*> cellsCurrBin = (*currentBin).BinGetCellsInBin();
      Cell *thisCell;
      double cellFract;
      int thisCellY;

      map<Cell*, double>::iterator itr;
      int currBinSize = cellsCurrBin.size();
      for(int cellCount = 0; cellCount < currBinSize; cellCount++) {
	thisCell = cellsCurrBin[cellCount];
	thisCellY = (*thisCell).CellGetWidth();
	_KEY_EXISTS_WITH_VAL (cellsToMove, thisCell, itr) {
	  cellFract = itr->second;
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
    } while(currentBin != destNode);
    
  } else {
    cout << "ERROR! ---> No destination node found" <<endl;
  }
  LegalizeBinClearAllLValue();
  LegalizeBinClearAllBValue();
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
  void *NodeObj;
  double bValueCurrent;
  double currentNodeLValue;
  double fractWidthSelected;
  double supply;
  double currentLVal, adjLVal, modAdjLVal, moveCost;
  double totalFractWidth;
  uint numEdges, numNodes;
  uint NodeIdx;
  uint iteration;
  int edgeIdx, binX, binY;
  bool sameZone;

  numEdges = myGraph.GetNumEdges();
  numNodes = myGraph.HyperGraphGetNumNodes();
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
      Cell *Key;
      double Value;
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
    double weight;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(myGraph, NodeObj, edgeIdx, weight){
      adjacentNode = (LegalizeBin*)myGraph.GraphGetOtherNodeOfEdge(edgeIdx, NodeObj);
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
    cout << "Destination bin found: ";
    LegalizePrintBinData(destBin);
    cout << endl;
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
    
    currBinZone = (*currentBin).BinGetZone();
    nextBinZone = (*nextBin).BinGetZone();
    
    sameZone = true;
    if (currBinZone != nextBinZone) {
      sameZone = false;
    }

    binX = (*nextBin).BinGetBegin();
    binY = (*nextBin).BinGetBot();

    edgeIdx = myGraph.HyperGraphNodesAreAdjacent((void*)currentBin, (void*)nextBin);
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
	  // (*currentBin).BinRemoveCellFromFract(thisCell);
	  (*nextBin).BinAddCellToBin(thisCell);
	  
	  if (!sameZone) {
	    (*currBinZone).ZoneRemoveCellFromZone(thisCell);
	    (*nextBinZone).ZoneAddCellToZone(thisCell);
	  }

	  // (*nextBin).BinSetCellFract(thisCell, 1.0);
	  if (thisCellY != binY) {
	    (*currentRow).PhysRowRemoveCellFromRow(thisCell);
	    (*nextRow).PhysRowAddCellToRow(thisCell);
	  }
	}
      }
    } else {
      cout << "SEVERE ERROR: No cells were moved to current bin " << endl;
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

void
LegalizeAssignLValues(vector<vector<LegalizeBin*> > &designBins)
{
  vector<LegalizeBin*> binsObj;
  VECTOR_FOR_ALL_ELEMS(designBins, vector<LegalizeBin*>, binsObj){
    LegalizeBin *thisBin;
    VECTOR_FOR_ALL_ELEMS(binsObj, LegalizeBin*, thisBin) {
      bool isUnblocked = (*thisBin).BinGetEmpty();
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
  vector<vector<LegalizeBin*> > DesignBins;
  vector<vector<Zone*> > DesignZones;
  vector<PhysRow*> allPhysRows;
  vector<vector<int> > allRowBounds;
  vector<LegalizeBin*> binsInRow;
  vector<Cell*> cellsInBin;
  vector<Cell*> cellsInRow;
  vector<LegalizeBin*> supplyBins;
  vector<LegalizeBin *> PathBins;
  rowOrientation rowType;
  HyperGraph myGraph;
  LegalizeBin *thisBin;
  PhysRow* Obj;				  
  Cell *thisCell, *CellPtr, *cellObj;
  int rowBegin, rowEnd, columnWidth;
  string CellName;

/* Get all the physical rows in Design */
  allPhysRows  = myDesign.DesignGetRows();
  
  /* Get bounding boxes for all subrows in all rows in Design */
  LegalizeGetAllBoundingBoxes(allPhysRows, allRowBounds);
  rowType=((allPhysRows[0])->PhysRowGetType());

  /* Checking the Legality of all Cells and setting the variable isLegal
     for all cells accordingly */ 
  int maxCellWidth = 0;
  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal())) {
      int cellWidth = (*CellPtr).CellGetWidth();
      if (cellWidth > maxCellWidth) {
	maxCellWidth = cellWidth;
      }
      LegalizeCheckLegality(CellPtr, allPhysRows, allRowBounds);
    }
  } DESIGN_END_FOR;
  columnWidth = 2 * maxCellWidth;
  cout << "COLUMN WIDTH= " << columnWidth << endl;
  
  
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

  rowBegin = ((allPhysRows[0])->PhysRowGetRowBegin());
  rowEnd = ((allPhysRows[0])->PhysRowCalculateRowEnd());

  /* Create the zones for each row and hence the bins */
  LegalizeCreateBins(myDesign, columnWidth, DesignBins, DesignZones);
  
  /* Calculate supply for all bins in design */
  LegalizeCalcSuppForAllBins(DesignBins, supplyBins);

  cout << "SUPPLY BINS AFTER CALCSUPP " << supplyBins.size() << endl;
  /* Assign cells fractionally to reduce the supply of supply nodes */
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsInRow){
    if (doFractReassign)    
      LegalizeDoFractReassign(binsInRow);
  } END_FOR;
  
  /* RECALCULATING SUPPLY AFTER FRACTIONAL REASSIGNMENT */
  LegalizeReCalcSuppForAllBins(DesignBins, supplyBins);
  
  /* CREATE GRAPH BY ADDING NODES AND EDGES */
  /* Add nodes to the graph */
  vector<LegalizeBin*> binsObj;
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsObj){
    LegalizeBin *thisBin;
    VECTOR_FOR_ALL_ELEMS(binsObj, LegalizeBin*, thisBin) {
      /* If bin is unblocked add it to the graph */
      bool isUnblocked = (*thisBin).BinGetEmpty();
      if (isUnblocked) {
	myGraph.HyperGraphAddNode((void*)thisBin);
      }
    } END_FOR;
  } END_FOR;
  
  /* Assign default L(v) values */
  LegalizeAssignLValues(DesignBins);
  
  /* Add edges to the graph */
  map<LegalizeBin*, uint> binsInGraph;
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsObj){
    LegalizeBin *thisBin;
    vector<void*> oneEdge;
    VECTOR_FOR_ALL_ELEMS(binsObj, LegalizeBin*, thisBin) {
      bool isUnblocked = (*thisBin).BinGetEmpty();
      if (isUnblocked) {
	binsInGraph[thisBin] = 1;       // Add the bin to map to indicate all edges to the bin has been formed
	LegalizeBin *topBin;
	LegalizeBin *botBin;
	LegalizeBin *leftBin;
	LegalizeBin *rightBin;
	LegalizeGetAdjacentBins(thisBin, leftBin, rightBin, 
				topBin, botBin, DesignBins);
	if (topBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, topBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)topBin);
	    myGraph.HyperGraphAddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
	if (botBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, botBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)botBin);
	    myGraph.HyperGraphAddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
	if (leftBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, leftBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)leftBin);
	    myGraph.HyperGraphAddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
	if (rightBin != NIL(LegalizeBin*)) {
	  _KEY_EXISTS(binsInGraph, rightBin) {
	  } else {
	    oneEdge.push_back((void*)thisBin);
	    oneEdge.push_back((void*)rightBin);
	    myGraph.HyperGraphAddEdge(oneEdge, NIL(void*), WEIGHT);
	    oneEdge.clear();
	  }
	}
      }
    } END_FOR;
  } END_FOR;
  
  /* Making a copy of the initial vector of supply bins */
  cout << "SUPPLY HEAP " << supplyBins.size() << endl;
  
  /* Creating the max heap */
  make_heap(supplyBins.begin(), supplyBins.end(), maxSupply);

  /* Get the first element = max element from the heap */
  LegalizeBin *maxSuppBin;
  LegalizeBin *tempBin;
  uint iteration = 0;

  LegalizePlotData("supply and demand bins", "pre.plt", myDesign, DesignBins, PathBins);

  while (supplyBins.size()) {
    maxSuppBin = supplyBins.front();
    vector<LegalizeBin *> supplyBinsCpy = supplyBins;
    cout << "Supply bin: ";
    LegalizePrintBinData(maxSuppBin);
    cout << endl;
    cout << "Before AUG PATH ALGO: " << supplyBins.size() << endl;
    /*
    VECTOR_FOR_ALL_ELEMS(supplyBinsCpy, LegalizeBin *, tempBin) {
      cout << " Bin: ");
      LegalizePrintBinData(tempBin);
      cout << " Supply: " << (*tempBin).BinGetSupply() << endl;
    } END_FOR;
    */
    LegalizeAugPathAlgoNew(myGraph, maxSuppBin, PathBins);
    
    
    LegalizeCalcSuppForAllBins(DesignBins, supplyBins);
    //    cout << "After moving cells and reassigning to bins:" << supplyBins.size() << endl;
    cout << "SUPPLY BINS AFTER CALCSUPP " << supplyBins.size() << endl;

    /* Do fract re-assign in this step before the next iteration */
    vector<LegalizeBin*> binsInRow;
    VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsInRow){
      if (doFractReassign) {
	LegalizeDoFractReassign(binsInRow);
      }
    } END_FOR;
    
    /* ReCalculate Supplies after doing fractional reassignment of cells */
    LegalizeReCalcSuppForAllBins(DesignBins, supplyBins);
    cout << "After fractional reassignment:" << supplyBins.size() << endl;

    /*
    cout << "After AUG PATH ALGO: " << endl;
    VECTOR_FOR_ALL_ELEMS(supplyBinsCpy, LegalizeBin *, tempBin) {
      cout << " Bin: ";
      LegalizePrintBinData(tempBin);
      cout << " Supply: " << (*tempBin).BinGetSupply() << endl;
    } END_FOR;
    */
    LegalizeAssignLValues(DesignBins);

    make_heap(supplyBins.begin(), supplyBins.end(), maxSupply);
    
    iteration++;
    //    if (iteration > 2) {
    //      break;
    //    }
  }

  int totalCellCount = 0;
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsInRow){
    LegalizeBin *thisBin;
    map<Cell*, double> cellFracts;
    vector<Cell*> cellsInBin;
    double totalFractWidthInRow;
    int totalCellWidthInRow;
    totalFractWidthInRow = 0.0;
    totalCellWidthInRow = 0;
    VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, thisBin){
      cellFracts = (*thisBin).BinGetAllFractCells();
      cellsInBin = (*thisBin).BinGetCellsInBin();
      totalCellCount += (cellsInBin).size();
      LegalizePrintBinData(thisBin);
      double totalCellWidth;
      Cell *thisCell;
      double value;
      cout << " BIN WIDTH " << (*thisBin).BinGetWidth() << endl;
      cout << "CELLS IN BIN ARE " << endl;
      totalCellWidth = 0.0;
      MAP_FOR_ALL_ELEMS(cellFracts, Cell*, double, thisCell, value){
	int cellWidth = (*thisCell).CellGetWidth() ;
	cout << (*thisCell).CellGetName() << " WIDTH " << cellWidth
	     << " FRACTION : " << value << endl;
	totalCellWidth += value * cellWidth;
	totalCellWidthInRow += cellWidth;
      } END_FOR;
      totalFractWidthInRow += totalCellWidth;
      cout << endl;
      cout << "TOTAL CELL WIDTH IN BIN " << totalCellWidth << endl;\
      if (totalCellWidth > ((*thisBin).BinGetWidth())) {
	cout << "ERROR: EXCESS CELLS NOT REMOVED YET " << endl;
      }
    } END_FOR;
    cout << "TOTAL CELL WIDTH IN ROW : " << totalCellWidthInRow << endl;
    cout << "TOTAL FRACT CELL WIDTH IN ROW : " << totalFractWidthInRow << endl;
  } END_FOR;
  cout << "TOTAL CELLS IN DESIGN " << totalCellCount << endl;
  
  

  LegalizePlotData("supply and demand bins", "post.plt", myDesign, DesignBins, PathBins);

  
  int numRows = allPhysRows.size();
  vector<Zone*> zonesInRow;
  vector<Cell*> cellsInZone;
  Zone *thisZone, *binInZone;
  int zoneBegin, zoneEnd, zoneBot;
  int cellWidth;
  int nextCellBegin;
  int cellX, cellY;
  int rowCount, zoneCount; 
  int numCellsInRow;
  rowCount = 0;
  

  VECTOR_FOR_ALL_ELEMS(DesignZones, vector<Zone*>, zonesInRow) {
    VECTOR_FOR_ALL_ELEMS(zonesInRow, Zone*, thisZone) {
      cellsInZone = (*thisZone).ZoneGetCellsInZone();
      sort(cellsInZone.begin(),cellsInZone.end(),ascendingX);
      nextCellBegin = (*thisZone).ZoneGetBegin();
      int totalWidth = 0;
      int zoneWidth = (*thisZone).ZoneGetWidth();
      vector<Cell*> allCellsInRow;
      (allPhysRows[rowCount])->PhysRowGetCellsInRow(allCellsInRow);
      numCellsInRow = allCellsInRow.size();
      
      VECTOR_FOR_ALL_ELEMS(cellsInZone, Cell*, thisCell){
	(*thisCell).CellSetXpos(nextCellBegin);
	cellWidth = (*thisCell).CellGetWidth();
	totalWidth += cellWidth;
	cout << " CELL: " << (*thisCell).CellGetName() <<  "\t"
	     << "WIDTH: " << cellWidth << endl;
	nextCellBegin += cellWidth;
      } END_FOR;
      cout << "NUMBER OF CELLS IN ZONE : " << rowCount << " " << cellsInZone.size() << endl;
      cout << "NUMBER OF CELLS IN ROW : " << numCellsInRow << endl;
      if (totalWidth > zoneWidth) {
	cout << "TOTAL CELL WIDTH = " << totalWidth << endl;
	cout << "ZONE WIDTH = " << zoneWidth << endl;
	cout << "ERROR : Supply not removed " << endl;
      }
      cellsInZone.clear();
    } END_FOR;
    rowCount++;
  } END_FOR;
}

/*###########################################################################################
  ################################### END OF CODE ###########################################
  ###########################################################################################
*/

