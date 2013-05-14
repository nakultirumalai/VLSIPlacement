# include <Legalize.h>
# include <lemon/list_graph.h>
# include <lemon/network_simplex.h>

//using namespace std;
using namespace lemon;

# define INFEASIBLE 0
# define OPTIMAL 1
# define UNBOUNDED 2
# define DEFAULT_DEMAND 119472

void
LegalizeTestMinCostFlow(void)
{
  typedef ListDigraph::NodeIt NodeIt;
  typedef ListDigraph::ArcIt ArcIt;
  
  ListDigraph g;
  vector<ListDigraph::Node> n;
  vector<ListDigraph::Arc> a;
  /* Create maps to assign cost and capacites to each arc */
  ListDigraph::ArcMap<int> capacities(g);
  ListDigraph::ArcMap<int> costs(g);
  ListDigraph::ArcMap<int> flows(g);
  /* Create a map to assign supply values to each node */
  ListDigraph::NodeMap<int> supply(g);

  /* Add source and sink nodes */
  n.push_back(g.addNode());
  supply[(n[0])] = 7;
  n.push_back(g.addNode());
  supply[(n[1])] = -7;
  
  /* Add four row nodes */
  for(int i = 0; i < 4; i++)
    n.push_back(g.addNode());
 
  /* Adding arcs between four row nodes */
  for(int i = 2; i < 5; i++){
    a.push_back(g.addArc(n[i],n[i+1]));
    a.push_back(g.addArc(n[i+1],n[i]));
  }
  
  /* Adding arcs between source and rows */
  a.push_back(g.addArc(n[0],n[2]));
  a.push_back(g.addArc(n[0],n[4]));

  /* Adding arcs between rows and sink */
  a.push_back(g.addArc(n[3],n[1]));
  a.push_back(g.addArc(n[5],n[1]));

  /* Adding capacities to the row arcs */
  for(int i = 0; i < 6; i++){
    capacities[(a[i])] = 10;
  }
  costs[(a[0])] = 10;
  costs[(a[1])] = 5;
  costs[(a[2])] = 2;
  costs[(a[3])] = 30;
  costs[(a[4])] = 21;
  costs[(a[5])] = 16;
  costs[(a[6])] = 2;
  costs[(a[7])] = 5;
  costs[(a[8])] = 10;
  costs[(a[9])] = 17;
  /* Adding capacities to the source and sink arcs */
  capacities[(a[6])] = 4;
  capacities[(a[7])] = 3;
  capacities[(a[8])] = 2;
  capacities[(a[9])] = 5;
  
  /* Printing the results */
  cout << "Nodes:";
  for (NodeIt i(g); i != INVALID; ++i)
    cout << " " << g.id(i)<<"\t"<<supply[i]<<endl;
  
  cout<<"Arcs:";
  for (ArcIt i(g); i != INVALID; ++i){
    cout << " (" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
    cout <<"\t"<<capacities[i]<<endl;
  }

  /* Run Min Cost Flow Solver */
  NetworkSimplex<ListDigraph> ns(g);
  int retVal;
  //  NetworkSimplex<lemon::ListDigraph>::SupplyType sType;
  //  sType = LEQ;
  retVal = ns.supplyType(lemon::NetworkSimplex<lemon::ListDigraph>::LEQ).upperMap(capacities).supplyMap(supply).run();
  
  if (retVal == INFEASIBLE) {
    cout << "Infeasible" << endl;
  } else if (retVal == OPTIMAL) {
    cout << "optimal" << endl;
  } else if (retVal == UNBOUNDED) {
    cout << "UNBOUNDED" << endl;    
  }
  /* Print out the flows 
  cout<<"Arc Flows:";
  for (ArcIt i(g); i != INVALID; ++i){
    cout << " (" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
    cout <<"\t"<<ns.flow()<<endl;
  }
  */
  ns.flowMap(flows);
  cout<<"Arc Flows:";
  for (ArcIt i(g); i != INVALID; ++i){
    cout << " (" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
    cout <<"\t"<<flows[i]<<endl;
  }
  cout<<"total cost = "<<(ns.totalCost<int>())<<endl;
}   
  

