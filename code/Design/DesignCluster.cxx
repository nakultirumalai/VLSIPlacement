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

/*********************************************************************
   Hide the nets that connect to only those cells which are inside a 
   cluster
*********************************************************************/
void
Design::DesignHideNets(vector<void *> listOfCells)
{
  Cell* cellPtr;
  Pin *pinPtr;
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
      }
    } CELL_END_FOR;
  } END_FOR;
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
		  unsigned int& resultHeight, unsigned int& resultWidth)
{
  Cell* cellPtr;
  unsigned int minWidth, minHeight;
  unsigned int totalWidth, totalHeight;
  unsigned int totalArea;
  
  minWidth = 0;
  minHeight = 0;
  totalHeight = 0;
  totalWidth = 0;
  totalArea = 0;

  VECTOR_FOR_ALL_ELEMS(listOfCells, Cell*, cellPtr) {
    Cell &thisCell = (*cellPtr);
    if (minHeight <= thisCell.CellGetHeight()) {
      minHeight = thisCell.CellGetHeight();
    }
    if (minWidth <= thisCell.CellGetWidth()) {
      minWidth = thisCell.CellGetWidth();
    }
    totalArea += thisCell.CellGetArea();
    totalHeight += thisCell.CellGetHeight();
    totalWidth += thisCell.CellGetWidth();
  } END_FOR;

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
*********************************************************************/
vector<Cell*>
Design::DesignClusterSpecifiedCells(vector<vector<void * > >listOfCells, double aspectRatio)
{
  Cell *cellPtr, *newCell;
  string clusterName;
  unsigned int resultWidth, resultHeight;
  unsigned int collectiveHeight, collectiveWidth;
  unsigned int maxHeight, maxWidth, count;
  map<Cell*, bool> cellHash;
  vector<void *> uniqueList;
  vector<void *> cellList;
  vector<Cell *> returnList;
  HyperGraph &myGraph = DesignGetGraph();
  
  VECTOR_FOR_ALL_ELEMS(listOfCells, vector<void *>, cellList) {
    _STEP_BEGIN("Clustering each cell set");
    collectiveHeight = 0; collectiveWidth = 0;
    maxHeight = 0; maxWidth = 0;
    clusterName = CLUSTER_NAME_PREFIX + getStrFromInt(clusterNumber++);

    newCell = new Cell();
    (*newCell).CellSetName(clusterName);
    (*newCell).CellSetIsCluster(true);
    (*this).DesignAddOneCellToDesignDB(newCell);

    VECTOR_FOR_ALL_ELEMS(cellList, Cell*, cellPtr) {
      if (cellHash.find(cellPtr) == cellHash.end()) {
	cellHash[cellPtr] = true;
      } else {
	continue;
      }
      Cell &thisCell = (*cellPtr);
      thisCell.CellSetIsClusterChild(true);
      collectiveHeight += thisCell.CellGetHeight();
      collectiveWidth += thisCell.CellGetWidth();
      (*newCell).CellAddChildCell(thisCell);
      uniqueList.push_back((void*)cellPtr);
    } END_FOR;
  
    getWidthAndHeight(uniqueList, aspectRatio, resultHeight, resultWidth);

    (*newCell).CellSetHeight(resultHeight);
    (*newCell).CellSetWidth(resultWidth);
    /* Get the graph corresponding to the design and commit the cluster 
       to the graph to effect connectivity changes */
    myGraph.HyperGraphClusterCells(uniqueList, (void *)newCell);

    /* Hide nets that are under the cluster */
    /* What the hell is the complexity of this? 
       Measure it! We need to make this more efficient */
    DesignHideNets(uniqueList);
    returnList.push_back(newCell);
    _STEP_END("Clustering each cell set");
  } END_FOR;

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
  Cell *cellPtr;
  vector<vector<void * > >cellsToCluster;
  vector<void *> connectedCells;
  string cellName;
  unsigned int numClustered;
  bool rtv;

  _STEP_BEGIN("Begin clustering");

  rtv = true;
  DESIGN_FOR_ALL_STD_CELLS_FILT(myDesign, cellName, cellPtr, CellIsClustered) {
    connectedCells = DesignGetConnectedCells(myGraph, cellPtr);
    connectedCells.push_back(cellPtr);
    cellsToCluster.push_back(connectedCells);
  } DESIGN_END_FOR;
  DesignClusterSpecifiedCells(cellsToCluster, (double)1.0);

  _STEP_END("End clustering");

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


