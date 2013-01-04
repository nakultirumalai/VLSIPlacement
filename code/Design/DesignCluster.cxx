# include <Design.h>

void 
Design::DesignClusterCells(vector<Cell*>& CellSet)
{
  
}


void
Design::DesignCollapseCluster(Cell& MasterCell)
{

}


/*******************************************************************************
 TOP LEVEL FUNCTION FOR CLUSTERING. SEVERAL CLUSTERING STRATEGIES CAN BE 
 EXPLORED.
*******************************************************************************/
void 
Design::DesignClusterCells(void) 
{
  Cell *cellPtr;
  map<Cell *, bool>clusteredCells;
  
  string cellName;

  /* Top level iteration */
  DESIGN_FOR_ALL_STD_CELLS((*this), cellName, cellPtr) {
    
  } DESIGN_END_FOR;

  /* Iterate over all the standard cells */
  
  /* Examine the connectivity and perform clustering.
     Connectivity examination can be done using 
}
