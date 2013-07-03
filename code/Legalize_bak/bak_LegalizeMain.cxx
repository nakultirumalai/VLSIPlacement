# include <Legalize.h>

using namespace std;

# define WEIGHT 0.0

int rowOfInterest = 29;
int binOfInterest = 4;
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
ascendingBins(pair<int, unsigned int> p1, pair<int, unsigned int> p2)
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
	  thisZone = new Zone(xPos, false);
	  zones.push_back(thisZone);
	  blockedZones.push_back(thisZone);
	  blockedZoneEnd = xPos + ((*Obj).CellGetWidth());
	  prevZone = thisZone;
	} else if (xPos > rowBegin) {
	  thisZone = new Zone(rowBegin, true);
	  zones.push_back(thisZone);
	  thisZone->ZoneSetEnd(xPos);
	  thisZone = new Zone(xPos, false);
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
	  thisZone = new Zone(prevBlockedZoneEnd, true);
	  zones.push_back(thisZone);
	  thisZone->ZoneSetEnd(xPos);
	  /* Create the next blocked zone */
	  thisZone = new Zone(xPos, false);
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
      thisZone = new Zone(prevBlockedZoneEnd, true);
      thisZone->ZoneSetEnd(rowEnd);
      zones.push_back(thisZone);
    } else if (prevBlockedZoneEnd == rowEnd){
      prevZone->ZoneSetEnd(prevBlockedZoneEnd);
    }
  }
  /* If row does not have any cell(s) that qualfies as fixed */
  if (!(fixedCells.size())){
    thisZone = new Zone(rowBegin, true);
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
  Zone* zoneObj;
  VECTOR_FOR_ALL_ELEMS(allZones, Zone*, zoneObj){
    int zBegin = zoneObj->ZoneGetBegin();
    unsigned int flag = (unsigned int)(zoneObj->ZoneGetEmpty());
    zones.push_back(make_pair(zBegin, flag));
  }END_FOR;
  
  /* Finding the union of zones and column boundaries
     to give bins */  
  bins.insert(bins.end(), zones.begin(), zones.end());
  sort(bins.begin(),bins.end(), ascendingBins);
  
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
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
    cellBegin = cellObj->CellGetXpos();
    VECTOR_FOR_ALL_ELEMS(allBins, LegalizeBin*, binObj){
      if (binObj->BinGetEmpty()){
	int begin = binObj->BinGetBegin();
	int end = binObj->BinGetEnd();
	if ((cellBegin >= begin) && (cellBegin < end)){
	    binObj->BinAddCellToBin(cellObj);
	}
      }
    } END_FOR;
  } END_FOR;
}

void
LegalizeCalcSuppForBins(vector<LegalizeBin*> &allBins, 
			vector<LegalizeBin*> &supplyBins)		
{
  LegalizeBin* bin;
  VECTOR_FOR_ALL_ELEMS(allBins, LegalizeBin*, bin){
    if (bin->BinGetEmpty()){
      int wCells = bin->BinGetTotalCellWidth();
      int wBin = bin->BinGetWidth();
      int supply = wCells - wBin;
      if (supply > 0) {
	supplyBins.push_back(bin);
      }
      bin->BinSetSupply(supply);
    }
  } END_FOR;
}

