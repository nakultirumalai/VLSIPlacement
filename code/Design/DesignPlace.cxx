# include <Design.h>

bool
Design::DesignPrintClusterParams(double clusterRatio, double maxArea, double maxWidth, 
				    double clusterBoundPenalty, uint clusterNumRows)
{
  bool rtv;

  rtv = true;
  /* Print all the clustering parameters first */
  cout << "CLUSTER : Clustering ratio : " << clusterRatio << endl;
  cout << "CLUSTER : Maximum Area     : " << maxArea << endl;
  cout << "CLUSTER : Maximum Width    : " << maxWidth << endl;
  cout << "CLUSTER : Bound penalty    : " << clusterBoundPenalty << endl;
  if (clusterNumRows == 0) {
    cout << "CLUSTER : Number of rows   : " << clusterNumRows << " : SQUARE CLUSTERS " << endl;
  } else {
    cout << "CLUSTER : Number of rows   : " << clusterNumRows << endl;
  }

  return (rtv);
}

void
Design::DesignCoarsenNetlist(void)
{
  Cell *cellPtr;
  Pin *pinPtr;
  Env &DesignEnv = this->DesignEnv;
  double clusterRatio, maxArea, maxWidth;
  double inputMaxWidth, inputMaxArea;
  double clusterBoundPenalty, clusteringTime;
  double clusteringStartTime, achievedClusteringRatio;
  uint clusterNumRows;
  uint numPinsBefore, numPinsAfter;
  uint maxx, maxy;
  string DesignName;
  EnvClusterType clusterType;
  HyperGraph &myGraph = DesignGetGraph();
  clusterType = DesignEnv.EnvGetClusterType();
  clusterRatio = DesignEnv.EnvGetClusteringRatio();
  maxArea = DesignEnv.EnvGetClusterMaxArea();
  maxWidth = DesignEnv.EnvGetClusterMaxWidth();
  clusterBoundPenalty = DesignEnv.EnvGetClusterBoundPenalty();
  clusterNumRows = DesignEnv.EnvGetClusterNumRows();
  DesignName = DesignEnv.EnvGetDesignName();
  DesignPrintClusterParams(clusterRatio, maxArea, maxWidth, 
			   clusterBoundPenalty, clusterNumRows);
  DesignGetBoundingBox(maxx, maxy);
  inputMaxWidth = floor(maxWidth * maxx);
  inputMaxArea = maxArea * ((double)maxx * maxy);

  achievedClusteringRatio = DesignGetNumCells();
  switch (clusterType) {
  case ENV_NO_CLUSTERING: 
    cout << "Clustering: No clustering " << endl;
    break;
  case ENV_LARGE_CLUSTERING:
    cout << "Clustering: Performing k-way clustering " << endl;
    DesignDoKWayClustering(myGraph, true, clusteringTime);
    DesignEnv.EnvRecordClusteringTime(clusteringTime);
    break;
  case ENV_FIRST_CHOICE_CLUSTERING:
    cout << "Clustering: First choice clustering. Not ready yet " << endl;
    break;
  case ENV_BEST_CHOICE_CLUSTERING:
    cout << "Clustering: Best choice clustering " << endl << endl;
    if (clusterNumRows > 0) {
      DesignDoBestChoiceClusteringCstr(myGraph, inputMaxArea, inputMaxWidth,
				       true, clusterRatio);
      //      DesignDoBestChoiceClusteringPenalty(myGraph, clusterBoundPenalty,
      //					  true, clusterRatio);
    } else {
      DesignDoBestChoiceClustering(myGraph, clusterRatio);
    }
    //    (void)DesignValidateClusterCstrs(clusterRatio, maxArea, maxWidth, clusterNumRows);
    /* Rebuild hypergraph after coarsening */
    DesignRebuildGraph();
    cout << "Finished coarsening" << endl;
    break;
  case ENV_NET_CLUSTERING:
    cout << "Clustering: Net cluster " << endl;
    DesignDoNetCluster(myGraph, maxArea, maxWidth);
    break;
  defaut:
    cout << "Clustering: Clustering strategy unknown. Not clustering netlist " << endl;
    break;
  }

  DesignEnv.EnvRecordClusteringTime(clusteringTime);
  achievedClusteringRatio = DesignGetNumTopCells() / achievedClusteringRatio;
  cout << "CLUSTERING DONE. CPU: " << clusteringTime << "s" << endl;
  cout << "CLUSTERING RATIO ACHIEVED: " << achievedClusteringRatio << endl;
  cout << "Dumping cluster information: PRE-TOP LEVEL PLACEMENT" << endl;
  DesignDumpClusterInfo((DesignName + ".csv"));
  //  DesignWriteBookShelfOutput((*this), "usb_sie_clust");
}

