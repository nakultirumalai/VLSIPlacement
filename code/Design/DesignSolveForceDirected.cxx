# include <Design.h>

/* A force-directed solver at the top level. The force 
   directed solver is used instead of an analytical solver
   because the number of cells in the top level is small */
void
Design::DesignSolveForAllCellsForceDirected(void)
{
  Cell *clusterCellPtr;
  vector<Cell *> clusterCells, fixedCells;
  uint averageClusterWidth, averageClusterHeight;
  double clusterXpos, clusterYpos;
  int siteWidth, rowHeight;
  string cellName;
  uint siteNum, rowNum;
  uint numSites, numRows, numClusters;
  uint maxx, maxy;
  Env &DesignEnv = DesignGetEnv();
  
  /* STEP : CREATE PLACEABLE BLOCKS IN THE DESIGN */
  DesignGetBoundingBox(maxx, maxy);
  averageClusterWidth = (uint)DesignGetAverageClusterCellWidth();
  averageClusterHeight = (uint)DesignGetAverageClusterCellHeight();
  numClusters = DesignGetNumClusters();
  numRows = floor(((double)maxy) / averageClusterHeight);
  numSites = ceil(((double)numClusters) / numRows);
  siteWidth = floor(((double)maxx) / numSites);
  rowHeight = averageClusterHeight;

  /* STEP : PLACE THE CELLS IN A CONSTRUCTIVE FASHION 
            INTO THE BLOCKS OF THE DESIGN */
  siteNum = 0;
  rowNum = 0;
  uint count = 0;
  DESIGN_FOR_ALL_CLUSTERS((*this), cellName, clusterCellPtr) {
    clusterXpos = siteNum * siteWidth;
    clusterYpos = rowNum * rowHeight;
    (*clusterCellPtr).CellSetXpos(clusterXpos); 
    siteNum++;
    (*clusterCellPtr).CellSetYpos(clusterYpos); 
    if (siteNum == numSites) {
      siteNum = 0;
      rowNum++;
    }
    clusterCells.push_back(clusterCellPtr);
    count++;
  } DESIGN_END_FOR;

  /* STEP : EXECUTE THE FORCE DIRECTED SOLVER TO MINIMIZE 
            THE WIRELENGTH */
  DesignComputeHPWL();
  cout << "PRE-GLOBAL Placement : X-HPWL: " << DesignGetXHPWL() 
       << " Y-HPWL: " << DesignGetYHPWL() 
       << " TOTAL: " << DesignGetHPWL() << endl;

  HyperGraph &myGraph = DesignGetGraph();
  if (DesignEnv.EnvGetUseFDPlacer()) {
    FDPTopLevel((*this), clusterCells, numRows, numSites, rowHeight, siteWidth, 
		1000, (clusterCells.size()));
  }

  /* STEP : FINISH */
  DesignComputeHPWL();
  cout << "Quality : X-HPWL: " << DesignGetXHPWL() 
       << " Y-HPWL: " << DesignGetYHPWL() 
       << " TOTAL: " << DesignGetHPWL() << endl;
  //  exit(0);
}
