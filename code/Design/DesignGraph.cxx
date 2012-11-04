/****** 
FUNCTIONS TO CONVERT THE DESIGN THAT HAS 
BEEN READ TO A HYPERGRAPH 
******/
# include <Design.h>
# include <DesignIter.h>
# include <Stat.h>

/**************/
void DesignCreateGraph(Design& myDesign, HyperGraph& thisGraph)
{
  vector<void*> listOfCells;
  Cell *CellPtr;
  Net *NetPtr;
  Pin *PinPtr;
  string Name;

  /* First add all the nodes */
  DESIGN_FOR_ALL_CELLS(myDesign, Name, CellPtr) {
    thisGraph.HyperGraphAddNode((void *)(CellPtr));
  } DESIGN_END_FOR;

  //  cout << "Memory used after adding nodes to graph: " << getMemUsage() << MEM_USAGE_UNIT << endl;  
  /* Iterate over all hyperedges and use them to create
     a hypergraph */
  DESIGN_FOR_ALL_NETS(myDesign, Name, NetPtr) {
    //    cout << "Memory used before adding edge: " << Name << "to graph: " << getMemUsage() << MEM_USAGE_UNIT << endl;  
    NET_FOR_ALL_CELLS((*NetPtr), CellPtr) {
      listOfCells.insert(listOfCells.end(), (void *)CellPtr);
    } NET_END_FOR;
    thisGraph.HyperGraphAddEdge(listOfCells, (void *)NetPtr, 
		      (*NetPtr).NetGetWeight());
    listOfCells.clear();
    //    cout << "Memory used after adding edge: " << Name << "to graph: " << getMemUsage() << MEM_USAGE_UNIT << endl;  
  } DESIGN_END_FOR;
}

