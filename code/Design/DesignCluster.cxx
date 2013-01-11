# include <Design.h>
# include <DesignIter.h>

static unsigned int clusterNumber;

void
Design::DesignCollapseCluster(Cell& MasterCell)
{

}


void
getWidthAndHeight(vector<void*>listOfCells, double aspectRatio,
		  unsigned int& resultHeight, unsigned int& resultWidth)
{
  Cell* cellPtr;
  unsigned int minWidth, minHeight;
  unsigned int totalWidth, totalHeight;
  unsigned int totalArea;
  
  /* 
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
  */
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

Cell*
Design::DesignClusterSpecifiedCells(vector<void *> listOfCells, double aspectRatio)
{
  Cell *cellPtr;
  Net *netPtr;
  Pin *pinPtr;
  string clusterName;
  unsigned int resultWidth, resultHeight;
  unsigned int collectiveHeight, collectiveWidth;
  unsigned int maxHeight, maxWidth;
  unsigned int count;
  map<Cell*, bool> cellHash;
  map<Net*, bool> netHash;
  
  collectiveHeight = 0;
  collectiveWidth = 0;
  maxHeight = 0;
  maxWidth = 0;

  clusterName = "**Cluster**" + getStrFromInt(clusterNumber++);
  cellPtr = new Cell();
  (*cellPtr).CellSetName(clusterName);
  (*cellPtr).CellSetIsCluster(true);
  (*this).DesignAddOneCellToDesignDB(cellPtr);

  VECTOR_FOR_ALL_ELEMS(listOfCells, Cell*, cellPtr) {
    Cell &thisCell = (*cellPtr);
    thisCell.CellSetIsClusterChild(true);
    collectiveHeight += thisCell.CellGetHeight();
    collectiveWidth += thisCell.CellGetWidth();
    (*cellPtr).CellAddChildCell(thisCell);
    cellHash[cellPtr] = true;
  } END_FOR;
  
  getWidthAndHeight(listOfCells, aspectRatio, resultHeight,
		    resultWidth);

  (*cellPtr).CellSetHeight(resultHeight);
  (*cellPtr).CellSetWidth(resultWidth);

  /* Get the graph corresponding to the design and commit the cluster 
     to the graph to effect connectivity changes */
  HyperGraph& myGraph = DesignGetGraph();
  myGraph.HyperGraphClusterCells(listOfCells, (void *)cellPtr);

  /* Hide nets that are under the cluster */
  /* What the hell is the complexity of this? 
     Measure it! We need to make this more efficient */
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

  return (cellPtr);
}

/*******************************************************************************
 TOP LEVEL FUNCTION FOR CLUSTERING. SEVERAL CLUSTERING STRATEGIES CAN BE 
 EXPLORED.
*******************************************************************************/
void 
Design::DesignClusterCells(HyperGraph& myGraph) 
{
  Cell *cellPtr, *connectedCellPtr;
  map<Cell *, bool>clusteredCells;
  vector<void *>connectedCells;
  string cellName;
  Design& myDesign = (*this);
  unsigned int numClustered;

  /* Top level iteration */
  numClustered = 0;
  DESIGN_FOR_ALL_STD_CELLS(myDesign, cellName, cellPtr) {
    numClustered++;
    connectedCells = myGraph.HyperGraphGetConnectedCells((void *)cellPtr);
    //cout << "CELL Name " << (*cellPtr).CellGetName() << " " << connectedCells.size() << endl;
    connectedCells.push_back((void *)cellPtr);
    (*this).DesignClusterSpecifiedCells(connectedCells, (double)1.0);
  } DESIGN_END_FOR;
  
  cout << "Number of clustered cells is " << numClustered << endl;
}

