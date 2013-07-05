# include <Design.h>

/*********************************************************************
   Top level function to perform default clustering 
   Iterate over the netlist and generate vector     
   of vector of cells and a cluster is created for  
   each vector of cells in the top level vector     
*********************************************************************/
bool
Design::DesignDoDefaultCluster(HyperGraph &myGraph)
{
  Design& myDesign = (*this);
  Cell *cellPtr, *thisCellPtr;
  vector<vector<void * > >cellsToCluster;
  string cellName;
  unsigned int numClustered;
  uint numClusters;
  bool rtv;

  _STEP_BEGIN("Begin default clustering");

  rtv = true;
  numClusters = 0;
  DESIGN_FOR_ALL_STD_CELLS(myDesign, cellName, cellPtr) {
    if ((*cellPtr).CellIsHidden()) continue;
    vector<Cell *> connectedCells = DesignGetConnectedCells(myGraph, cellPtr);
    if (connectedCells.empty()) {
      continue;
    }
    connectedCells.push_back(cellPtr);
    DesignClusterCells(connectedCells, /* collapseClustersInList */ true, 
		       /* placeBoundaryCells */ true);
    numClusters++;
    //    if (numClusters == 1000) break;
  } DESIGN_END_FOR;

  _STEP_END("End default clustering");

  return rtv;
}

/***************************************************
  TOP LEVEL FUNCTION TO DO TIMING DRIVEN CLUSTERING
***************************************************/
bool
Design::DesignDoTimingDrivenCluster(HyperGraph &myGraph)
{
  Path *thisPath;
  Cell *cellPtr;
  vector<Cell *> clusterList;
  uint pathIdx;

  _STEP_BEGIN("Path-based clustering");

  DESIGN_FOR_ALL_PATHS((*this), pathIdx, thisPath) {
    vector<Cell *> &pathCells = (*thisPath).PathGetCells();
    clusterList.clear();
    VECTOR_FOR_ALL_ELEMS(pathCells, Cell*, cellPtr) {
      if ((*cellPtr).CellIsHidden()) continue;
      if ((*cellPtr).CellIsSequential()) continue;
      clusterList.push_back(cellPtr);
    } END_FOR;
    if (clusterList.empty()) continue;
    DesignClusterCells(clusterList, /* collapseClustersInList */ true, 
		       /* placeBoundaryCells */ true);
  } DESIGN_END_FOR;

  _STEP_END("Path-based clustering");
}

/***************************************************
  TOP LEVEL FUNCTION TO DO FIRST CHOICE CLUSTERING
***************************************************/
bool
Design::DesignDoFirstChoiceClustering(HyperGraph &myGraph)
{
  
}

/***************************************************
  FUNCTION TO TEST THE CLUSTER CREATION AND 
  DESTRUCTION AND PLACEMENT OF CELLS AROUND
***************************************************/
bool
Design::DesignDoClusterTest()
{
  vector<Cell *> listOfCells;
  Cell *clusterCell;
  Cell *cell1 = DesignGetNode("o90");  listOfCells.push_back(cell1);
  Cell *cell2 = DesignGetNode("o89");  listOfCells.push_back(cell2);
  Cell *cell3 = DesignGetNode("o2138");  listOfCells.push_back(cell3);
  Cell *cell4 = DesignGetNode("o1778");   listOfCells.push_back(cell4);
  Cell *cell5 = DesignGetNode("o91");   listOfCells.push_back(cell5);
  Cell *cell6 = DesignGetNode("o0"); listOfCells.push_back(cell6);
  Cell *cell7 = DesignGetNode("o1986"); listOfCells.push_back(cell7);
  Cell *cell8 = DesignGetNode("o1987"); listOfCells.push_back(cell8);
  Cell *cell9 = DesignGetNode("o92"); listOfCells.push_back(cell9);

  DesignClusterCells(listOfCells, /* collapseClustersInList */ true, 
		     /* placeBoundaryCells */ true);
  
  clusterCell = DesignGetNode("c1");
  
  (*clusterCell).CellSetXpos(100000);
  (*clusterCell).CellSetYpos(100000);

  /* Dissolve the cluster */
  DesignUnclusterCell(clusterCell, false);
}