void
LegalizeReCalcSuppAllBins(vector<vector<LegalizeBin*> > &designBins, 
			  vector<LegalizeBin*> &supplyBins)
{
  vector<LegalizeBin*> binsInRow;
  LegalizeBin *thisBin;
  int binsInRowSize;
  int wCells, wBin, supply;
  VECTOR_FOR_ALL_ELEMS(designBins, vector<LegalizeBin*>, binsInRow){
    binsInRowSize = binsInRow.size();
    if (binsInRowSize) {
      VECTOR_FOR_ALL_ELEMS(binsInRow, LegalizeBin*, thisBin) {
	if ((*thisBin).BinGetEmpty()) {
	  wCells = (*thisBin).BinGetTotalCellWidth();
	  wBin = (*thisBin).BinGetWidth();
	  supply = wCells - wBin;
	  if (supply > 0) {
	    supplyBins.push_back(thisBin);
	  } 
	  (*thisBin).BinSetSupply(supply);
	}
      } END_FOR;
    }
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
  binTwo = allBins[idxTwo];
  binThree = allBins[idxThree];
}
  

  
void 
LegalizeDoFractReassign(vector<LegalizeBin*> &suppBins, 
			vector<LegalizeBin*> &allBins)

{
  int binOneBegin, binTwoBegin, binThreeBegin;
  int binOneEnd, binTwoEnd, binThreeEnd;
  int cellBegin, cellEnd, cellWidth;
  int binOneSupply, binTwoSupply;
  double gamma1, gamma2, gamma3;
  LegalizeBin* binOne;
  LegalizeBin* binTwo = NIL(LegalizeBin*);
  LegalizeBin* binThree = NIL(LegalizeBin*);
  bool dPrint = false;
  VECTOR_FOR_ALL_ELEMS(suppBins, LegalizeBin*, binOne){
    int bIndex = (*binOne).BinGetIndex();
    if ((currentRow == rowOfInterest) && (binOfInterest == bIndex)) {
      dPrint = false;
    }
    binOneBegin = (*binOne).BinGetBegin();
    binOneEnd = (*binOne).BinGetEnd();
    binOneSupply = (*binOne).BinGetSupply();
    int bOneTotalCellWidth = (*binOne).BinGetTotalCellWidth();
    if (dPrint) {
      cout << "ROW " << rowOfInterest << " BIN " << binOfInterest << endl;
      cout << " BEGIN " << binOneBegin << " END " << binOneEnd 
	   << " SUPPLY " << binOneSupply << " TOTAL CELL WIDTH " 
	   << bOneTotalCellWidth << endl;
    }
    LegalizeGetNeighbours(binOne, binTwo, binThree, allBins);
    vector<Cell*> cellsInBin = (*binOne).BinGetCellsInBin();
    bool condBinVar = (binTwo && binThree && varOption);
    if (dPrint && condBinVar) {
      cout << " PROCESSING FIRST BLOCK " << endl;
    } else if (dPrint && !condBinVar) {
      cout <<  " PROCESSING SECOND BLOCK " << endl;
    }
    int cellCount = 0;
    Cell* cellObj;
    VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, cellObj){
      cellBegin = (*cellObj).CellGetXpos();
      cellWidth = (*cellObj).CellGetWidth();
      cellEnd = cellBegin + cellWidth;
      if (dPrint) {
	cout << "CELL " << cellCount << " BEGINS AT " << cellBegin
	     << " ENDS AT " << cellEnd << " WIDTH " << cellWidth << endl;
      }
      if (condBinVar) {
	
	binTwoBegin = (*binTwo).BinGetBegin();
	binTwoEnd = (*binTwo).BinGetEnd();

	binThreeBegin = (*binThree).BinGetBegin();
	binThreeEnd = (*binThree).BinGetEnd();

	gamma1 = LegalizeThetaFunc(binOneBegin, binOneEnd, cellBegin, cellEnd);
	gamma2 = LegalizeThetaFunc(binTwoBegin, binTwoEnd, cellBegin, cellEnd);
	gamma3 = LegalizeThetaFunc(binThreeBegin, binThreeEnd, cellBegin, cellEnd);
	if (gamma3 > 0) {
	  double fract = gamma3 / cellWidth;
	  (*binThree).BinSetCellFract(cellObj, fract);
	  int newCellWidth = (*binThree).BinGetTotalCellWidth() + gamma3;
	  int binWidth = (*binThree).BinGetWidth();
	  int supply = newCellWidth - binWidth;
	  (*binThree).BinSetTotalCellWidth(newCellWidth);
	  (*binThree).BinSetSupply(supply);
	}
	if (gamma2 > 0) {
	  double fract = gamma2 / cellWidth;
	  (*binTwo).BinSetCellFract(cellObj, fract);
	  int newCellWidth = (*binTwo).BinGetTotalCellWidth() + gamma2;
	  int binWidth = (*binTwo).BinGetWidth();
	  int supply = newCellWidth- binWidth;
	  (*binTwo).BinSetTotalCellWidth(newCellWidth);
	  (*binTwo).BinSetSupply(supply);
	  
	}
	double fract = gamma1 / cellWidth;
	(*binOne).BinSetCellFract(cellObj, fract);
	int newCellWidth = (*binOne).BinGetTotalCellWidth() - cellWidth + gamma1;
	int binWidth = (*binOne).BinGetWidth();
	int supply = newCellWidth - binWidth;
	(*binOne).BinSetTotalCellWidth(newCellWidth);
	(*binOne).BinSetSupply(supply);
	  
      } else {
	if (binTwo) {
	  binTwoBegin = (*binTwo).BinGetBegin();
	  binTwoEnd = (*binTwo).BinGetEnd();
	  binTwoSupply = (*binTwo).BinGetSupply();
	  gamma1 = LegalizeThetaFunc(binOneBegin, binOneEnd, cellBegin, cellEnd);
	  gamma2 = LegalizeThetaFunc(binTwoBegin, binTwoEnd, cellBegin, cellEnd);
	  if (dPrint) {
	    cout << "BIN TWO BEGIN: " << binTwoBegin << " END: " << binTwoEnd
		 << " SUPPLY BEFORE REASSIGNMENT " << binTwoSupply << endl;
	    cout << " GAMMA ONE " << gamma1 << " GAMMA TWO " << gamma2 << endl;
	  }
	}
	if (binThree) {
	  binThreeBegin = (*binThree).BinGetBegin();
	  binThreeEnd = (*binThree).BinGetEnd();
	  int binThreeSupply = (*binThree).BinGetSupply();
	  gamma3 = LegalizeThetaFunc(binThreeBegin, binThreeEnd, cellBegin, cellEnd);
	  if (dPrint) {
	    cout << "BIN THREE BEGIN: " << binThreeBegin << " END: " << binThreeEnd
		 << " SUPPLY BEFORE REASSIGNMENT " << binThreeSupply << endl;
	    cout << " GAMMA THREE " << gamma3 << endl;
	  }
	}
	if (gamma3 > 0) {
	  continue;
	}
	if ((gamma2 > 0) && (binTwoSupply < 0)){
	  double fract = gamma1 / cellWidth;
	  int newCellWidth = (*binOne).BinGetTotalCellWidth() - cellWidth + gamma1;
	  int binWidth = (*binOne).BinGetWidth();
	  int supply = newCellWidth - binWidth;
	  (*binOne).BinSetTotalCellWidth(newCellWidth);
	  (*binOne).BinSetSupply(supply);
	  (*binOne).BinSetCellFract(cellObj, fract);
	  if (dPrint) {
	    cout << "NEW TOTAL CELL WIDTH IN BIN ONE " << newCellWidth << endl;
	    cout << "BIN WIDTH " << binWidth <<  endl;
	    cout << "MODIFIED SUPPLY " << supply << endl;
	    cout << "CELL FRACT IN BIN ONE " << fract << endl;
	  }
	  fract = gamma2 / cellWidth;
	  newCellWidth = (*binTwo).BinGetTotalCellWidth() + gamma2;
	  binWidth = (*binTwo).BinGetWidth();
	  supply = newCellWidth - binWidth;
	  (*binTwo).BinSetTotalCellWidth(newCellWidth);
	  (*binTwo).BinSetSupply(supply);
	  (*binTwo).BinSetCellFract(cellObj, fract);
	  if (dPrint) {
	    cout << "NEW TOTAL CELL WIDTH IN BIN TWO " << newCellWidth << endl;
	    cout << "MODIFIED SUPPLY " << supply << endl;
	    cout << "CELL FRACT IN BIN TWO " << fract << endl;
	  }
	}
      }
      cellCount++;      
    } END_FOR;
    dPrint = false;
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
LegalizeAugPathAlgo(HyperGraph &myGraph, LegalizeBin *suppNode)
{

  if (debugMode) {
    cout << "###########################################################################" << endl;
    cout << "               ENTERING AUG PATH ALGORITHM                                " << endl;
    cout << "###########################################################################" << endl;
    cout << endl;
    PhysRow *currRow = (*suppNode).BinGetRow();
    cout << "SOURCE NODE IS " << (*currRow).PhysRowGetIndex() << " [" 
	 << (*suppNode).BinGetIndex() << "] " << endl;
    cout << " B(S) = " << (*suppNode).BinGetSupply() << endl;
    cout << endl;
  }
  
  /* Get A(s) = All the fractional cells in source bin suppNode */
  map<Cell*, double> AofS = (*suppNode).BinGetAllFractCells();

  /* Set the L-value of source node to 0 */
  LegalizeBinSetLValue(suppNode, 0.0);
  
  /* Set the b(v) of all other supply bins except the source bin to be 0 */
  void *NodeObj;
  uint NodeIdx;
  HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
    double supply = (*(LegalizeBin*)NodeObj).BinGetSupply();
    if ((LegalizeBin*)NodeObj == suppNode) {
      LegalizeBinSetBValue((LegalizeBin*)NodeObj, supply);
    } else {
      if (supply <= 0.0) {
	LegalizeBinSetBValue((LegalizeBin*)NodeObj, supply);
      } else {
	LegalizeBinSetBValue((LegalizeBin*)NodeObj, 0.0);
      }
    }
  } HYPERGRAPH_END_FOR;
  
# if 0 
  /* For debugging AofS and b(v) as present in the ALgorithm */
  if (0) {
    /* Print all fractional cells of source node */
    Cell* key;
    double val;
    cout << "CELLS IN SOURCE BIN " << endl;
    MAP_FOR_ALL_ELEMS(AofS, Cell*, double, key, val){
      cout << (*key).CellGetName() << " --> " << val << endl;
    } END_FOR;
  }
  
  if (0) {
    void *NodeObj;
    uint NodeIdx;
    LegalizeBin *presNode;
    PhysRow *presRow;
    int binIndex, rowIndex;
    double bValue;
    HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
      presNode = (LegalizeBin*)NodeObj;
      binIndex = (*presNode).BinGetIndex();
      presRow = (*presNode).BinGetRow();
      rowIndex = (*presRow).PhysRowGetIndex();
      bValue = LegalizeBinGetBValue(presNode);
      if (bValue > 0) {
	cout << "SOURCE BIN FOUND " << endl;
      }
      cout << rowIndex << " [" << binIndex << "] --> " << bValue << endl;
    } HYPERGRAPH_END_FOR;
  }
  
  if (0) {
    cout << " BEFORE UPDATING " << endl;
    void *NodeObj;
    uint NodeIdx;
    LegalizeBin *presNode;
    PhysRow *presRow;
    int binIndex, rowIndex;
    double lValue;
    HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
      presNode = (LegalizeBin*)NodeObj;
      binIndex = (*presNode).BinGetIndex();
      presRow = (*presNode).BinGetRow();
      rowIndex = (*presRow).PhysRowGetIndex();
      lValue = LegalizeBinGetLValue(presNode);
      if (lValue == DBL_MAX) {
	lValue = -1.0;
      }
      cout << rowIndex << " [" << binIndex << "] --> " << lValue << endl;
    } HYPERGRAPH_END_FOR;
  }

