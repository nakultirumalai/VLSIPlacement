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
  double fdpTime, fdpNetlistBuildTime;
  uint siteWidth, rowHeight;
  string cellName;
  uint siteNum, rowNum;
  uint numSites, numRows, numClusters;
  uint iterCount, abortLimit;
  uint maxx, maxy;
  ulong totalHPWL;
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
  totalHPWL = DesignGetHPWL();
  //  cout << "After constructive placement : X-HPWL: " << DesignGetXHPWL() 
  //       << " Y-HPWL: " << DesignGetYHPWL() 
  //       << " HPWL: " << totalHPWL << endl;
  DesignEnv.EnvSetHPWLAfterConstructive(totalHPWL);
  string DesignName = DesignEnv.EnvGetDesignName();
  HyperGraph &myGraph = DesignGetGraph();
  DesignDumpNetDegreeProfile((DesignName + "_NetsPreFD.csv"));
  if (DesignEnv.EnvGetUseFDPlacer()) {
    iterCount = DesignEnv.EnvGetIterCount();
    abortLimit = DesignEnv.EnvGetAbortLimit();
    iterCount = 100;
    FDPTopLevel((*this), clusterCells, numRows, numSites, rowHeight, siteWidth, 
		iterCount, abortLimit);
  }
  DesignDumpNetDegreeProfile((DesignName + "_NetsPostFD.csv"));
  /* STEP : FINISH */
  DesignComputeHPWL();
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLAfterFDPlacement(totalHPWL);
  //  cout << "After top level solver : X-HPWL: " << DesignGetXHPWL() 
  //       << " Y-HPWL: " << DesignGetYHPWL() 
  //       << " HPWL: " << DesignGetHPWL() << endl;
}
