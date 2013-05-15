# include <Legalize.h>

using namespace lemon;

# define INFEASIBLE 0
# define OPTIMAL 1
# define UNBOUNDED 2
# define DEFAULT_DEMAND 50
# define ArcMap ListDigraph::ArcMap
# define NodeMap ListDigraph::NodeMap

typedef ListDigraph::NodeIt NodeIt;
typedef ListDigraph::ArcIt ArcIt;

bool ascendingX(Cell*, Cell*);
void LegalizeFormIntervals(vector<Region*> &, vector<Interval*> &);



void
LegalizeCalculateMinCostFlow(ListDigraph &g, ArcMap<int> &capacities, NodeMap<int> &supply)
{
  int retVal;
  NetworkSimplex<ListDigraph> ns(g);
  retVal = ns.supplyType(lemon::NetworkSimplex<lemon::ListDigraph>::LEQ).upperMap(capacities).supplyMap(supply).run();
  if (retVal == INFEASIBLE) {
    cout << "Infeasible" << endl;
  } else if (retVal == OPTIMAL) {
    cout << "optimal" << endl;
  } else if (retVal == UNBOUNDED) {
    cout << "UNBOUNDED" << endl;    
  }
  /* Create a map to copy the results of the solver */
  ArcMap<int> flows(g);  
  ns.flowMap(flows);
  cout<<"Arc : Flow"<<endl;
  for (ArcIt i(g); i != INVALID; ++i){
    if(flows[i]){
      cout << " (" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
      cout <<" : "<<flows[i]<<endl;
    }
  }
  /* Maybe later 
  ListDigraph::Node s;
  s = ListDigraph::nodeFromId(1);
  Bfs<ListDigraph> bfs(g);
  bfs.run(s);
  for (NodeIt n(g); n != INVALID; ++n) {
    if (bfs.reached(n)) {
      cout << g.id(n);
      ListDigraphBase::Node prev = bfs.predNode(n);
      while (prev != INVALID) {
	cout << "<-" << g.id(prev);
	prev = bfs.predNode(n);
      }    
      cout << endl;
    }
  }
  */
  cout<<"total cost = "<<(ns.totalCost<int>())<<endl;
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

bool
ascendingX(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetXpos()) < (cell2->CellGetXpos()));
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
  Zone* myZone;
  Zone* prevZone;
  /* Sort fixed cells in ascending order of x-pos */
 
  if(fixedCells.size()){
    sort(fixedCells.begin(),fixedCells.end(),ascendingX);
    /* Iterate over all fixed cells to find the zones */
    VECTOR_FOR_ALL_ELEMS(fixedCells, Cell*, Obj){
      int xPos = ((*Obj).CellGetXpos());
      if(firstBlockedZone){
	if (xPos == rowBegin){
	  myZone = new Zone(xPos, false);
	  prevZone = myZone;
	  zones.push_back(myZone);
	  blockedZones.push_back(myZone);
	  blockedZoneEnd = xPos + ((*Obj).CellGetWidth());
	}
	else if(xPos > rowBegin){
	  myZone = new Zone(rowBegin, true);
	  zones.push_back(myZone);
	  myZone->ZoneSetEnd(xPos);
	  myZone = new Zone(xPos, false);
	  zones.push_back(myZone); 
	  blockedZones.push_back(myZone);
	  prevZone = myZone;
	  blockedZoneEnd = xPos + ((*Obj).CellGetWidth());
	}
	firstBlockedZone = false;
      }
      else{
	if(xPos == rowBegin){
	  if((xPos + ((*Obj).CellGetWidth())) > prevBlockedZoneEnd){
	    blockedZoneEnd = (xPos + ((*Obj).CellGetWidth()));
	  }
	}
	else if(xPos <= prevBlockedZoneEnd){
	  if((xPos + ((*Obj).CellGetWidth())) > prevBlockedZoneEnd){
	    blockedZoneEnd = (xPos + ((*Obj).CellGetWidth()));
	  }
	}
	else if(xPos > prevBlockedZoneEnd){
	  myZone = new Zone(prevBlockedZoneEnd, true);
	  zones.push_back(myZone);
	  prevZone->ZoneSetEnd(prevBlockedZoneEnd);
	  myZone = new Zone(xPos, false);
	  prevZone = myZone;
	  zones.push_back(myZone);
	  blockedZones.push_back(myZone);
	  blockedZoneEnd = (xPos + ((*Obj).CellGetWidth()));
	}
      }
      prevBlockedZoneEnd = blockedZoneEnd;
    }END_FOR;
    if(prevBlockedZoneEnd != rowEnd){
      prevZone->ZoneSetEnd(prevBlockedZoneEnd);
      myZone = new Zone(prevBlockedZoneEnd, true);
      myZone->ZoneSetEnd(rowEnd);
      zones.push_back(myZone);
    }
  }
  /* If row does not have any cell(s) that qualfies as fixed */
  if (!(fixedCells.size())){
    myZone = new Zone(rowBegin, true);
    myZone->ZoneSetEnd(rowEnd);
    zones.push_back(myZone);
  }
  
  unsigned int zoneEnd = zones.size();
  if (zoneEnd > 1){
    zones[0]->ZoneSetLRBound(0);
    zones[(zoneEnd-1)]->ZoneSetLRBound(1);
    VECTOR_FOR_ALL_ELEMS(movableCells, Cell*, Obj){
      int cellBegin = Obj->CellGetXpos();
      int cellEnd = cellBegin + (Obj->CellGetWidth());
      int cellMid = (cellBegin + cellEnd)/2;
      Zone* prevZone;
      Zone* zoneObj;
      /* If cell is inside a blocked zone push it to an empty zone */
      VECTOR_FOR_ALL_ELEMS(zones, Zone*, zoneObj){
	if ((cellMid >= (zoneObj->ZoneGetBegin())) && (cellMid < (zoneObj->ZoneGetEnd()))){
	  if (!(zoneObj->ZoneGetEmpty())){
	    int newXpos;
	    if ((zoneObj->ZoneGetLRBound()) == 0){
	      newXpos = cellBegin + ((zoneObj->ZoneGetEnd()) - cellMid);
	      Obj->CellSetXpos(newXpos);
	    } else if((zoneObj->ZoneGetLRBound()) == 1){
	      newXpos = cellBegin - (cellMid - (zoneObj->ZoneGetBegin()));
	      Obj->CellSetXpos(newXpos);
	    } else if((zoneObj->ZoneGetLRBound()) == 2){
	      int leftDist = cellMid - (zoneObj->ZoneGetBegin());
	      int rightDist = (zoneObj->ZoneGetEnd()) - cellMid;
	      if (leftDist > rightDist){
		newXpos = cellBegin + rightDist;
	      } else{
		newXpos = cellBegin - leftDist;
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

bool
ascendingRegions(pair<int, unsigned int> p1, pair<int, unsigned int> p2)
{
  return((p1.first) < (p2.first));
}

void
LegalizeFindRegionsInRow(vector<Zone*> &allZones, 
			 vector<Region*> &allRegions, PhysRow* &myRow,
			 int columnWidth, unsigned int rowBegin, unsigned int rowEnd)
{
  /* Finding the column boundaries */
  vector<pair<int, unsigned int> > regions;
  for (int i = rowBegin; i < rowEnd; i += columnWidth){
    regions.push_back(make_pair(i,2));
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
     to give regions */  
  regions.insert(regions.end(), zones.begin(), zones.end());
  sort(regions.begin(),regions.end(), ascendingRegions);
  
  pair<int, unsigned int> Obj;
  map<int, unsigned int> zoneMap;
  VECTOR_FOR_ALL_ELEMS(zones, pair<int MCOMMA unsigned int>, Obj){
    zoneMap.insert(Obj);
  }END_FOR;
  
  pair<int, unsigned int> lastZone = *(zoneMap.begin());
  for (int idx = 0; idx < regions.size(); idx++) {
    pair<int, unsigned int> &Obj = regions[idx];
    if (Obj.second == 2){
      _KEY_EXISTS(zoneMap, Obj.first) {
	lastZone = make_pair(Obj.first, zoneMap[Obj.first]);
	regions.erase(remove(regions.begin(), regions.end(), Obj), regions.end());
	idx--;
      } else {
	Obj.second = lastZone.second;
      }
    } else {
      lastZone = Obj;
    }
    
  }
  /* Insert region objects into a vector */
  Region* region;
  Region* prevRegion;
  bool firstRegion = true;

  VECTOR_FOR_ALL_ELEMS(regions, pair<int MCOMMA unsigned int>, Obj){
    if (firstRegion){
      region = new Region(Obj.first, myRow, Obj.second);
      /* The first region is always bounded on the left */
      region->RegionSetLBound(true);
      
      allRegions.push_back(region);
      firstRegion = false;
    } else{
      prevRegion->RegionSetEnd(Obj.first);
      prevRegion->RegionCalculateWidth();
      region = new Region(Obj.first, myRow, Obj.second);
      
      /* If previous region was blocked, set LBound on the present region */
      if (!(prevRegion->RegionGetEmpty())){
	if (region->RegionGetEmpty()){
	  region->RegionSetLBound(true);
	}
      }
      
      /* If the previous region was empty and present region is blocked,
	 Set RBound on the previous region */
      else if (prevRegion->RegionGetEmpty()){
	if (!(region->RegionGetEmpty())){
	  prevRegion->RegionSetRBound(true);
	}
      }	
      allRegions.push_back(region);
    }
    prevRegion = region;
  }END_FOR;
  prevRegion->RegionSetEnd(rowEnd);
  prevRegion->RegionCalculateWidth();
  /* The last region is always bounded on the right */
  prevRegion->RegionSetRBound(true);
  
}


Region*
LegalizeFindPorNRegion(vector<Region*> &allRegions, Region* region, bool prev)
{
  Region* Obj;
  if (prev){
    VECTOR_FOR_ALL_ELEMS(allRegions, Region*, Obj){
      if ((Obj->RegionGetEnd()) == (region->RegionGetBegin())){
	return Obj;
	break;
      }
    }END_FOR;
  } else{
      VECTOR_FOR_ALL_ELEMS(allRegions, Region*, Obj){
	if ((Obj->RegionGetBegin()) == (region->RegionGetEnd())){
	  return Obj;
	  break;
	}
      }END_FOR;
  }
}

void
LegalizeAddCellsToRegions(vector<Cell*> &cellsInRow, vector<Region*> &allRegions)
{
  int cellBegin, cellEnd, cellMid;
  Cell* cellObj;
  Region* regionObj;
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
    if (!CellIsFixed(cellObj)){
      cellBegin = cellObj->CellGetXpos();
      cellEnd = cellBegin + (cellObj->CellGetWidth());
      cellMid = cellBegin + cellEnd;
      VECTOR_FOR_ALL_ELEMS(allRegions, Region*, regionObj){
	if (regionObj->RegionGetEmpty()){
	  if ((cellMid >= (regionObj->RegionGetBegin())) && (cellMid < (regionObj->RegionGetEnd()))){
	    regionObj->RegionAddCellToRegion(cellObj);
	  }
	}
      }END_FOR;
    }
  }END_FOR;
}

void
LegalizeCalculateSupplyForRegions(vector<Region*> &allRegions)
{
  Region* region;
  VECTOR_FOR_ALL_ELEMS(allRegions, Region*, region){
    if (region->RegionGetEmpty()){
      int wCells = region->RegionGetTotalCellWidth();
      int wRegion = region->RegionGetRegionWidth();
      bool LBound = region->RegionGetLBound();
      bool RBound = region->RegionGetRBound();
      int supply = wCells - wRegion;
      int fCellWidth;
      int lCellWidth;
      bool supplyFound = false;
  
      /* Indicates a possible supply region */
      if (supply > 0){
	fCellWidth = region->RegionGetFCellWidth();
	lCellWidth = region->RegionGetLCellWidth();
   
	if ((LBound) && !(RBound)){            // Bound on the left
	  supply -= ((0.5) * lCellWidth);
	} else if (!(LBound) && (RBound)){     // Bound on the right
	  supply -= ((0.5) * fCellWidth);
	} else if (!(LBound) && !(RBound)){    // Not Bound in any direction
	  supply -= ((0.5) * lCellWidth);
	  supply -= ((0.5) * fCellWidth);
	  // If bound in both direction no extra calculation required for supply
	}
	supplyFound = true;
      } else if (supply <= 0){
	/* Indicates a possible demand region */
	if ((LBound) && !(RBound)){
	  // Bound on the left, so find the first cell width of next region
	  Region* next = LegalizeFindPorNRegion(allRegions, region, false);
	  fCellWidth = next->RegionGetFCellWidth(); 
	  supply += ((0.5) * fCellWidth);
	} else if (!(LBound) && (RBound)){
	  // Bound on the right, so find the last cell width of previous region
	  Region* prev = LegalizeFindPorNRegion(allRegions, region, true);
	  lCellWidth = prev->RegionGetLCellWidth();
	  supply += ((0.5) * lCellWidth);
	} else if (!(LBound) && !(RBound)){
	  /* Not bound on either side so, find the width of first cell of next
	     region and the last cell of the previous region */
	  Region* next = LegalizeFindPorNRegion(allRegions, region, false);
	  fCellWidth = next->RegionGetFCellWidth();
	  Region* prev = LegalizeFindPorNRegion(allRegions, region, true);
	  lCellWidth = prev->RegionGetLCellWidth();
	  supply += ((0.5) * lCellWidth);
	  supply += ((0.5) * fCellWidth);
	}
      }
      // For a supposed supply node gives the value of max(supply, 0)
      if((supplyFound) && (supply < 0)){
	region->RegionSetSupply(0);
      } else if ((supplyFound) && (supply >= 0)){
	region->RegionSetSupply(supply);
      } else if((!supplyFound) && (supply < 0)){
	// For a supposed demand node gives the value of min(supply,0)
	region->RegionSetSupply(supply);
      } else if((!supplyFound) && (supply >= 0)){
	region->RegionSetSupply(0);
      }
    }
  }END_FOR;
}

void
LegalizeGetAllIntervals(vector<Region*> &allRegions, vector<Interval*> &allIntervals)
{
  Region* regionObj;
  Region* lastRegion;
  vector<Region*> formInt;
  bool firstRegion = true;
  VECTOR_FOR_ALL_ELEMS(allRegions, Region*, regionObj){
    if (firstRegion){
      /* The first region in the row is an empty region */
      if (regionObj->RegionGetEmpty()){
	formInt.push_back(regionObj);
      } 
      /* If the first region is blocked, nothing is done */
      firstRegion = false;
    } else{
      if (!(regionObj->RegionGetEmpty()) && (formInt.size() > 1)){
	/* Found a blocked region and a set of consecutive unblocked regions found to form interval */
	LegalizeFormIntervals(formInt, allIntervals);
	formInt.clear();
      } else if (!(regionObj->RegionGetEmpty()) && (formInt.size() == 1)){
	/* Found a block region but the set of unblocked regions contain only one region
	   So empty the vector */
	formInt.clear();
      } else if (regionObj->RegionGetEmpty()){
	/* Found an empty region, store it for a possible interval formation */
	formInt.push_back(regionObj);
      }
      lastRegion = regionObj;
    }
  }END_FOR;
  /* If atleast the last two regions were empty form an interval */
  if ((lastRegion->RegionGetEmpty()) && (formInt.size() > 1)){
    LegalizeFormIntervals(formInt, allIntervals);
    formInt.clear();
  }
}

void
LegalizeFormIntervals(vector<Region*> &formInt, vector<Interval*> &allIntervals)
{
  Interval* interval;
  int endIdx = formInt.size() - 1;
  for(int idx = 0; idx < endIdx; idx++){
    interval = new Interval();
    interval->IntervalAddRegion(formInt[idx]);
   
    int begin = formInt[idx]->RegionGetBegin();
    interval->IntervalSetBegin(begin);
    for(int j = idx + 1; j <= endIdx; j++){
      interval->IntervalAddRegion(formInt[j]);
      int end = formInt[j]->RegionGetEnd();
      interval->IntervalSetEnd(end);
      interval->IntervalSetLBound();
      interval->IntervalSetRBound();
      interval->IntervalCalculateAndSetSupply(formInt);
      if (interval->IntervalGetSupply()){
        Interval *toAdd = new Interval();
	*toAdd = *interval;
	allIntervals.push_back(toAdd);
      }
    }
  }
}


# if 0
void
LegalizeConstructGNFGraph(vector<PhysRow*> &allPhysRows, ListDigraph &g, 
			  ArcMap<int> &capacities, NodeMap<int> &supply)
{
  /* Used to define infinity for capacity values
 
  int MAX = std::numeric_limits<int>::max();
  cout<<"MAX "<<MAX<<endl;
  int INFI = std::numeric_limits<int>::has_infinity ?
    std::numeric_limits<int>::infinity() : MAX;
  cout<<"INFI "<<INFI<<endl;
  */
  
  vector<ListDigraph::Node> n;
  vector<ListDigraph::Arc> a;
  int numRows = (allPhysRows.size());
    
  
  /* Adding a virtual source and a sink node */
  n.push_back(g.addNode()); 
  n.push_back(g.addNode());
  
  /* Add the region nodes to the diGraph */
  
  unsigned int nodeIndex = 2;
  unsigned int arcIndex = 0;
  PhysRow* Obj;

  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
            
    /* Supply rows */
    if(wsViolation < 0){
      //Adding a supply node and its arc to the source
      n.push_back(g.addNode()); 
      a.push_back(g.addArc(n[0],n[nodeIndex]));
      nodeIndex++;
      capacities[a[arcIndex]] = abs(wsViolation);
      arcIndex++;
      rowsWithExcessCells++;
      totExcessCellWidth += abs(wsViolation);
      /* Adding arcs between teo rows */
      if(!firstRowNode){
	a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-1)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
	a.push_back(g.addArc(n[(nodeIndex-1)],n[(nodeIndex-2)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
      }
      else
	firstRowNode = false;
    }
    if(wsViolation > 0){
      //Adding a demand node and its arc to the sink
      n.push_back(g.addNode());
      a.push_back(g.addArc(n[nodeIndex],n[1]));
      nodeIndex++;
      capacities[a[arcIndex]] = wsViolation;
      arcIndex++;
      rowsWithWhiteSpaces++;
      totWSWidth += wsViolation;
      if(!firstRowNode){
	a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-1)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
	a.push_back(g.addArc(n[(nodeIndex-1)],n[(nodeIndex-2)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
      }
      else
	firstRowNode = false;
    }
    
  }END_FOR;
  
  /* Adding supply values to the virtual source and sink nodes */

  supply[n[0]] = totExcessCellWidth;
  //supply[n[0]] = 10000;
  //supply[n[1]] = -totWSWidth;
  supply[n[1]] = -totExcessCellWidth;
  
  cout << "DBG: Total supply :" << totExcessCellWidth << endl;
  cout << "DBG: Total demand :" << -totExcessCellWidth << endl;

  cout << "Node : Supply/Demand"<<endl;
  for (NodeIt i(g); i != INVALID; ++i)
    cout << g.id(i) << " : "<<supply[i]<<endl;
   
  cout<<"Arc : Capacity : Cost"<<endl;
  for (ArcIt i(g); i != INVALID; ++i){
    cout << "(" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
    cout <<" : "<<capacities[i]<<" : 1"<<endl;
  }
  
  cout<<"Total number of Physical Rows in design : "<<numRows<<endl;
  cout<<"Total rows with excess of WS : "<<rowsWithWhiteSpaces<<endl;
  cout<<"Total rows with lack of WS : "<<rowsWithExcessCells<<endl;
  cout<<"Total number of Nodes Created :  "<<countNodes(g)<<endl;
  cout<<"Total number of Arcs Created : "<<countArcs(g)<<endl;
}

# endif

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
  
  int columnWidth = 4000;
  PhysRow* Obj;				  
  
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    
    /* Mark the fixed cells in the row */
    Obj->PhysRowMarkFixedCellsInRow(columnWidth);
    vector<Cell*> cellsInRow;
    Obj->PhysRowGetCellsInRow(cellsInRow);
    
    /* Find Zones in the row and push all movable cells into free zone */
    vector<Zone*> zones;
    LegalizeFindZonesInRow(Obj, zones, rowBegin, rowEnd);
    
     /* Find all the regions in the row */
    vector<Region*> allRegions;
    LegalizeFindRegionsInRow(zones, allRegions, Obj, columnWidth, rowBegin, rowEnd);
    
    /* Add Cell To respective regions */
    LegalizeAddCellsToRegions(cellsInRow, allRegions);
    
    /* Calculate the supplies of all regions */
    LegalizeCalculateSupplyForRegions(allRegions);

    /* Form intervals from unblocked regions */
    vector<Interval*> allIntervals;
    LegalizeGetAllIntervals(allRegions, allIntervals);
    
    /* Form arcs between nodes under consideration */
    

    /* Print the regions and their respective supply */
    Region* RObj;
    cout << "Regions for row " << i <<" are " << endl;
    VECTOR_FOR_ALL_ELEMS(allRegions, Region*, RObj){
      cout << "(" << (RObj->RegionGetBegin()) << "," << (RObj->RegionGetEnd()) 
	   << ") " << (RObj->RegionGetEmpty()) << "\t" << (RObj->RegionGetLBound())
	   << "\t" << (RObj->RegionGetRBound()) << "\t" << (RObj->RegionGetSupply()) << endl;
    }END_FOR;
    
    /* Print the intervals and their respective supply */
    Interval* intObj;
    cout << "Intervals for row " << i <<" are " << endl;
    VECTOR_FOR_ALL_ELEMS(allIntervals, Interval*, intObj){
      cout << "(" << (intObj->IntervalGetBegin()) << "," << (intObj->IntervalGetEnd()) << ")" << intObj->IntervalGetSupply() << endl;
    }END_FOR;
    

  }END_FOR;
}        
    /* Debugging 2 
       pair<int, unsigned int> zoneObj;
       int pzoneBegin = -1;
       int zoneBegin;
       unsigned int zoneFlag;
       if(zones.size() > 1){
       
       cout<<"Zones for row "<<i<<" are "<<endl;
       VECTOR_FOR_ALL_ELEMS(zones, pair<int MCOMMA unsigned int>, zoneObj){
	if (pzoneBegin == -1){
	pzoneBegin = zoneObj.first;
	zoneFlag = zoneObj.second;
	} else{
	zoneBegin = zoneObj.first;
	cout << "[" << pzoneBegin << "," << zoneBegin <<"]" << zoneFlag<< endl;
	pzoneBegin = zoneBegin;
	zoneFlag = zoneObj.second;
	}
	}END_FOR;
	cout << "[" << pzoneBegin << "," << rowEnd <<"]" << zoneFlag<< endl;
	}
    */
     
    /* Snap cells in blocked zones to nearest empty zones */


# if 0
/* REVIEW FOR MORE EFFICIENCY */
void
LegalizeSnapCellsToFreeZone(int zoneBegin, int zoneEnd, vector<Cell*> &cellsInRow, int rowBegin, int rowEnd)
{
  Cell* cellObj;
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
    if (!CellIsFixed(cellObj)){
      int cellBegin = cellObj->CellGetXpos();
      int cellEnd = (cellBegin + cellObj->CellGetWidth());
      int cellMid = (cellBegin + cellEnd)/2 ;
      if ((cellMid >= zoneBegin) && (cellMid < zoneEnd)){
	/* Push Cell To nearest free zone */
	int leftDist = cellMid - zoneBegin + 1;
	int rightDist = zoneEnd - cellMid; 
	if (leftDist > rightDist){
	  if (zoneEnd == rowEnd){
	    /* Right boundary blocked so pushing cell
	       to the left free zone 
	    */
	    cellBegin -= leftDist;
	  } else{
	    /* Pushing cell to the next free zone */
	    cellBegin += rightDist;
	  }
	} else{
	  if (zoneBegin == rowBegin){
	    /* Left boundary blocked so pushing cell
	       to the next free zone on the right 
	  */
	    cellBegin += rightDist;
	  } else{
	    /* Pushing cell to the previoud free zone */
	    cellBegin -= leftDist;
	  }
	}
	cellObj->CellSetXpos(cellBegin);
      }
    }
  }END_FOR;
}

# endif

			       
/*
void
LegalizeCalculateSupplyForInterval(vector<Region*> &regions, Interval* interval)
{
  int totalWC = 0;
  int totalWR = 0;
  int supply;
  int LBCellWidt
h;
  int RBCellWidth;
  unsigned int bound;
  //bool right = false;
  int end = regions.size() - 1;
  Region* Obj;
  
  VECTOR_FOR_ALL_ELEMS(regions, Region*, Obj){
    totalWC += (Obj->RegionGetTotalCellWidth());
    totalWR += (Obj->RegionGetRegionWidth());
  }END_FOR;
  supply = totalWC + totalWR;
  RBCellWidth = (region[end]->RegionGetLCellWidth());
  LBCellWidth = (region[0]->RegionGetFCellWidth());
  
  if ((regions[0]->RegionGetLRBound()) == 0)
    supply -= ((1/2) * RBCellWidth)
  if ((regions[end]->RegionGetLRBound()) == 2)
    supply -= ((1/2) * LBCellWidth)
      
    }
    if (!bound){
      supply -= ((1/2) * (region[0]->RegionGetLCellWidth()));
      supply -= ((1/2) * (region[end]->RegionGetFCellWidth()));
    }
    if (supply >= 0){
      return supply;
    } else{
      return 0;
    }
  }
  else
*/

    /* 
    VECTOR_FOR_ALL_ELEMS(allRegions, Region*, RObj){
      cout << "(" << (RObj->RegionGetBegin()) << "," << (RObj->RegionGetEnd()) 
	   << ") " << (RObj->RegionGetBlocked()) <<endl;
      
      vector<Cell*> cellsInRegion = RObj->RegionGetCellsInRegion();
      if(cellsInRegion.size()){
	cout << "Cells In Region are" <<endl;
	Cell* CObj;
	VECTOR_FOR_ALL_ELEMS(cellsInRegion, Cell*, CObj){
	  cout << "CELL:" << CObj->CellGetName() <<",";
	}END_FOR;
	cout<<endl;
      }
      }END_FOR;
    */




# if 0    
    Cell* cellObj;
    
    VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
      
      
    }END_FOR;
    
  }END_FOR;


    cout << "Blocked Zones are: "<< endl;
      
    /* Find all the regions in the row */
    vector<Region*> allRegions;
    LegalizeFindRegionsInRow(zones, allRegions, Obj, columnWidth, rowBegin,
			     rowEnd);
    
    
    /* Set flags to represent boundary regions */
    (allRegions[0])->RegionSetLRBound(0);
    int end = (allRegions.size()) - 1;
    if (end == 0){
      (allRegions[end])->RegionSetLRBound(3);
    } else{
      (allRegions[end])->RegionSetLRBound(1);
    }
    
    
    Region* RObj;
    /* Find and Add cells in row to respective regions */
    
    VECTOR_FOR_ALL_ELEMS(allRegions, Region*, RObj){
      // cout << i << "region bound " <<(RObj->RegionGetLRBound())<<endl;
      RObj->RegionFindCellsInRegion(cellsInRow);
    }END_FOR;
    
    /* Calculate the supplies of each region */
    VECTOR_FOR_ALL_ELEMS(allRegions, Region*, RObj){
      LegalizeCalculateSupplyForRegion(RObj, allRegions);
    }END_FOR;
    
    
    /* Print the regions and their respective supply */
    cout << "Regions for row " << i <<" are " << endl;
    VECTOR_FOR_ALL_ELEMS(allRegions, Region*, RObj){
      cout << "(" << (RObj->RegionGetBegin()) << "," << (RObj->RegionGetEnd()) 
	   << ") " << (RObj->RegionGetSupply()) <<endl;
    }END_FOR;
    
    /* Create intervals with non-zero supply values */
  }END_FOR;

VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
      cout << "Cell : " << cellObj->CellGetName() << " is now at " << cellObj->CellGetXpos() << endl;
    }END_FOR;
    
    /* Print Zones for testing */
    if (debugPrint){
      if(zones.size() > 1){
	cout << "Zones for Row " << i << " are " << endl;
	Zone* zoneObj;
	VECTOR_FOR_ALL_ELEMS(zones, Zone*, zoneObj){
	  cout << "(" << zoneObj->ZoneGetBegin() << "," << zoneObj->ZoneGetEnd() << ")\t"
	       << zoneObj->ZoneGetEmpty() << endl;
	}END_FOR;
      } else{
	cout << "Only one zone in row : " << zones[0]->ZoneGetBegin() << "," << zones[0]->ZoneGetEnd() 
	     << ")\t" << zones[0]->ZoneGetEmpty() << endl;
      }
    }


/* For Debugging 
    Cell* cellObj;
    VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
      if(CellIsFixed(cellObj)){
	cout << "Cell: "<<cellObj->CellGetName()<<" is fixed"<<endl;
      }
    }END_FOR;
    
    Cell* cellObj;
    VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, cellObj){
      cout << "Cell : " << cellObj->CellGetName() << " was at " << cellObj->CellGetXpos() << endl;
    }END_FOR;
    */



# endif
  
  



  /*
  ListDigraph g;
  ArcMap<int> capacities(g);
  NodeMap<int> supply(g);  
  LegalizeConstructGNFGraph(allPhysRows, g, capacities, supply);
  LegalizeCalculateMinCostFlow(g, capacities, supply);
  cout<<"Cells are snapped to Rows, but hold on. Legalizing yet to be done."<<endl;

  // cout<<"Phew..... Legalizing done\n>>>>> Let's go Rock the Party <<<<<"<<endl;
  */

# if 0

void
LegalizeConstructGNFGraph(vector<PhysRow*> &allPhysRows, ListDigraph &g, 
			  ArcMap<int> &capacities, NodeMap<int> &supply)
{
  /* Used to define infinity for capacity values
 
  int MAX = std::numeric_limits<int>::max();
  cout<<"MAX "<<MAX<<endl;
  int INFI = std::numeric_limits<int>::has_infinity ?
    std::numeric_limits<int>::infinity() : MAX;
  cout<<"INFI "<<INFI<<endl;
  */
  
  vector<ListDigraph::Node> n;
  vector<ListDigraph::Arc> a;
  bool firstRowNode = true;
  int rowsWithWhiteSpaces = 0;
  int rowsWithExcessCells = 0;
  int wMax, wsViolation;
  int prevWmax;
  int totWSWidth = 0;
  int totExcessCellWidth = 0;
  int numRows = (allPhysRows.size());
  
  
 
  
  /* Creating a global network flow graph 
     Capacity and cost of each arc are assigned
     later */
  
  
  /* Adding a virtual source and a sink node */
  n.push_back(g.addNode()); 
  n.push_back(g.addNode());
  
  
  

  /* Add the row nodes to the diGraph */
  
  unsigned int nodeIndex = 2;
  unsigned int arcIndex = 0;
  PhysRow* Obj;

  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    wMax = (Obj->PhysRowGetWMax());
    wsViolation = (wMax - (Obj->PhysRowGetTotalCellWidth()));
    
    //cout << "DBG: For row " << (nodeIndex-2)  << ":" << endl;
    //cout << "DBG: Wmax :" << wMax  << " wsViolation: " << wsViolation << endl;
        
    /* Supply rows */
    if(wsViolation < 0){
      //Adding a supply node and its arc to the source
      n.push_back(g.addNode()); 
      a.push_back(g.addArc(n[0],n[nodeIndex]));
      nodeIndex++;
      capacities[a[arcIndex]] = abs(wsViolation);
      arcIndex++;
      rowsWithExcessCells++;
      totExcessCellWidth += abs(wsViolation);
      /* Adding arcs between teo rows */
      if(!firstRowNode){
	a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-1)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
	a.push_back(g.addArc(n[(nodeIndex-1)],n[(nodeIndex-2)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
      }
      else
	firstRowNode = false;
    }
    if(wsViolation > 0){
      //Adding a demand node and its arc to the sink
      n.push_back(g.addNode());
      a.push_back(g.addArc(n[nodeIndex],n[1]));
      nodeIndex++;
      capacities[a[arcIndex]] = wsViolation;
      arcIndex++;
      rowsWithWhiteSpaces++;
      totWSWidth += wsViolation;
      if(!firstRowNode){
	a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-1)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
	a.push_back(g.addArc(n[(nodeIndex-1)],n[(nodeIndex-2)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
      }
      else
	firstRowNode = false;
    }
    
  }END_FOR;
  
  /* Adding supply values to the virtual source and sink nodes */

  supply[n[0]] = totExcessCellWidth;
  //supply[n[0]] = 10000;
  //supply[n[1]] = -totWSWidth;
  supply[n[1]] = -totExcessCellWidth;
  
  cout << "DBG: Total supply :" << totExcessCellWidth << endl;
  cout << "DBG: Total demand :" << -totExcessCellWidth << endl;

  cout << "Node : Supply/Demand"<<endl;
  for (NodeIt i(g); i != INVALID; ++i)
    cout << g.id(i) << " : "<<supply[i]<<endl;
   
  cout<<"Arc : Capacity : Cost"<<endl;
  for (ArcIt i(g); i != INVALID; ++i){
    cout << "(" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
    cout <<" : "<<capacities[i]<<" : 1"<<endl;
  }
  
  cout<<"Total number of Physical Rows in design : "<<numRows<<endl;
  cout<<"Total rows with excess of WS : "<<rowsWithWhiteSpaces<<endl;
  cout<<"Total rows with lack of WS : "<<rowsWithExcessCells<<endl;
  cout<<"Total number of Nodes Created :  "<<countNodes(g)<<endl;
  cout<<"Total number of Arcs Created : "<<countArcs(g)<<endl;
}

# endif
