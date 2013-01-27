# include <Design.h>
# include <DesignIter.h>

static unsigned int clusterNumber;

/*********************************************************************
   If a cell is clustered then it will be collapsed by this routine
*********************************************************************/
void
Design::DesignCollapseCluster(Cell& MasterCell)
{

}

void 
netHidingConsistencyCheck(vector<void *>affectedNets, 
			  vector<void *>listOfCells) 
{
  Cell* cellPtr;
  Pin *pinPtr;
  Net *netPtr;
  map<Net*, bool> netHash;
  
  VECTOR_FOR_ALL_ELEMS(listOfCells, Cell*, cellPtr) {
    Cell &cellObj = (*cellPtr);
    CELL_FOR_ALL_NETS(cellObj, PIN_DIR_ALL, netPtr) {
      if (netHash.find(netPtr) != netHash.end()) {
	continue;
      } else {
	netHash[netPtr] = true;
      }
      bool markNet = true;
      Net &netObj = (*netPtr);
      NET_FOR_ALL_PINS(netObj, pinPtr) {
	Cell& parentCell = (*pinPtr).PinGetParentCell();
	if (parentCell.CellIsClusterChild()) {
	  markNet = false;
	  break;
	}
      } NET_END_FOR;
      if (markNet == true) {
	(*netPtr).NetSetIsUnderCluster(true);
      } else {
	netHash.erase(netPtr);
      }
    } CELL_END_FOR;
  } END_FOR;

  bool success = true;
  VECTOR_FOR_ALL_ELEMS(affectedNets, Net*, netPtr) {
    if (netHash.find(netPtr) != netHash.end()) {
      continue;
    }
    success = false;
    break;
  } END_FOR;
  if (success == false) {
    _ASSERT_TRUE("Consistency check for hiding nets failed");
  }
}


/*********************************************************************
   Hide the nets that connect to only those cells which are inside a 
   cluster
*********************************************************************/
void
Design::DesignHideNets(vector<void *> affectedNets, vector<void *> listOfCells)
{
  Net *netPtr;
  
  VECTOR_FOR_ALL_ELEMS(affectedNets, Net*, netPtr) {
    (*netPtr).NetSetIsUnderCluster(true);
  } END_FOR;

  /* Perform a consistency check when enabled */
  if (performNetHidingConsistency) {
    netHidingConsistencyCheck(affectedNets, listOfCells);
  }
}

/*********************************************************************
   Hide the nets that connect to only those cells which are inside a 
   cluster
*********************************************************************/
void
Design::DesignPropagateTerminals(Cell* fromCell, Cell* toCell)
{
  Pin *PinPtr;
  char dir;

  CELL_FOR_ALL_PINS((*fromCell), dir, PinPtr) {
    Pin *pinPtr = new Pin((*PinPtr).PinGetId(), (*toCell), 
			  (*PinPtr).PinGetName());
    PinSetOriginalPin(PinPtr, pinPtr);
    (*pinPtr).PinSetDirection(dir);
    Net& connectedNet = (*PinPtr).Disconnect();
    (*pinPtr).Connect(connectedNet);
  } CELL_END_FOR;
}

/*********************************************************************
   Work back from the following equations:
   aspectRatio = width / height;
   area = width * height;

   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   Height computation:
   area = width * height;
   area = (height ^ 2) * aspectRatio;
   height = sqrt(area / aspectRatio);
     
   However, height = minHeight;
     
   Therefore, height = max(minHeight, round(sqrt(area/aspectRatio)));
     
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   Weight computation:
   area = width * height;
   area = (width ^ 2) / aspectRatio;
   width = sqrt(area * aspectRatio);

   However, width = minWidth;
     
   Therefore, width = max(minWidth, round(sqrt(area * aspectRatio)));
*********************************************************************/
void
getWidthAndHeight(vector<void*>listOfCells, double aspectRatio,
		  unsigned int totalHeight, unsigned int totalWidth,
		  unsigned int totalArea, unsigned int minHeight,
		  unsigned int minWidth,
		  unsigned int& resultHeight, unsigned int& resultWidth)
{
  if (aspectRatio != 0) {
    resultWidth = (unsigned int)ceil(sqrt(totalArea * aspectRatio));
    if (resultWidth < minWidth) {
      resultWidth = minWidth;
    }
    resultHeight = (unsigned int)ceil(sqrt(totalArea / aspectRatio));
    if (resultHeight < minHeight) {
      resultHeight = minHeight;
    }
  } else {
    resultWidth = totalWidth;
    resultHeight = minHeight;
  }
}

