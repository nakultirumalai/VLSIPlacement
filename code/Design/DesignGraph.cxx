/****** 
FUNCTIONS TO CONVERT THE DESIGN THAT HAS 
BEEN READ TO A HYPERGRAPH 
******/
# include <Design.h>
# include <DesignIter.h>
# include <Stat.h>

/**************/
void 
Design::DesignCreateGraph(HyperGraph& thisGraph)
{
  vector<void*> listOfCells;
  Cell *CellPtr;
  Net *NetPtr;
  Pin *PinPtr;
  string Name;
  Env &DesignEnv = DesignGetEnv();

  /* First add all the nodes */
  DESIGN_FOR_ALL_CELLS((*this), Name, CellPtr) {
    //    cout << "Cell: " << (*CellPtr).CellGetName() << " being added to the hypergraph" << endl;
    thisGraph.AddNode((void *)(CellPtr));
  } DESIGN_END_FOR;

  /* Iterate over all hyperedges and use them to create
     a hypergraph */
  DESIGN_FOR_ALL_NETS((*this), Name, NetPtr) {
    //    cout << "Net: " << Name << " being added to the hypergraph" << endl;
    NET_FOR_ALL_CELLS((*NetPtr), CellPtr) {
      listOfCells.insert(listOfCells.end(), (void *)CellPtr);
    } NET_END_FOR;
    thisGraph.AddEdge(listOfCells, (void *)NetPtr, (*NetPtr).NetGetWeight());
    listOfCells.clear();
  } DESIGN_END_FOR;
}