# endif
  
  /* For debugging */
  PhysRow *currRow;
  int currRowIdx;
  int currBinIdx;
  PhysRow *prevRow;
  int prevRowIdx;
  int prevBinIdx;
  /* End for debugging */

  LegalizeBin *currentNode;
  LegalizeBin *dest;
  map<LegalizeBin*, bool> usedNodes;
  map<LegalizeBin*,LegalizeBin*> pathMap;
  map<Cell*, double> cellsToMove;
  vector<pair<LegalizeBin* , double> > binsLValues;
  uint numEdges = myGraph.GetNumEdges();
  vector<map<Cell*, double> > chosenCells(numEdges);
  vector<double> totalFractWidths(numEdges);
  double fractWidthSelected;
  double currentNodeLVal;
  uint binCount = 0;
  uint iteration = 0;
  LegalizeBin *prevNode;

  /* Set destination node to Null */
  dest = NIL(LegalizeBin*);
  
  /* Set current node to source node */
  currentNode = suppNode;
  /* Get the L(v) of current node and push to the vector of L(v) */
  currentNodeLVal = LegalizeBinGetLValue(currentNode);
  binsLValues.push_back(make_pair(currentNode, currentNodeLVal));
  
  if (debugMode) {
    cout << "###################################################### " << endl;
    cout << "             ENTERING WHILE(1)                        " << endl;
    cout << "###################################################### " << endl;
    cout << endl;
  }
  
  while(1) {
    if (debugMode) {
      cout << "########### ITERATION " << iteration << " ######### " << endl;
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
      
      if (debugMode) {
	prevBinIdx = (*prevNode).BinGetIndex();
	prevRow = (*prevNode).BinGetRow();
	prevRowIdx = (*prevRow).PhysRowGetIndex();
	currRow = (*currentNode).BinGetRow();
	currRowIdx = (*currRow).PhysRowGetIndex();
	currBinIdx = (*currentNode).BinGetIndex();
	cout << "P( " << currRowIdx << "[" << currBinIdx << "]) = " << prevRowIdx << "[" 
	     << prevBinIdx << "]" << endl;
      }
      
      /* b(v) = b(v) + width(C(P(v),v)) */
      int edgeIdx = (int)myGraph.HyperGraphNodesAreAdjacent((void*)prevNode, (void*)currentNode);
      if (edgeIdx != -1) {
	fractWidthSelected = totalFractWidths[edgeIdx];
      }
      double modBValue = bValueCurrent + fractWidthSelected;
      LegalizeBinSetBValue(currentNode, modBValue);
      if (debugMode) {
	cout << "B(V) OF CURRENT NODE AFTER UPDATING " << LegalizeBinGetBValue(currentNode) << endl;
      }

      /* A(v) = GAMMA(v) U C(P(v),v)) */
      cellsToMove = chosenCells[edgeIdx];
      map<Cell*, double> AofV = (*currentNode).BinGetAllFractCells();
      Cell* Key;
      double Value;
      if (debugMode) {
	cout << "SIZE OF A(V) BEFORE UPDATING " << AofV.size() << endl;
      }
      MAP_FOR_ALL_ELEMS(cellsToMove, Cell*, double, Key, Value){
	_KEY_EXISTS(AofV, Key){
	  AofV[Key] = AofV[Key] + Value;
	  if (0) {
	    cout << " IN KEY EXISTS " << endl;
	  }
	} else {
	  AofV[Key] = Value;
	}
      } END_FOR;
      
      if (debugMode) {
	cout << "SIZE OF A(V) AFTER UPDATING " << AofV.size() << endl;
      }
	    
      binCount++;
    }

    /* If B(v) <= 0, Set t = v. Break */
    double currentBVal = LegalizeBinGetBValue(currentNode);
    if (currentBVal <= 0) {
      if (debugMode) {
	cout << " B(V) = " << currentBVal << " LESS THAN EQUAL ZERO FOR ITERATION " << iteration << endl;
      }
      dest = currentNode;
      break;
    }
      
    
    /* U := U\{v} */
    _KEY_EXISTS(usedNodes, currentNode) {
    } else {
      usedNodes[currentNode] = true;
    }
    if (debugMode) {
      LegalizeBin *key;
      bool value;
      cout << "NODES USED TILL THIS ITERATION " << endl;
      MAP_FOR_ALL_ELEMS(usedNodes, LegalizeBin*, bool, key, value) {
	currRow = (*key).BinGetRow();
	currRowIdx = (*currRow).PhysRowGetIndex();
	currBinIdx = (*key).BinGetIndex();
	cout << currRowIdx << "[" << currBinIdx << "]" << endl;
      } END_FOR;
    }
    
    NodeObj = (void*)currentNode;
    uint EdgeIdx;
    double weight;
      
    vector<LegalizeBin*> adjacentBins;
    int currentNodeY = (*currentNode).BinGetBot();
    int adjNodeY;
    double moveCost;
      
    map<Cell*, double> binCells = (*currentNode).BinGetAllFractCells();

    double totalFractWidth;
    double currentLVal, adjLVal, modAdjLVal;
    currentLVal = LegalizeBinGetLValue(currentNode);
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
	  if (0) {
	    cout << "DEBUGGING WHILE GETTING ALL CEllS TO MOVE" <<  endl;
	    cout << "EDGE-INDEX " << EdgeIdx << " NUM_CELLS " << cellsToMove.size() << endl;
	  }

	  chosenCells[EdgeIdx] = cellsToMove;
	  totalFractWidths[EdgeIdx] = totalFractWidth;

	} else {
	  moveCost = LegalizeGetCellsToMove((*currentNode), (*adjacentNode), binCells, false,
					    cellsToMove, totalFractWidth);
	  if (0) {
	    cout << "DEBUGGING WHILE GETTING ALL CEllS TO MOVE " <<  endl;
	    cout << "EDGE-INDEX " << EdgeIdx << " NUM_CELLS " << cellsToMove.size() << endl;
	  }

	  chosenCells[EdgeIdx] = cellsToMove;
	  totalFractWidths[EdgeIdx] = totalFractWidth;
	}
      
      
	if (0) {
	  cout << "NUMBER OF CELLS IN SOURCE BIN: " << binCells.size() << endl;
	  cout << "NUMBER OF CELLS TO MOVE: " << cellsToMove.size() << endl;
	  int currentBinIdx = (*currentNode).BinGetIndex();
	  int adjBinIdx = (*adjacentNode).BinGetIndex();
	  PhysRow *currentRow = (*currentNode).BinGetRow();
	  int currentRowIdx = (*currentRow).PhysRowGetIndex();
	  PhysRow *adjRow = (*adjacentNode).BinGetRow();
	  int adjRowIdx = (*adjRow).PhysRowGetIndex();
	  cout << "CELL TO BE MOVED FROM " << currentRowIdx << " [" << currentBinIdx
	       << "] TO " << adjRowIdx << " [" << adjBinIdx << "] ARE : " << endl;
	  Cell *key;
	  double val;
	
	  MAP_FOR_ALL_ELEMS(cellsToMove, Cell*, double, key, val) {
	    cout << (*key).CellGetName() << " --> " << val << endl;
	  } END_FOR;
	  cout << "MOVE COST " << moveCost << endl;
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
	}
	if (debugMode) {
	  PhysRow *adjRow = (*adjacentNode).BinGetRow();
	  int adjRowIdx = (*adjRow).PhysRowGetIndex();
	  int adjBinIdx = (*adjacentNode).BinGetIndex();
	  cout << "ADJACENT BIN " << adjRowIdx << "[" << adjBinIdx << "] L(V) = " << modAdjLVal << endl;
	}
      } 
    } HYPERGRAPH_END_FOR;

    if (0) {
      cout << " AFTER UPDATING " << endl;
      void *NodeObj;
      uint NodeIdx;
      LegalizeBin *presNode;
      PhysRow *presRow;
      int binIndex, rowIndex;
      double lValue;
      HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
	presNode = (LegalizeBin*)NodeObj;
	binIndex = (*presNode).BinGetIndex();
	presRow = (*presNode).BinGetRow();
	rowIndex = (*presRow).PhysRowGetIndex();
	lValue = LegalizeBinGetLValue(presNode);
	if (lValue == DBL_MAX) {
	  lValue = -1.0;
	}
	cout << rowIndex << " [" << binIndex << "] --> " << lValue << endl;
      } HYPERGRAPH_END_FOR;
    }

    
    /* Stop iterating if a node with no edges is found */
    if (!adjacentBins.size()) {
      break;
    }
      
    sort(binsLValues.begin(), binsLValues.end(), sortLValues);
    
    /* Choose bin with minimum L(v) for next iteration */
    pair<LegalizeBin*, double> binLValObj;
    VECTOR_FOR_ALL_ELEMS(binsLValues, pair<LegalizeBin* MCOMMA double>, binLValObj){
      _KEY_EXISTS(usedNodes,(binLValObj.first)) {
	continue;
      } else {
	currentNode = binLValObj.first;
	if (debugMode) {
	  PhysRow *adjRow = (*currentNode).BinGetRow();
	  int adjRowIdx = (*adjRow).PhysRowGetIndex();
	  int adjBinIdx = (*currentNode).BinGetIndex();
	  cout << "LEAST L(v) SELECTED " << adjRowIdx << "[" << adjBinIdx << "] L(V) = " 
	       << binLValObj.second << endl;
	  cout << endl;
	}
	break;
      }
    } END_FOR;
    
    if (0) {
      PhysRow *currRow = (*currentNode).BinGetRow();
      int currRowIdx = (*currRow).PhysRowGetIndex();
      int currBinIdx = (*currentNode).BinGetIndex();
      cout << " BIN WITH MIN L-VALUE " << endl;
      cout << currRowIdx << " [" << currBinIdx << "] --> " << LegalizeBinGetLValue(currentNode) << endl;
    }
    
    if (0) {
      LegalizeBin *key;
      LegalizeBin *value;
      cout << "PATH(S) SO FAR IS " << endl;
      MAP_FOR_ALL_ELEMS(pathMap, LegalizeBin*, LegalizeBin*, key, value) {
	PhysRow *destRow = (*key).BinGetRow();
	PhysRow *sourceRow = (*value).BinGetRow();
	int destRowIdx = (*destRow).PhysRowGetIndex();
	int sourceRowIdx = (*sourceRow).PhysRowGetIndex();
	int destBinIdx = (*key).BinGetIndex();
	int srcBinIdx = (*value).BinGetIndex();
	cout << sourceRowIdx << " [" << srcBinIdx << "] --> " << destRowIdx << " [" 
	     << destBinIdx << "] " << endl;
      } END_FOR;
    }
    iteration ++;
    /*
    if (iteration == 2) {
      break;
    }
    */
  }

  /* Found a demand bin */
  if (dest != NIL(LegalizeBin*)) {  
    if (debugMode) {
      cout << " IN STEP FOR MOVING CELLS " << endl;
    }
    LegalizeBin *currentBin; 
    LegalizeBin *prevBin;
    LegalizeBin *nextBin;
    vector<LegalizeBin*> reversePath;
    int binX, binY;
    bool exitWhile;
    PhysRow *currentRow;
    PhysRow *nextRow;

    currentBin = dest;
    reversePath.push_back(currentBin);
    prevBin = pathMap[currentBin];
    while (1) {
      currentBin = prevBin;
      reversePath.push_back(currentBin);
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
	  (*nextBin).BinAddCellToBin(thisCell);
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
    } while(currentBin != dest);
    
  } else {
    cout << "ERROR! ---> No destination node found" <<endl;
  }
  
  if (debugMode) {
    cout << "###########################################################################" << endl;
    cout << "               EXITING AUG PATH ALGORITHM                                " << endl;
    cout << "###########################################################################" << endl;
    cout << endl;
  }

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
		 vector<vector<LegalizeBin*> > &DesignBins)
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
  newPlot.PlotWriteOutput();
}