void
LegalizeConstructGNFGraph(vector<PhysRow*> &allPhysRows, ListDigraph &g)
{
  
  typedef ListDigraph::NodeIt NodeIt;
  typedef ListDigraph::ArcIt ArcIt;
  typedef ListDigraph::Node Node;
  typedef ListDigraph::Arc Arc;
  
  vector<Node> n;
  vector<Arc> a;
  
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
  
  
  /* Create maps to assign cost and capacites to each arc */
  ListDigraph::ArcMap<int> capacities(g);
  ListDigraph::ArcMap<int> costs(g);
  
  /* Create a map to assign supply values to each node */
  ListDigraph::NodeMap<int> supply(g); 


  /* Create a map to copy the results of the solver */
  ListDigraph::ArcMap<int> flows(g);  

  /* Add the row nodes to the diGraph */
  unsigned int rowCount;
  int totalWs;
  unsigned int nodeIndex = 2;
  unsigned int arcIndex = 0;
  PhysRow* Obj;
  
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    wMax = (Obj->PhysRowGetBoundingBoxWidth()) - (Obj->PhysRowGetBlockedWidth());
    wsViolation = (wMax - (Obj->PhysRowGetTotalCellWidth()));
    cout << "DBG: For row " << (nodeIndex-2)  << ":" << endl;
    cout << "DBG: Wmax :" << wMax  << " wsViolation: " << wsViolation << endl;
    if(wsViolation > 0)
      totalWs += wsViolation;
    
    
    
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
     
      //Adding a dummy demand node and its arc to the sink
      n.push_back(g.addNode());
      a.push_back(g.addArc(n[nodeIndex],n[1]));
      nodeIndex++;
      capacities[a[arcIndex]] = DEFAULT_DEMAND;
      arcIndex++;
      rowsWithWhiteSpaces++;
      totWSWidth += DEFAULT_DEMAND;
      
      //Adding the inter-row arcs 
      
      a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-1)]));
      capacities[a[arcIndex]] = DEFAULT_DEMAND;
      arcIndex++;
      a.push_back(g.addArc(n[(nodeIndex-1)],n[(nodeIndex-2)]));
      capacities[a[arcIndex]] = wMax;
      arcIndex++;
      if(!firstRowNode){
	a.push_back(g.addArc(n[(nodeIndex-3)],n[(nodeIndex-2)]));
	capacities[a[arcIndex]] = wMax;
	arcIndex++;
	a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-3)]));
	capacities[a[arcIndex]] = DEFAULT_DEMAND;
	arcIndex++;
      }
      else firstRowNode = false;
    }
    
  }END_FOR;
  
  for(int i = 0; i<50; i++){
    
    /* Adding a demand node and its corresponding arc to sink */
    n.push_back(g.addNode());
    a.push_back(g.addArc(n[nodeIndex],n[1]));
    nodeIndex++;
    capacities[a[arcIndex]] = DEFAULT_DEMAND;
    arcIndex++;
    rowsWithWhiteSpaces++;
    totWSWidth += DEFAULT_DEMAND;
    
    /* Adding arc to and from the row node before this */
    a.push_back(g.addArc(n[(nodeIndex-2)],n[(nodeIndex-1)]));
    capacities[a[arcIndex]] = DEFAULT_DEMAND;
    arcIndex++;
    a.push_back(g.addArc(n[(nodeIndex-1)],n[(nodeIndex-2)]));
    capacities[a[arcIndex]] = DEFAULT_DEMAND;
    arcIndex++;
  }

  cout << "DBG: Total WS in design :" << totalWs << endl;

  /* Adding supply values to the virtual source and sink nodes */

  //supply[n[0]] = totExcessCellWidth;
  supply[n[0]] = 10000;
  supply[n[1]] = -totWSWidth;
  
  cout << "DBG: Total supply :" << totExcessCellWidth << endl;
  cout << "DBG: Total demand :" << totWSWidth << endl;

  /* Print all nodes and their supplies */
  cout << "Nodes:"<<endl;
  for (NodeIt i(g); i != INVALID; ++i)
    cout << " " << g.id(i)<<" : "<<supply[i]<<endl;
  
  /* Print all the arcs and their capacities */
  cout<<"Arcs:"<<endl;
  for (ArcIt i(g); i != INVALID; ++i){
    cout << " (" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
    cout <<"->"<<capacities[i]<<endl;
  }

  if(debugPrint){
    cout<<"Total number of Physical Rows in design : "<<numRows<<endl;
    cout<<"Total rows with excess of WS : "<<rowsWithExcessCells<<endl;
    cout<<"Total rows with lack of WS : "<<rowsWithWhiteSpaces<<endl;
    cout<<"Total number of Nodes : "<<countNodes(g)<<endl;
    cout<<"Total number of arcs : "<<countArcs(g)<<endl;
  }
  //if(0){
    int retVal;
    NetworkSimplex<ListDigraph> ns(g);
    // First run
    retVal = ns.supplyType(lemon::NetworkSimplex<lemon::ListDigraph>::LEQ).upperMap(capacities).supplyMap(supply).run();
    if (retVal == INFEASIBLE) {
      cout << "Infeasible" << endl;
    } else if (retVal == OPTIMAL) {
      cout << "optimal" << endl;
    } else if (retVal == UNBOUNDED) {
      cout << "UNBOUNDED" << endl;    
    }
    
    ns.flowMap(flows);
    cout<<"Arc Flows:";
    for (ArcIt i(g); i != INVALID; ++i){
      cout << " (" << g.id(g.source(i)) << "," << g.id(g.target(i)) << ")";
      cout <<"\t"<<flows[i]<<endl;
    }
    cout<<"total cost = "<<(ns.totalCost<int>())<<endl;
    //} 
}

void 
LegalizeGetAllBoundingBoxes(vector<PhysRow*> &allPhysRows, 
			    vector<vector<unsigned int> > &allRows)
{
  PhysRow* Obj;
  
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    vector<unsigned int> v;
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

      
void
LegalizeDesign(Design &myDesign)
{
  /* Get all the physical rows in Design */
  vector<PhysRow*> allPhysRows;
  allPhysRows  = myDesign.DesignGetRows();
  
  /* Get bounding boxes for all subrows in all rows in Design */
  vector<vector<unsigned int> > allRowBounds;
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
     for all cells accordingly */

  DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal()))
      LegalizeCheckLegality(CellPtr, allPhysRows, allRowBounds);
  }DESIGN_END_FOR;
  

  /* Legalizing design so that every cell is contained completely in some row */

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
  myDesign.DesignAddAllCellsToPhysRows();
  ListDigraph g;
  LegalizeConstructGNFGraph(allPhysRows,g);
  
  cout<<"Cells are snapped to Rows, but hold on. Legalizing yet to be done."<<endl;

  // cout<<"Phew..... Legalizing done\n>>>>> Let's go Rock the Party <<<<<"<<endl;
}