void
Design::DesignCollapseClusters(void)
{
  Cell *cellPtr;
  Cluster *clusterOfCell;
  vector<Cell*> cellsToSolve;
  Env &DesignEnv = this->DesignEnv;
  string DesignName, cellName;
  string clustDir;
  HyperGraph &myGraph = DesignGetGraph();

  map<string, Cell*> DesignClusterMap = DesignGetClusters();
  map<string, Cell*>::iterator mapIter;
  for (mapIter = DesignClusterMap.begin(); mapIter != DesignClusterMap.end(); mapIter++) {
    cellName = mapIter->first;
    cellPtr = mapIter->second;
    DesignUnclusterLargeCluster(cellPtr, /* noDissolve */false);
  }
}

void
Design::DesignRunInternalPlacer(EnvSolverType solverType)
{
  EnvNetModel netModel;
  Env &DesignEnv = this->DesignEnv;
  
  netModel = DesignEnv.EnvGetNetModel();
  
  switch (solverType) {
  case ENV_SOLVER_QUADRATIC_MOSEK:
    DesignSolveForAllCellsMosekIter();
    break;
  case ENV_SOLVER_QUADRATICXY_MOSEK:
    //    DesignSolveForAllCellsMosekIterSolveBoth();
    break;
  case ENV_SOLVER_QUADRATIC_CONJ_GRAD:
    //DesignSolveForAllCellsConjGradWnLibIter();
    DesignSolveForAllCellsConjGradIter();
    break;
  case ENV_SOLVER_NON_LINEAR:
    if (netModel == ENV_CLIQUE_MODEL) {
    } else if (netModel == ENV_STAR_MODEL) {
    } else if (netModel == ENV_HYBRID_MODEL) {
    }
    cout << "Solver not ready yet!" << endl;
    break;
  case ENV_SOLVER_NON_LINEAR_CONJ_GRAD:
    if (netModel == ENV_CLIQUE_MODEL) {
    } else if (netModel == ENV_STAR_MODEL) {
    } else if (netModel == ENV_HYBRID_MODEL) {
    }
    cout << "Solver not ready yet!" << endl;
    break;
  case ENV_SOLVER_FORCE_DIRECTED:
    DesignSolveForAllCellsForceDirected();
    break;
  default: cout << "Unknown solver type provided" << endl;
  };
}

void
Design::DesignRunExternalPlacer(EnvGlobalPlacerType globalPlacerType) 
{
  double globalPlacementTime;
  int statusCode;
  string placerPath, placerLogFile;
  string DesignName;
  string clusterDesignName, clustDir;

  Env &topEnv = DesignGetEnv();
  DesignName = topEnv.EnvGetDesignName();
  clusterDesignName = DesignName + "_clust";
  clustDir = "." + DesignName + "_clust";
  
  /* Write out the current clustered netlist into the folder .designName_clust */
  DesignWriteCurrentNetlist(clustDir, clusterDesignName);
  if (globalPlacerType == ENV_NTUPLACE_GP) {
    /* Run NTUPlace for global placement. The variable 
       NTUPLACE_FULL_PATH has to be set for NTUPlace to 
       run successfully */
    statusCode = DesignRunNTUPlace("." + clusterDesignName, clusterDesignName, 
				   globalPlacementTime, true, true, false, placerLogFile);
    topEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
    cout << "Completed running placer: NTUPlace ... Returned status: " 
	 << statusCode << endl;
  } else if (globalPlacerType == ENV_FAST_PLACE_GP) {
    /* Run FastPlace for global placement. The variable 
       FASTPLACE_FULL_PATH has to be set for FastPlace to 
       run successfully */
    statusCode = DesignRunFastPlace("." + clusterDesignName, clusterDesignName,
				    globalPlacementTime, true, true, false, 
				    placerLogFile);
    topEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
    cout << "Completed running placer: FastPlace ... Returned status: " 
	 << statusCode << endl;
  } else if (globalPlacerType == ENV_MPL6_GP) {
    /* Run MPL6 for global placement. The variable 
       MPL6_FULL_PATH has to be set for mPL6 to 
       run successfully */
    statusCode = DesignRunMPL6("." + clusterDesignName, clusterDesignName,
			       globalPlacementTime, true, true);
    topEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
    cout << "Completed running placer: mPL6 ... Returned status: " 
	 << statusCode << endl;
  } else {
    cout << "Error: External global placer cannot be identified!" << endl;
    exit(0);
  }
}