void
LegalizeDesign(Design &myDesign)
{
  vector<PhysRow*> allPhysRows;
  vector<vector<int> > allRowBounds;
  rowOrientation rowType;
  HyperGraph myGraph;
  string CellName;
  Cell* CellPtr;
  int rowBegin;
  int rowEnd;
  int columnWidth = 4000;
  PhysRow* Obj;				  
  int totNumCells = 0;
  vector<vector<Zone*> >DesignZones;
  vector<vector<LegalizeBin*> > DesignBins;
  int rowCount = 0;  
  vector<Cell*> cellsInRow;
  Cell* cellObj;

/* Get all the physical rows in Design */
  allPhysRows  = myDesign.DesignGetRows();
  
  /* Get bounding boxes for all subrows in all rows in Design */
  LegalizeGetAllBoundingBoxes(allPhysRows, allRowBounds);
  rowType=((allPhysRows[0])->PhysRowGetType());

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
  
  /*  Adding all cells to their respective rows */
  myDesign.DesignAddAllCellsToPhysRows();

  
  /* Get the zones of each row */
  rowBegin = ((allPhysRows[0])->PhysRowGetRowBegin());
  rowEnd = ((allPhysRows[0])->PhysRowCalculateRowEnd());
  
  if (0) {
    cout << "ROW\tBIN\tSUPPLY" << endl;
  }
  
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj){
    currentRow = rowCount;
    (*Obj).PhysRowGetCellsInRow(cellsInRow);
    
    
    /* Find Zones in the row and push all movable cells into free zone */
    vector<Zone*> zones;
    LegalizeFindZonesInRow(Obj, zones, rowBegin, rowEnd);
    DesignZones.push_back(zones);

    /*###########################################################################
      ################ SUBDIVIDE CHIP AREA INTO BINS ############################
      ###########################################################################
    */

     /* Find all the regions in the row */
    vector<LegalizeBin*> allBins; // denotes all bins in a specific row 
    LegalizeFindBinsInRow(zones, allBins, Obj, columnWidth, rowBegin, rowEnd);
    
    /* Merge bins if they satisfy the condition for merging */
    LegalizeMergeBins(allBins, columnWidth);

    /* Add Cell To respective regions */
    LegalizeAddCellsToBins(cellsInRow, allBins);
    
    /* Calculate the supplies of all regions */
    vector<LegalizeBin*> supplyBinsInRow;
    LegalizeCalcSuppForBins(allBins, supplyBinsInRow);
    
    /* Adding all the bins to the vector<vector> of DesignBins */
    DesignBins.push_back(allBins); 

    /*###########################################################################
      ############ DO FRACTIONAL ASSIGNMENT OF CELLS TO BINS ####################
      ###########################################################################
    */

    if (doFractReassign)    
      LegalizeDoFractReassign(supplyBinsInRow, allBins);
    
    /* Reassigning bins to the vector of supplyBins */
    /* Recalculating the supply of all the bins */
    supplyBinsInRow.clear();
    LegalizeCalcSuppForBins(allBins, supplyBinsInRow);
    supplyBins.insert(supplyBins.end(), supplyBinsInRow.begin(), supplyBinsInRow.end());

    vector<int> prevFactCells;
    LegalizeBin* binObj;
    int binCount = 0;
    VECTOR_FOR_ALL_ELEMS (allBins, LegalizeBin*, binObj){
      int supply = (*binObj).BinGetSupply();
      if (0) {
	cout << rowCount << "\t" << binCount << "\t" << supply << endl;
      }
      binCount++;
    } END_FOR;
    rowCount++;
  } END_FOR;
  
  /*###########################################################################
    ############ CONNECT ALL NEIGHBOURING BINS BY EDGES IN GRAPH ###############
    ###########################################################################
  */
  
  /* Add nodes to the graph */
  vector<LegalizeBin*> binsObj;
  /* Iterate over all the rows */
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsObj){
    /* Iterate over all bins in a row */
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
  
  uint rowNum = 0;

  /* Add edges to the graph */
  map<LegalizeBin*, uint> binsInGraph;
  VECTOR_FOR_ALL_ELEMS(DesignBins, vector<LegalizeBin*>, binsObj){
    /* Iterate over all bins in a row */
    LegalizeBin *thisBin;
    vector<void*> oneEdge;

    if (0) {
      cout << "ROW " << rowNum++ << "\t";
    }
    
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
      
	if (0) {
	  cout << "BIN " << (*thisBin).BinGetIndex() << endl;
	  cout << "ADJACENT BINS ARE : " << endl;
	  if (topBin != NIL(LegalizeBin*)) {
	    cout << "TOP BIN, ROW : " << rowNum << " BIN : " << (*topBin).BinGetIndex() << endl;
	  }
	  if (botBin != NIL(LegalizeBin*)) {
	    cout << "BOT BIN, ROW : " << (rowNum-2) << " BIN : " << (*botBin).BinGetIndex() << endl;
	  }
	  if (leftBin != NIL(LegalizeBin*)) {
	    cout << "LEFT BIN, ROW : " << (rowNum-1) << " BIN : " << (*leftBin).BinGetIndex() << endl;
	  }
	  if (rightBin != NIL(LegalizeBin*)) {
	    cout << "RIGHT BIN, ROW : " << (rowNum-1) << " BIN : " << (*rightBin).BinGetIndex() << endl;
	  }
	}

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
  
  if (0) {
    /* Test to check successfull addition of nodes in graph */
    void *NodeObj;
    uint NodeIdx;
    LegalizeBin *presNode;
    PhysRow *presRow;
    int binIndex, rowIndex;
    HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
      presNode = (LegalizeBin*)NodeObj;
      binIndex = (*presNode).BinGetIndex();
      presRow = (*presNode).BinGetRow();
      rowIndex = (*presRow).PhysRowGetIndex();
      cout << "ROW : " << rowIndex << " BIN : " << binIndex << endl;
    } HYPERGRAPH_END_FOR;
  }

  if (0) {
    /* Test to check successfull addition of edges in graph */
    void *NodeObj;
    uint NodeIdx;
    LegalizeBin *presNode;
    LegalizeBin *adjNode;
    PhysRow *presRow;
    PhysRow *adjRow;
    int binIndex, rowIndex;
    int adjBinIdx, adjRowIdx;
    HYPERGRAPH_FOR_ALL_NODES(myGraph, NodeIdx, NodeObj){
      presNode = (LegalizeBin*)NodeObj;
      binIndex = (*presNode).BinGetIndex();
      presRow = (*presNode).BinGetRow();
      rowIndex = (*presRow).PhysRowGetIndex();
      
      uint EdgeIdx;
      double Weight;
      HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(myGraph, NodeObj, EdgeIdx, Weight){
	adjNode = (LegalizeBin*)myGraph.GraphGetOtherNodeOfEdge(EdgeIdx, NodeObj);
	adjBinIdx = (*adjNode).BinGetIndex();
	adjRow = (*adjNode).BinGetRow();
	adjRowIdx = (*adjRow).PhysRowGetIndex();
	cout << rowIndex << " [" << binIndex << "] " << " -- " << adjRowIdx 
	     << " [" << adjBinIdx << "] " <<  endl;
      } HYPERGRAPH_END_FOR;
    } HYPERGRAPH_END_FOR;
  }


  vector<LegalizeBin*> supplyHeap = supplyBins;

  /* MAX HEAP FORMATION TEST BEGIN */
  vector<LegalizeBin*> testHeap = supplyHeap;
  if (0) {
    cout << "BEFORE HEAP FORMATION " << endl;
    int presBinIdx, presRowIdx;
    PhysRow *presRow;
    LegalizeBin *binObj;
    int presSupply;
    VECTOR_FOR_ALL_ELEMS(testHeap, LegalizeBin*, binObj){
      presBinIdx = (*binObj).BinGetIndex();
      presRow = (*binObj).BinGetRow();
      presRowIdx = (*presRow).PhysRowGetIndex();
      presSupply = (*binObj).BinGetSupply();
      cout << presRowIdx << " [" << presBinIdx << "] " << "-> "<< presSupply << endl;
    } END_FOR;
  }
  make_heap(testHeap.begin(), testHeap.end(), maxSupply);
  if (0) {
    cout << "AFTER HEAP FORMATION " <<  endl;
    int presBinIdx, presRowIdx;
    PhysRow *presRow;
    int presSupply;
    for (int count = 0; count < testHeap.size(); count++) {
      presBinIdx = (*testHeap.front()).BinGetIndex();
      presRow = (*testHeap.front()).BinGetRow();
      presRowIdx = (*presRow).PhysRowGetIndex();
      presSupply = (*testHeap.front()).BinGetSupply();
      cout << presRowIdx << " [" << presBinIdx << "] " << "-> "<< presSupply << endl;
      pop_heap(testHeap.begin(), testHeap.end(), maxSupply);
      testHeap.pop_back();
    }
  }
  /* MAX HEAP FORMATION TEST END */
  
  make_heap(supplyHeap.begin(), supplyHeap.end(), maxSupply);
  /* Get the first element = max element from the heap */
  LegalizeBin *maxSuppBin = supplyHeap.front();
  pop_heap(supplyHeap.begin(), supplyHeap.end(), maxSupply);
  supplyHeap.pop_back();
  while (supplyHeap.size()) {
    if (0) {
      cout << "SUPPLIES BEFORE AUG_PATH_ALGO " << endl;
      int presBinIdx, presRowIdx;
      PhysRow *presRow;
      int presSupply;
      LegalizeBin *thisBin;
      VECTOR_FOR_ALL_ELEMS(supplyBins, LegalizeBin*, thisBin){
	presBinIdx = (*thisBin).BinGetIndex();
	presRow = (*thisBin).BinGetRow();
	presRowIdx = (*presRow).PhysRowGetIndex();
	presSupply = (*thisBin).BinGetSupply();

	if (presSupply < 0) {
	  cout << "####################################################" << endl;
	  cout << "NEGATIVE SUPPLY VALUE !!!!!!!!!!!!!!!!!!!!! " << endl;
	  cout << endl;
	}

	cout << presRowIdx << " [" << presBinIdx << "] " << "-> "<< presSupply << endl;
      } END_FOR;
    }

    LegalizeAugPathAlgo(myGraph, maxSuppBin);
    LegalizeReCalcSuppAllBins(DesignBins, supplyHeap);

    
    if (debugMode) {
      cout << "SUPPLIES AFTER AUG_PATH_ALGO " << endl;
      int presBinIdx, presRowIdx;
      PhysRow *presRow;
      int presSupply;
      LegalizeBin *thisBin;
      VECTOR_FOR_ALL_ELEMS(supplyBins, LegalizeBin*, thisBin){
	presBinIdx = (*thisBin).BinGetIndex();
	presRow = (*thisBin).BinGetRow();
	presRowIdx = (*presRow).PhysRowGetIndex();
	presSupply = (*thisBin).BinGetSupply();
	cout << presRowIdx << " [" << presBinIdx << "] " << "-> "<< presSupply << endl;
      } END_FOR;
    }

    make_heap(supplyHeap.begin(), supplyHeap.end(), maxSupply);
    maxSuppBin = supplyHeap.front();
    pop_heap(supplyHeap.begin(), supplyHeap.end(), maxSupply);
    supplyHeap.pop_back();
    LegalizeAssignLValues(DesignBins);
    
    break;
  }
  
  /* Legalize Placement in a Zone */
  
  LegalizePlotData("supply and demand bins", "supplytest.plt", myDesign, DesignBins);
}