/*********************************************************************
   Given a vector of vector of cells, iterates over each cell list,
   clusters the cells specified in the list with the given aspect
   ratio. TODO: Aspect ratio variation will be done later.
   Returns a list of clustered cells.
   CAUTION: The list of cells specified must be unique in each 
            set and across sets. No two sets can have the same 
            cell under the list of cells to be clustered.
*********************************************************************/
vector<Cell*>
Design::DesignClusterSpecifiedCells(vector<vector<void * > >listOfCells, 
				    double aspectRatio)
{
  HyperGraph &myGraph = DesignGetGraph();
  Cell *cellPtr, *newCell;
  vector<void *> uniqueList;
  vector<unsigned int> cellIndices;
  vector<void *> cellList;
  vector<Cell *> returnList;
  vector<void *> affectedNets;
  map<Cell *, bool> cellHash;
  string clusterName;
  unsigned int resultWidth, resultHeight;
  unsigned int collectiveHeight, collectiveWidth;
  unsigned int maxHeight, maxWidth, count;
  unsigned int stopCount;
  unsigned int minHeight, minWidth;
  unsigned int totalHeight, totalWidth, totalArea;
  unsigned int cellHeight, cellWidth, cellArea;
  unsigned int cellIndex;
  unsigned int numClusters;
  
  numClusters=0;
  VECTOR_FOR_ALL_ELEMS(listOfCells, vector<void *>, cellList) {
    _STEP_BEGIN("Clustering cell set");
    collectiveHeight=0; collectiveWidth=0;
    maxHeight=0; maxWidth=0; 

    /* Build the name of the cluster cell */
    clusterName = CLUSTER_NAME_PREFIX + getStrFromInt(clusterNumber++);

    /* Create the cluster cell */
    newCell = new Cell();
    (*newCell).CellSetName(clusterName);
    (*newCell).CellSetIsCluster(true);
    (*newCell).CellIncrementClusterLevel();
    /* Flag the cell as clustered */
    CellSetIsClustered(newCell);

    /* Add the cell to the design database */
    (*this).DesignAddOneCellToDesignDB(newCell);

    /* Initialize all the variables */
    minHeight = 0; minWidth = 0; totalArea = 0;
    totalHeight = 0; totalWidth = 0;
    
    /* Collect data from the set of cells provided */
    VECTOR_FOR_ALL_ELEMS(cellList, Cell*, cellPtr) {
      Cell &thisCell = (*cellPtr);
      cellHeight = thisCell.CellGetHeight();
      cellWidth = thisCell.CellGetWidth();
      cellArea = thisCell.CellGetArea();
      if (minHeight <= cellHeight) {
	minHeight = cellHeight;
      }
      if (minWidth <= cellWidth) {
	minWidth = cellWidth;
      }
      totalArea += cellArea;
      totalHeight += cellHeight;
      totalWidth += cellWidth;
      collectiveHeight += cellHeight;
      collectiveWidth += cellWidth;
      
      /* Mark the cell as a child of the cluster cell formed */
      thisCell.CellSetIsClusterChild(true);
      /* Add the child cell to the new cluster cell */
      (*newCell).CellAddChildCell(thisCell);

      cellIndex = myGraph.HyperGraphGetCellIndex(cellPtr);
      cellIndices.push_back(cellIndex);

      /* Propagate pin connections to the newly clustered cell */
      DesignPropagateTerminals(newCell, cellPtr);

      cellHash[cellPtr] = true;
    } END_FOR;
  
    /* Get the height and width of the resulting cell */
    getWidthAndHeight(cellList, aspectRatio, totalHeight,
		      totalWidth, totalArea, minHeight, minWidth,
		      resultHeight, resultWidth);
    
    (*newCell).CellSetHeight(resultHeight);
    (*newCell).CellSetWidth(resultWidth);
    /* Get the graph corresponding to the design and commit the 
       cluster to the graph to effect connectivity changes.
       Obtain the list of affected nets which should be hidden 
       as they are only connected to cells within the cluster 
    */
    myGraph.HyperGraphClusterNodes(cellIndices, (void *)newCell,
				   affectedNets);

    /* Hide nets that are under the cluster */
    DesignHideNets(affectedNets, cellList);

    /* Add the clustered cell to the return list */
    returnList.push_back(newCell);

    _STEP_END("Clustering each cell set");
    numClusters++;
  } END_FOR;
  cout << "Clustered " << numClusters << " clusters successfully" << endl;

  return (returnList);
}