/*************************************************
  TOP LEVEL FUNCTION TO PERFORM GLOBAL PLACEMENT
*************************************************/
void 
Design::DesignDoGlobalPlacement(void)
{
  EnvGlobalPlacerType globalPlacerType;
  EnvSolverType solverType;
  Env &DesignEnv = this->DesignEnv;
  
  globalPlacerType = DesignEnv.EnvGetGlobalPlacerType();
  solverType = DesignEnv.EnvGetSolverType();
  string DesignName = DesignEnv.EnvGetDesignName();
  /***************************/
  /* Optional: Do clustering */
  /***************************/
  /* Record start time of clustering */
  DesignCoarsenNetlist();
  /* Record end time of clustering */

  if (globalPlacerType == ENV_NO_PLACEMENT) {
    return;
  }
  _STEP_BEGIN("Global placement");  
  /* While doing global placement, it might be necessary to run other 
     placers to do global placement since macro spreading is not 
     yet implemented in our placer */
  if (globalPlacerType == ENV_NO_EXTERNAL_GP) {
    //    ProfilerStart("Global placement");
    /* Record start time of global placement */
    DesignEnv.EnvSetGlobalPlacementStartTime();
    DesignRunInternalPlacer(solverType);
    /* Record end time of global placement */
    DesignEnv.EnvRecordGlobalPlacementTime();
    //    ProfilerStop();
  } else {
    clock_t start, end;
    double cpuTimeSpent;
    time_t timer1;
    time_t timer2;
    time(&timer1);
    //    DesignEnv.EnvSetGlobalPlacementStartTime();
    DesignRunExternalPlacer(globalPlacerType);
    time(&timer2);
    cpuTimeSpent = difftime(timer2, timer1);
    //    cout << "Time taken: " << cpuTimeSpent << endl;
    //DesignEnv.EnvRecordGlobalPlacementTime(cpuTimeSpent);
  }
  cout << "Dumping cluster information: POST-TOP LEVEL PLACEMENT" << endl;
  DesignDumpClusterInfo((DesignName + ".csv"));
  //  ProfilerStart("Unclustering");
  DesignCollapseClusters();
  //  ProfilerStop();

  _STEP_END("Global placement");  
}

/*************************************************
  TOP LEVEL FUNCTION TO PERFORM LEGALIZATION
*************************************************/
void
Design::DesignDoLegalization(void)
{
  Env &DesignEnv = this->DesignEnv;
  string dirName;
  string desName = DesignEnv.EnvGetDesignName();
  EnvLegalizer legalizerType = DesignEnv.EnvGetLegalizer();
  
  switch (legalizerType) {
  case ENV_BIN_BASED_LEGALIZER:
    DesignEnv.EnvSetLegalizationStartTime();
    LegalizeDesign(*this);
    DesignEnv.EnvRecordLegalizationTime();
    break;
  case ENV_FAST_PLACE_LEGALIZER:
    time_t timer1;
    time_t timer2;
    double cpuTimeSpent;
    desName = desName + "_leg";
    dirName = "." + desName;
    DesignWriteCurrentNetlist(dirName, desName);
    time(&timer1);
    DesignRunFastPlaceLegalizer(dirName, desName, true, true);
    time(&timer2);
    cpuTimeSpent = difftime(timer2, timer1);
    DesignEnv.EnvRecordLegalizationTime(cpuTimeSpent);
    break;
  }
}

/*************************************************
  TOP LEVEL FUNCTION TO PERFORM DETAILED PLACEMENT
*************************************************/
void
Design::DesignDoDetailedPlacement(void)
{
  Env &DesignEnv = this->DesignEnv;
  string dirName;
  string desName = DesignEnv.EnvGetDesignName();
  EnvDetailedPlacer detailedPlacerType = DesignEnv.EnvGetDetailedPlacer();
  
  switch (detailedPlacerType) {
  case ENV_NO_DETAIL_PLACEMENT:
    break;
  case ENV_FAST_PLACE_DP:
    time_t timer1;
    time_t timer2;
    double cpuTimeSpent;
    desName = desName + "_dp";
    dirName = "." + desName;
    DesignWriteCurrentNetlist(dirName, desName);
    time(&timer1);
    DesignRunFastPlaceDetailedPlacer(dirName, desName, true, true);
    time(&timer2);
    cpuTimeSpent = difftime(timer2, timer1);
    DesignEnv.EnvRecordDetailedPlacementTime(cpuTimeSpent);
    break;
  case ENV_OURPLACER_DP:
    break;
  default: break;
  }
}