/*###########################################################################################
  ################################### END OF CODE ###########################################
  ###########################################################################################
*/


















# if 0

void
LegalizeDoFractReassign(vector<LegalizeBin*> &allBins, 
			vector<LegalizeBin*> &supplyBins, 
			vector<Cell*> &cellsInRow)
{
  LegalizeBin* binObj;
  VECTOR_FOR_ALL_ELEMS (supplyBins, LegalizeBin*, binObj){
    int binIndex = binObj->BinGetIndex();
    bool isEmpty = binObj->BinGetEmpty();
    bool boundLeft = binObj->BinGetLBound();
    bool boundRight = binObj->BinGetRBound();
    vector<Cell*> cellsInBin = binObj->BinGetCellsInBin();
    
    if (boundLeft && !boundRight) {
      /* Check only the next bin on right for a possible demand bin */
	int rightBinIndex = binIndex + 1;
	LegalizeBin* rightBin = allBins[rightBinIndex];
	int rBSupply = rightBin->BinGetSupply();
	if (rBSupply < 0) {
	  /* Found a neighbouring demand bin */
	  int rBinBegin = rightBin->BinGetBegin();
	  int rBinEnd = rightBin->BinGetEnd();
	  Cell* cellObj;
	  VECTOR_FOR_ALL_ELEMS (cellsInBin, Cell*, cellObj) {
	    int cellBegin = cellObj->CellGetXpos();
	    int cellWidth = cellObj->CellGetWidth();
	    int cellEnd = cellBegin + cellWidth;
	    double overlapWidth = LegalizeThetaFunc(rBinBegin, rBinEnd, cellBegin, cellEnd);
	    double fract = overlapWidth/cellWidth;
	    if (fract > 0.0) {
	      double fractThisBin = 1.0 - fract;
	      /* Modify the fraction of cell in the current bin */
	      binObj->BinSetCellFraction(cellObj, fractThisBin);
	      int thisBinSupply = binObj->BinGetSupply();
	      int modSupply = thisBinSupply - overlapWidth;
	      binObj->BinSetSupply(modSupply);
	      /* Add the other fraction to the neighbouring demand bin */
	      rightBin->BinSetCellFraction(cellObj, fract);
	      int rightBinSupply = rightBin->BinGetSupply();
	      modSupply = rightBinSupply + overlapWidth;
	      rightBin->BinSetSupply(modSupply);
	    }
	  } END_FOR;
	}
      } else if (!boundLeft && boundRight) {
	/* Check only the next bin on left for a possible demand bin */
	int leftBinIndex = binIndex - 1;
	LegalizeBin* leftBin = allBins[leftBinIndex];
	int lBSupply = leftBin->BinGetSupply();
	if (lBSupply < 0) {
	  /* Found a neighbouring demand bin */
	  int lBinBegin = leftBin->BinGetBegin();
	  int lBinEnd = leftBin->BinGetEnd();
	  Cell* cellObj;
	  VECTOR_FOR_ALL_ELEMS (cellsInBin, Cell*, cellObj) {
	    int cellBegin = cellObj->CellGetXpos();
	    int cellWidth = cellObj->CellGetWidth();
	    int cellEnd = cellBegin + cellWidth;
	    double overlapWidth = LegalizeThetaFunc(lBinBegin, lBinEnd, cellBegin, cellEnd);
	    double fract = overlapWidth/cellWidth;
	    if (fract > 0.0) {
	      double fractThisBin = 1.0 - fract;
	      /* Modify the fraction of cell in the current bin */
	      binObj->BinSetCellFraction(cellObj, fractThisBin);
	      int thisBinSupply = binObj->BinGetSupply();
	      int modSupply = thisBinSupply - overlapWidth;
	      binObj->BinSetSupply(modSupply);
	      /* Add the other fraction to the neighbouring demand bin */
	      leftBin->BinSetCellFraction(cellObj, fract);
	      int leftBinSupply = leftBin->BinGetSupply();
	      modSupply = leftBinSupply + overlapWidth;
	      leftBin->BinSetSupply(modSupply);
	    }
	  } END_FOR;
	}
      } else if (!boundLeft && !boundRight) {
	/* Check both left and right bins for a possible demand bin */
	int leftBinIndex = binIndex - 1;
	int rightBinIndex = binIndex + 1;
	LegalizeBin* leftBin = allBins[leftBinIndex];
	LegalizeBin* rightBin = allBins[rightBinIndex];
	int lBSupply = leftBin->BinGetSupply();
	int rBSupply = rightBin->BinGetSupply();
	if (rBSupply < lBSupply) {
	  if (rBSupply < 0) {
	    /* Found a neighbouring demand bin */
	    int rBinBegin = rightBin->BinGetBegin();
	    int rBinEnd = rightBin->BinGetEnd();
	    Cell* cellObj;
	    VECTOR_FOR_ALL_ELEMS (cellsInBin, Cell*, cellObj) {
	      int cellBegin = cellObj->CellGetXpos();
	      int cellWidth = cellObj->CellGetWidth();
	      int cellEnd = cellBegin + cellWidth;
	      double overlapWidth = LegalizeThetaFunc(rBinBegin, rBinEnd, cellBegin, cellEnd);
	      double fract = overlapWidth/cellWidth;
	      if (fract > 0.0) {
		double fractThisBin = 1.0 - fract;
		/* Modify the fraction of cell in the current bin */
		binObj->BinSetCellFraction(cellObj, fractThisBin);
		int thisBinSupply = binObj->BinGetSupply();
		int modSupply = thisBinSupply - overlapWidth;
		binObj->BinSetSupply(modSupply);
		/* Add the other fraction to the neighbouring demand bin */
		rightBin->BinSetCellFraction(cellObj, fract);
		int rightBinSupply = rightBin->BinGetSupply();
		modSupply = rightBinSupply + overlapWidth;
		rightBin->BinSetSupply(modSupply);
	      }
	    } END_FOR;
	  }
	} else if (rBSupply >= lBSupply) {
	  if (lBSupply < 0) {
	    /* Found a neighbouring demand bin */
	    int lBinBegin = leftBin->BinGetBegin();
	    int lBinEnd = leftBin->BinGetEnd();
	    Cell* cellObj;
	    VECTOR_FOR_ALL_ELEMS (cellsInBin, Cell*, cellObj) {
	      int cellBegin = cellObj->CellGetXpos();
	      int cellWidth = cellObj->CellGetWidth();
	      int cellEnd = cellBegin + cellEnd;
	      double overlapWidth = LegalizeThetaFunc(lBinBegin, lBinEnd, cellBegin, cellEnd);
	      double fract = overlapWidth/cellWidth;
	      if (fract > 0.0) {
		double fractThisBin = 1.0 - fract;
		/* Modify the fraction of cell in the current bin */
		binObj->BinSetCellFraction(cellObj, fractThisBin);
		int thisBinSupply = binObj->BinGetSupply();
		int modSupply = thisBinSupply - overlapWidth;
		binObj->BinSetSupply(modSupply);
		/* Add the other fraction to the neighbouring demand bin */
		leftBin->BinSetCellFraction(cellObj, fract);
		int leftBinSupply = leftBin->BinGetSupply();
		modSupply = leftBinSupply + overlapWidth;
		leftBin->BinSetSupply(modSupply);
	      }
	    } END_FOR;
	  }
	}
      }
    }
  } END_FOR;
}

