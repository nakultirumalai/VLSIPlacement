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

void
LegalizeDesign(Design &myDesign)
{
  
  typedef vector<pair<int, unsigned int> > vector_type;
  
  /* Get all the physical rows in Design */
  vector<PhysRow*> allPhysRows;
  allPhysRows  = myDesign.DesignGetRows();
  
  /* Get bounding boxes for all subrows in all rows in Design */
  vector<vector<int> > allRowBounds;
  LegalizeGetAllBoundingBoxes(allPhysRows, allRowBounds);
  
  rowOrientation rowType=((allPhysRows[0])->PhysRowGetType());
  string CellName;
  Cell* CellPtr;
  
  /* Used for assigning randoms postions to cells in pl file
     Required for testing Legalize
     unsigned int rowHeight=((allPhysRows[0])->PhysRowGetHeight());
     unsigned int numPhysRows=myDesign.DesignGetNumPhysRows();
     unsigned int maxY=(rowHeight*numPhysRows - rowHeight);
     cout<<"Max Y is"<<maxY<<endl; 
  */
  
  /* Checking the Legality of all Cells and setting the variable isLegal
     for all cells accordingly 

  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal()))
      LegalizeCheckLegality(CellPtr, allPhysRows, allRowBounds);
  }DESIGN_END_FOR;
  */

  

  /* Legalizing design so that every cell is contained completely in some row */

  /*
  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal())){
      
      if((CellPtr->CellXIsLegal()) && (CellPtr->CellYIsLegal()));
	//cout<<"Cell: "<<CellName<<"is legal"<<endl;
      else{
	//cout<<"Cell: "<<CellName<<"is notlegal"<<endl;
	//cout<<"Legalizing..."<<endl;
	LegalizeSnapToNearestRows(CellPtr, allPhysRows, rowType);
      }
    }
  }DESIGN_END_FOR;
  */
  
  myDesign.DesignAddAllCellsToPhysRows();
  
  /* Get the zones of each row */
  //unsigned int rowBegin = ((allPhysRows[0])->PhysRowGetRowBegin());
  //unsigned int rowEnd = ((allPhysRows[0])->PhysRowGetRowEnd());

  unsigned int rowBegin = 0;
  unsigned int rowEnd = 119472;
  int columnWidth = 16;

  PhysRow* Obj;				  
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    Obj->PhysRowMarkFixedCellsInRow(columnWidth);
    vector<pair<int,unsigned int> > zones;
    LegalizeFindZonesInRow(Obj, zones, rowBegin, rowEnd);
    cout<<"Zones for row "<<i<<" are "<<endl;
    for (vector_type::const_iterator pos = zones.begin();
	 pos != zones.end(); ++pos){
      cout << pos->first << " " << pos->second << endl;
    }
    vector<pair<int,unsigned int> > regions;
    LegalizeFindRegionsInRow(zones, regions, Obj, columnWidth);
    cout<<"Regions for row "<<i<<" are "<<endl;
    for (vector_type::const_iterator pos = regions.begin();
	 pos != regions.end(); ++pos){
      cout << pos->first << " " << pos->second << endl;
    }
  }END_FOR;




  /*
  ListDigraph g;
  ArcMap<int> capacities(g);
  NodeMap<int> supply(g);  
  LegalizeConstructGNFGraph(allPhysRows, g, capacities, supply);
  LegalizeCalculateMinCostFlow(g, capacities, supply);
  cout<<"Cells are snapped to Rows, but hold on. Legalizing yet to be done."<<endl;

  // cout<<"Phew..... Legalizing done\n>>>>> Let's go Rock the Party <<<<<"<<endl;
  */
}

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
  
  
  bool debugPrint;
  
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
		      vector<vector<unsigned int> > &allRows)
{
  /* Get the values the coordinates, height and width of the cells */
  int cellX = cell->CellGetXpos();
  int cellY = cell->CellGetYpos();
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();
  vector<unsigned int> Obj; 
  
  rowOrientation rowType = (allPhysRows[0]->PhysRowGetType());
  
  VECTOR_FOR_ALL_ELEMS(allRows, vector<unsigned int>, Obj){
    
    /* Indicate more than one subRows */
    
    if (Obj.size() > 4){
      if (rowType == HORIZONTAL){
	for (int i = 0; i < Obj.size(); i+=4){
	  int botX = Obj[i];
	  int botY = Obj[i+1];
	  int topX = Obj[i+2];
	  int topY = Obj[i+3];
	  int cellXend = cellX + cellWidth;
	  int cellYend = cellY + cellHeight;
	  bool check1 = false;
	  bool check2 = false;
	  /* Checking X and Y Legality */
	  if ((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	      (cellXend <= topX)){
	    cell->CellSetXIsLegal(true);
	    check1 = true;
	  }
	  
	  if((cellY == botY) && (cellYend > botY) && 
	     ((cellHeight % (topY - botY)) == 0)){
	    cell->CellSetYIsLegal(true);  
	    check2 = true;
	  }
	  if(check1 && check2) break; /* No more checks required */
	}
      }
      else if(rowType == VERTICAL){
	for(int i = 0; i < Obj.size(); i += 4){
	  int botX = Obj[i];
	  int botY = Obj[i+1];
	  int topX = Obj[i+2];
	  int topY = Obj[i+3];
	  int cellXend = cellX + cellHeight;
	  int cellYend = cellY + cellWidth;
	  bool check1 = false;
	  bool check2 = false;
	  if ((cellX == botX) && (cellXend > botX) && 
	      ((cellHeight % (topX - botX)) == 0)){
	    cell->CellSetXIsLegal(true);
	    check1 = true;
	  }
	  if ((cellY >= botY) && (cellY < topY) &&
	      (cellYend > cellY) && (cellYend <= topY)){
	    cell->CellSetYIsLegal(true);
	    check2 = true;
	  }
	  if(check1 && check2) break;
	  
	}
      }
    }
    
    /* Indicates one subRow present in the entire row */
    else if ((Obj.size() == 4)){
      //cout<<"In rows with single subrows"<<endl;
      if (rowType == HORIZONTAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellWidth;
	int cellYend = cellY + cellHeight;
	if (0){
	  cout<<"BotX="<<botX<<endl;
	  cout<<"BotY="<<botY<<endl;
	  cout<<"topX="<<topX<<endl;
	  cout<<"topY="<<topY<<endl;
	  cout<<"CellXend="<<cellXend<<endl;
	  cout<<"CellYend="<<cellYend<<endl;
	  cout<<"CellX="<<cellX<<endl;
	  cout<<"CellY="<<cellY<<endl;
	}
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
	 cellX = cellX - (otherCoordinateEnd - cellXend);
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
       
  }
  else if(rowType==VERTICAL){
     int key=cell->CellGetXpos();
     int cellY = cell->CellGetYpos();
     int cellYend = cellY + (cell->CellGetHeight());
     
     /* Cell has violated X-bounds */

     if(!(cell->CellXIsLegal())){
       findRowCoordinates(RowCoordinates, min, max, key, nearRows);
       if(fabs(key-nearRows[1])>=fabs(key-nearRows[0])){
	 cell->CellSetXpos(nearRows[0]);
       }
              else{
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
ascendingXpos(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetXpos()) < (cell2->CellGetXpos()));
}

void
LegalizeFindZonesInRow(PhysRow* row, vector<pair<int,unsigned int> > &zones, 
		       unsigned int rowBegin, unsigned int rowEnd)
{
  vector<Cell*> allCells;
  row->PhysRowGetCellsInRow(allCells);
  Cell* Obj;
  
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj){
    cout<<(Obj->CellGetName())<<endl;
  }END_FOR;

  vector<Cell*> fixedCells;
  /* Get all the fixed cells in the row */
  
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj){
    if(CellIsFixed(Obj)){
      fixedCells.push_back(Obj);
    }
  }END_FOR;
  
  /* Sort fixed cells in ascending order of x-pos */
  if(fixedCells.size()){
    sort(fixedCells.begin(),fixedCells.end(),ascendingXpos);
    /* Iterate over all fixed cells to find the zones */
    bool firstBlockedZone = true;
    
    int prevBlockedZoneEnd = 0;
    int blockedZoneEnd = 0;

    VECTOR_FOR_ALL_ELEMS(fixedCells, Cell*, Obj){
      int xPos = ((*Obj).CellGetXpos());
      if(firstBlockedZone){
	if (xPos == rowBegin){
	  zones.push_back(make_pair(xPos,false));
	  blockedZoneEnd = xPos + ((*Obj).CellGetWidth());
	}
	else if(xPos > rowBegin){
	  zones.push_back(make_pair(rowBegin,true));
	  zones.push_back(make_pair(xPos,false));
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
	  zones.push_back(make_pair(prevBlockedZoneEnd,true));
	  zones.push_back(make_pair(xPos,false));
	  blockedZoneEnd = (xPos + ((*Obj).CellGetWidth()));
	}
      }
      prevBlockedZoneEnd = blockedZoneEnd;
    }END_FOR;
    if(prevBlockedZoneEnd != rowEnd)
      zones.push_back(make_pair(prevBlockedZoneEnd,true));
  }
  /* If row does not have any cell(s) that qualfies as fixed */
  else{
    zones.push_back(make_pair(rowBegin,true));
  }
}

bool
ascendingRegions(pair<int, unsigned int> p1, pair<int, unsigned int> p2)
{
  return((p1.first) < (p2.first));
}

void
LegalizeFindRegionsInRow(vector<pair<int,unsigned int> > &zones, 
			 vector<pair<int,unsigned int> > &regions, PhysRow* &myRow,
			 int columnWidth)
{
  int rowBegin = ((*myRow).PhysRowGetRowBegin());
  int rowEnd = (((*myRow).PhysRowGetNumSites()) * ((*myRow).PhysRowGetSiteSpacing()));
  for (int i = rowBegin; i < rowEnd; i+=columnWidth){
    regions.push_back(make_pair(i,2));
  }
  /* Finding the union of zones and column boundaries
     to give regions 
  */

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
}  
