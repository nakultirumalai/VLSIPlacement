# include <Design.h>

inline void
getSitesAndBlocksForDesign(uint maxx, uint maxy, double averageClusterWidth, 
			   double averageClusterHeight, uint &numSites, 
			   uint &numRows)
{
  numSites = floor(maxx / averageClusterWidth);
  numRows = floor(maxy / averageClusterHeight);
  
  /* Add two rows and two sites for the ports */
  //  numSites = 2;
  //  numRows += 2;
}

/* A force-directed solver at the top level. The force 
   directed solver is used instead of an analytical solver
   because the number of cells in the top level is small */
void
Design::DesignSolveForAllCellsForceDirected(void)
{
  Cell *clusterCellPtr;
  vector<Cell *> clusterCells, fixedCells;
  uint averageClusterWidth, averageClusterHeight;
  string cellName;
  uint siteNum, rowNum;
  uint numSites, numRows;
  uint maxx, maxy;
  
  /* STEP : CREATE PLACEABLE BLOCKS IN THE DESIGN */
  DesignGetBoundingBox(maxx, maxy);
  averageClusterWidth = (uint)DesignGetAverageClusterCellWidth();
  averageClusterHeight = (uint)DesignGetAverageClusterCellHeight();
  getSitesAndBlocksForDesign(maxx, maxy, averageClusterWidth, 
			     averageClusterHeight, numSites, numRows);

  /* STEP : PLACE THE CELLS IN A CONSTRUCTIVE FASHION 
            INTO THE BLOCKS OF THE DESIGN */
  siteNum = 1;
  rowNum = 1;
  DESIGN_FOR_ALL_CLUSTERS((*this), cellName, clusterCellPtr) {
    (*clusterCellPtr).CellSetXpos((siteNum-1) * averageClusterWidth); siteNum++;
    (*clusterCellPtr).CellSetYpos((rowNum -1) * averageClusterHeight); 
    if (siteNum == (numSites - 1)) {
      siteNum = 1;
      rowNum++;
    }
    clusterCells.push_back(clusterCellPtr);
  } DESIGN_END_FOR;

  /* STEP : EXECUTE THE FORCE DIRECTED SOLVER TO MINIMIZE 
            THE WIRELENGTH */
  HyperGraph &myGraph = DesignGetGraph();
  FDPTopLevel(fixedCells, clusterCells, (int)numRows, (int)numSites,
	      (int)averageClusterHeight, (int)averageClusterWidth, 
	      true, myGraph);

  /* STEP : FINISH */
}