# endif

# if 0

    binIdx = binObjn->BinGetIndex();
    binIdxPlusOne = binIdx + 1;
    if (binIdxPlusOne <= lastIdx) {
      rightBin = allBins[binIdxPlusOne];
      rBinSupp = rightBin->BinGetSupply();
      /* If combo is not set and right bin not a demand bin */
      /* Go to the next iteration of the loop */
      if ((rBinSupp > 0) && !combo){
	continue;
      } else if ((rBinSupp > 0) && combo){
	/* If right bin not a demand bin but combo is set */
	/* Do fractional reassignment of cells to the supply bin */
	vector<Cell*> cellsInBin = binObj->BinGetCellsInBin();
	Cell* thisCell;
	double overlap;
	VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
	  int cellBegin = thisCell->CellGetXpos();
	  int cellWidth = thisCell->CellGetWidth();
	  int cellEnd = cellBegin + cellWidth;
	  overlap = LegalizeThetaFunc(rBinBegin, rBinEnd, cellBegin, cellEnd);
	  if (overlap > 0){
	    double fractOverlap = overlap / cellWidth;
	    rightBin->BinSetCellFract(thisCell, fractOverlap);
	    binObj->Bin
	  } 
	} END_FOR;
      } else if (rBinSupp <= 0) {
	/* Found a neighbouring demand bin, so irrespective of value of combo do,*/
	/* Iterate over all cells in the supply bin and find overlap */
        vector<Cell*> cellsInBin = binObj->BinGetCellsInBin();
	Cell* thisCell;
	double overlap;
	VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
	  int cellBegin = thisCell->CellGetXpos();
	  int cellWidth = thisCell->CellGetWidth();
	  int cellEnd = cellBegin + cellWidth;
	  overlap = LegalizeThetaFunc(rBinBegin, rBinEnd, cellBegin, cellEnd);
	  if (overlap > 0){
	    double fractOverlap = overlap / cellWidth;
	    rightBin->BinSetCellFract(thisCell, fractOverlap);
	  } 
	} END_FOR;
      }
      binIdxPlusTwo = binIdx + 2;
      if (binIdxPlusTwo <= lastIdx) {
	rightMostBin = allBins[binIdxPlusTwo];
	rMBinSupply = rightMostBin->BinGetSupply();
	rMBinBegin = rightMostBin->BinGetBegin();
	rMBinEnd = rightMostBin->BinGetEnd();
	if((rMBinSupply > 0) && !combo){
	  continue;
	} else if ((rMBinSupply > 0) && combo){
	  /* If right-most bin not a demand bin but combo is set */
	  /* Do fractional reassignment of cells to the supply bin */
	  vector<Cell*> cellsInBin = binObj->BinGetCellsInBin();
	  Cell* thisCell;
	  double overlap;
	  VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
	    int cellBegin = thisCell->CellGetXpos();
	    int cellWidth = thisCell->CellGetWidth();
	    int cellEnd = cellBegin + cellWidth;
	    overlap = LegalizeThetaFunc(rMBinBegin, rMBinEnd, cellBegin, cellEnd);
	    if (overlap > 0){
	      double fractOverlap = overlap / cellWidth;
	      rightMostBin->BinSetCellFract(thisCell, fractOverlap);
	    } 
	  } END_FOR;
	} else if (rMBinSupp <= 0) {
	  /* Found a neighbouring demand bin, so irrespective of value of combo do,*/
	  /* Iterate over all cells in the supply bin and find overlap */
	  vector<Cell*> cellsInBin = binObj->BinGetCellsInBin();
	  Cell* thisCell;
	  double overlap;
	  VECTOR_FOR_ALL_ELEMS(cellsInBin, Cell*, thisCell){
	    int cellBegin = thisCell->CellGetXpos();
	    int cellWidth = thisCell->CellGetWidth();
	    int cellEnd = cellBegin + cellWidth;
	    overlap = LegalizeThetaFunc(rMBinBegin, rMBinEnd, cellBegin, cellEnd);
	    if (overlap > 0){
	      double fractOverlap = overlap / cellWidth;
	      rightMostBin->BinSetCellFract(thisCell, fractOverlap);
	    } 
	  } END_FOR;
	}
      }
    }

# endif
# if 0    
    Zone* zObj;
    cout << "zones for row " << i <<" are " << endl;
    VECTOR_FOR_ALL_ELEMS (zones, Zone*, zObj){
      cout << "(" << (zObj->ZoneGetBegin()) << "," << (zObj->ZoneGetEnd()) << ")" << (zObj->ZoneGetEmpty()? "EMPTY" : "BLOCKED") << e    } END_FOR;

# endif