/*********************************************************************
   Given a cell pointer, uses the hypergraph to get a list of 
   connected cells.
*********************************************************************/
vector<void*>
DesignGetConnectedCells(HyperGraph &myGraph, Cell *cellPtr) 
{
  vector<void*> connectedCells;
  vector<void*> returnCells;
  map<Cell*, bool> addedCells;
  Cell *connectedCellPtr;
  
  connectedCells = myGraph.HyperGraphGetConnectedCells((void *)cellPtr);
  VECTOR_FOR_ALL_ELEMS(connectedCells, Cell*, connectedCellPtr) {
    Cell &thisCell = *connectedCellPtr;
    if (addedCells.find(connectedCellPtr) != addedCells.end()) {
      _ASSERT_TRUE("DUPLICATES FOUND!!!");
      exit(0);
      continue;
    }
    returnCells.push_back((void *)connectedCellPtr);
  } END_FOR;

  return (returnCells);
}

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
  bool rtv;

  _STEP_BEGIN("Begin default clustering");

  rtv = true;
  DESIGN_FOR_ALL_STD_CELLS_FILT(myDesign, cellName, cellPtr, CellIsClustered) {
    vector<void *> connectedCells = DesignGetConnectedCells(myGraph, cellPtr);
    connectedCells.push_back(cellPtr);
    VECTOR_FOR_ALL_ELEMS(connectedCells, Cell*, thisCellPtr) {
      CellSetIsClustered(thisCellPtr);
    } END_FOR;
    cellsToCluster.push_back(connectedCells);
  } DESIGN_END_FOR;
  DesignClusterSpecifiedCells(cellsToCluster, (double)1.0);

  _STEP_END("End default clustering");

  return rtv;
}

/* Top level function to perform first choice clustering */
bool
Design::DesignDoFCCluster(HyperGraph &myGraph) 
{
  bool rtv;

  rtv = true;
  
  return rtv;
}

/* Top level function to perform net clustering */
bool
Design::DesignDoNetCluster(HyperGraph &myGraph) 
{
  bool rtv;

  rtv = true;
  
  return rtv;
}

/* Top level function to perform edge separability based
   clustering */
bool
Design::DesignDoESCCluster(HyperGraph &myGraph) 
{
  bool rtv;

  rtv = true;
  
  return rtv;
}

/*******************************************************************************
 TOP LEVEL FUNCTION FOR CLUSTERING. SEVERAL CLUSTERING STRATEGIES CAN BE 
 EXPLORED.
*******************************************************************************/
void 
Design::DesignClusterCells(HyperGraph& myGraph, clusteringType clustType) 
{
  bool clustResult;

  clustResult = true;

  switch (clustType) {
  case DEFAULT_CLUSTER:
    clustResult = DesignDoDefaultCluster(myGraph);
    break;
  case FC_CLUSTER:
    clustResult = DesignDoFCCluster(myGraph);
    cout << "First choice clustering not supported." << endl;
    break;
  case NET_CLUSTER:
    clustResult = DesignDoNetCluster(myGraph);
    cout << "Net clustering not supported." << endl;
    break;
  case ESC_CLUSTER:
    clustResult = DesignDoESCCluster(myGraph);
    cout << "Edge separability based clustering not supported." << endl;
    break;
  default: cout << "Unknown clustering type specified. Not clustering circuit" << endl;
  };  
  
  if (!clustResult) {
    cout << "Clustering failed!!" << endl;
  }
}


