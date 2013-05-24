# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>
# include <Flags.h>
# include <Legalize.h>
# include <lemon/list_graph.h>

using namespace lemon;

# define MAX_ARGS 7

HyperGraph& convertDesignToGraph(Design& thisDesign)
{
  HyperGraph *myGraph = new HyperGraph();
  
  DesignCreateGraph(thisDesign, (*myGraph));
  thisDesign.DesignSetGraph(*myGraph);

  return (*myGraph);
}

int main(int argc, char *argv[])
{
  string designName, designPath;
  string switchName;
  int i;

  designName = "";
  designPath = "";

  if (argc > 1 && argc < MAX_ARGS) {
    i = 1;
    while (i < argc) {
      if (argv[i][0] == '-') {
	/* Switch to options */
	switchName = argv[i] + 1;
	if (switchName == "analyse") {
	  performAnalysis = true;
	} else if (switchName == "trace_depth") {
	  string traceDepthStr = argv[i+1];
	  if (!strIsNumber(traceDepthStr)) {
	    _ERROR("Trace depth specified is not a number. Assuming 1");
	    traceDepth = 1;
	  } else {
	    traceDepth = atoi(traceDepthStr.data());
	    i++;
	  }
	}
      } else {
	if (designPath == "") {
	  designPath = argv[i];
	} else if (designName == "") {
	  designName = argv[i]; 
	}
      }
      i++;
    }
    FlagsInit();

    Design myDesign(designPath, designName);
    HyperGraph &myGraph = convertDesignToGraph(myDesign);
    
    //myDesign.DesignSolveForSeqCells();
    //myDesign.DesignSolveForAllCellsIter();
    LegalizeDesign(myDesign);
    DesignWriteBookShelfOutput(myDesign); 

    //    DesignWriteBookShelfOutput(myDesign); 
    string plotFileName;
    plotFileName = "Legal.plt";
    myDesign.DesignPlotData("Title", plotFileName);
    return 0;
    /* Plot the stuff in the design */
    //    string plotFileName;
    //    plotFileName = "Itr.plt";
    //    myDesign.DesignPlotData("Title", plotFileName);

    /* Plot the stuff in the design 
    string plotFileName;
    plotFileName = "Legal.plt";
    myDesign.DesignPlotData("Title", plotFileName);
*/

    if (performAnalysis == true) {
      DesignCollectStats(myDesign);
      DesignWriteStats(myDesign);
    }

    //LegalizeDesign(myDesign);
    
    /*
    
    string CellName="";
    Cell* CellPtr;
    
    vector<PhysRow*> allPhysRows = myDesign.DesignGetRows();
    vector<vector<unsigned int> > allRowBounds;
    LegalizeGetAllBoundingBoxes(allPhysRows, allRowBounds);
    
    
    rowOrientation rowType=((allPhysRows[0])->PhysRowGetType());
    unsigned int rowHeight=((allPhysRows[0])->PhysRowGetHeight());
    unsigned int numPhysRows=myDesign.DesignGetNumPhysRows();
    unsigned int maxY=(rowHeight*numPhysRows - rowHeight);
    cout<<"Max Y is"<<maxY<<endl;
    
    */

    /* Initialize isLegal variable in all cells
       To be done by the solver in final Design */

    /* Done in Cell constuctor, not required anymore 
    DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
      if(!(CellPtr->CellIsTerminal()))	
	CellPtr->CellSetIsLegal(false);
    }DESIGN_END_FOR;

    */
    
    /* Was used for testing legalize funtion, not required if solver results are read 
    //Modifying Cell locations for testing 
    
    DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
    if(!(CellPtr->CellIsTerminal())){	
      int y= CellPtr->CellGetYpos();
	cout<<"Cell:"<<CellName<<" was at ("<<CellPtr->CellGetXpos()<<","<<
	CellPtr->CellGetYpos()<<")"<<endl; 
	int modY= y + (rand() % maxY);
	CellPtr->CellSetYpos(modY);
	cout<<"Cell is now at ("<<CellPtr->CellGetXpos()<<","<<
	CellPtr->CellGetYpos()<<")"<<endl;
      }
    }DESIGN_END_FOR;

    */

    /* Checking the Legality of all Cells and setting the variable isLegal
       for all cells accordingly */


    //myDesign.DesignClusterCells(myGraph, DEFAULT_CLUSTER);
    /* Second param is clustering type. Can take on the following values:
       FCC_CLUSTER, NET_CLUSTER, ESC_CLUSTER */
    /* Function moved to LegalizeDesign 
    DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
      if(!(CellPtr->CellIsTerminal()))
	LegalizeCheckLegality(CellPtr, allPhysRows, allRowBounds);
    }DESIGN_END_FOR;

    */
    
    /* Legalizing Design so that all Cells are placed in rows */
    
    /* Function moved to LegalizeDesign 
    DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
      if(!(CellPtr->CellIsTerminal())){
	if((CellPtr->CellIsLegal())==true)
	  cout<<"Cell: "<<CellName<<"is legal"<<endl;
	else{
	  cout<<"Cell: "<<CellName<<"is notlegal"<<endl;
	  cout<<"Legalizing..."<<endl;
	  LegalizeSnapToNearestRows(CellPtr, allPhysRows, rowType);
	}
      }
    }DESIGN_END_FOR;
    */
    /* Will add later after testing SnapToRows 

     //Get the subRow locations and row locations for all Cells
     //and assign Cells to those locations 

     int cellCount=0;
     DESIGN_FOR_ALL_CELLS(myDesign, CellName, CellPtr){
       if(!(CellPtr->CellIsPort())){
	 myDesign.DesignAddCellToPhysRow(CellPtr, allRowBounds, allPhysRows);
	 cellCount++;
       }
     }DESIGN_END_FOR;
     
     cout<<cellCount<<" cells added to Rows."<<endl;
    

     // Get Cells in Rows
     
     PhysRow* Obj;
     VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
       vector<vector<Cell *> > allCells;
       cout<<"Cells in Row:"<<i<<endl;
       Obj->PhysRowGetCellsInRow(allCells);
       vector<Cell*> Obj2;
       VECTOR_FOR_ALL_ELEMS(allCells, vector<Cell *>, Obj2){
	 cout<<" and subRow: "<<i<<" are: "<<endl;
	 Cell* Obj1;
	 VECTOR_FOR_ALL_ELEMS(Obj2, Cell*, Obj1){
	   cout<<Obj1->CellGetName()<<", ";
	 }END_FOR;
	 cout<<endl;
       }END_FOR;
       cout<<endl;
       int cellArea = (Obj->PhysRowGetTotalCellArea());
       int boundingBoxArea = (Obj->PhysRowGetBoundingBoxArea());
       int blockedArea = (Obj->PhysRowGetBlockedArea());
       cout<<"Total Cell Area: "<<cellArea<<endl;
       cout<<"Total Bounding box area: "<<boundingBoxArea<<endl;
       cout<<"Total Blocked area: "<<blockedArea<<endl;
       cout<<"Wmax = "<<(boundingBoxArea - blockedArea)<<endl;
     }END_FOR;
     
     LegalizeConstructGNFGraph(allPhysRows, g);
    */
    //    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
  }
}
