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
  string clusterName;
  unsigned int resultWidth, resultHeight;
  unsigned int collectiveHeight, collectiveWidth;
  unsigned int maxHeight, maxWidth;
  unsigned int count;
  
  collectiveHeight = 0;
  collectiveWidth = 0;
  maxHeight = 0;
  maxWidth = 0;

  VECTOR_FOR_ALL_ELEMS(listOfCells, Cell*, cellPtr) {
    Cell &thisCell = (*cellPtr);
    thisCell.CellSetIsCluster(true);
    collectiveHeight += thisCell.CellGetHeight();
    collectiveWidth += thisCell.CellGetWidth();
  } END_FOR;
  
  getWidthAndHeight(listOfCells, aspectRatio, resultHeight,
		    resultWidth);
  clusterName = "**Cluster**" + getStrFromInt(clusterNumber++);
  //cout << "Cluster cell formed: " << clusterName << " " << listOfCells.size() << " Height: " << resultHeight << " Width: " << resultWidth << endl;
  cellPtr = new Cell(resultHeight, resultWidth, clusterName);
  (*this).DesignAddOneCellToDesignDB(cellPtr);

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

