# include <Design.h>

void
Design::DesignCoarsenNetlist(void)
{
  Cell *cellPtr;
  Pin *pinPtr;
  Env &DesignEnv = this->DesignEnv;
  uint numPinsBefore, numPinsAfter;

  numPinsBefore = 0; numPinsAfter = 0;
  cout << "Coarsening netlist" << endl;
  HyperGraph &myGraph = DesignGetGraph();
  DesignDoBestChoiceClustering(myGraph);
  cout << "Finished coarsening" << endl;
  /* Rebuild hypergraph after coarsening */
  cout << "Rebuilding hypergraph.." << endl;
  DesignRebuildGraph();
  HyperGraph &myGraph2 = DesignGetGraph();
  //  cout << "After clustering:" << endl
  //       << " Number of nodes:" << myGraph2.GetNumNodes() << endl
  //       << " Number of edges:" << myGraph2.GetNumEdges() 
  //       << endl;
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

  //  return;
  if (0) {
    cellPtr = DesignGetNode("c2405");
    cellsToSolve.push_back(cellPtr);
    DesignPlotDataSelected("Before unclustering", "before.plt", cellsToSolve);
    clusterOfCell = (Cluster *) CellGetCluster(cellPtr);
    (*clusterOfCell).PrintCluster();
    cellsToSolve = (*clusterOfCell).ClusterGetCellsOfCluster();
    DesignUnclusterCell(cellPtr, /* noDissolve = false */false);
    DesignPlotDataSelected("After unclustering", "after.plt", cellsToSolve);
    return;
  } else {
    map<string, Cell*> DesignClusterMap = DesignGetClusters();
    map<string, Cell*>::iterator mapIter;
    for (mapIter = DesignClusterMap.begin(); mapIter != DesignClusterMap.end(); mapIter++) {
      cellName = mapIter->first;
      cellPtr = mapIter->second;

      //    cout << "Unclustering cell " << cellName << endl;
      //    if (cellName == "c205") {
      //      cout << "Break here" << endl;
      //    }
      DesignUnclusterCell(cellPtr, /* noDissolve = false */false);
    }
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
  default: cout << "Unknown solver type provided" << endl;
  };
}

void
Design::DesignRunExternalPlacer(EnvGlobalPlacerType globalPlacerType) 
{
  int statusCode;
  string placerPath;
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
    statusCode = DesignRunNTUPlace("." + clusterDesignName, clusterDesignName);
    cout << "Completed running placer: NTUPlace ... Returned status: " 
	 << statusCode << endl;
  } else if (globalPlacerType == ENV_FAST_PLACE_GP) {
    /* Run FastPlace for global placement. The variable 
       FASTPLACE_FULL_PATH has to be set for FastPlace to 
       run successfully */
    statusCode = DesignRunFastPlace("." + clusterDesignName, clusterDesignName);
    cout << "Completed running placer: FastPlace ... Returned status: " 
	 << statusCode << endl;
  } else if (globalPlacerType == ENV_MPL6_GP) {
    /* Run MPL6 for global placement. The variable 
       MPL6_FULL_PATH has to be set for mPL6 to 
       run successfully */
    statusCode = DesignRunMPL6("." + clusterDesignName, clusterDesignName);
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
  
  /***************************/
  /* Optional: Do clustering */
  /***************************/
  ProfilerStart("Begin clustering");
  /* Record start time of clustering */
  DesignEnv.EnvSetClusteringStartTime();
  DesignCoarsenNetlist();
  /* Record end time of clustering */
  DesignEnv.EnvRecordClusteringTime();
  ProfilerStop();

  _STEP_BEGIN("Global placement");
  /* While doing global placement, it might be necessary to run other 
     placers to do global placement since macro spreading is not 
     yet implemented in our placer */
  if (globalPlacerType == ENV_NO_EXTERNAL_GP) {
    ProfilerStart("Global placement");
    /* Record start time of global placement */
    DesignEnv.EnvSetGlobalPlacementStartTime();
    DesignRunInternalPlacer(solverType);
    /* Record end time of global placement */
    DesignEnv.EnvRecordGlobalPlacementTime();
    ProfilerStop();
  } else {
    clock_t start, end;
    double cpuTimeSpent;
    start = clock();
    cout << "START TIME: " << start << endl;
    DesignEnv.EnvSetGlobalPlacementStartTime();
    DesignRunExternalPlacer(globalPlacerType);
    end = clock();
    cout << "END TIME: " << start << endl;
    cpuTimeSpent = ((double) (end - start)) / CLOCKS_PER_SEC;
    DesignEnv.EnvRecordGlobalPlacementTime(cpuTimeSpent);
  }
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
    LegalizeDesign(*this);
    break;
  case ENV_FAST_PLACE_LEGALIZER:
    desName = desName + "_leg";
    dirName = "." + desName;
    DesignWriteCurrentNetlist(dirName, desName);
    DesignRunFastPlaceLegalizer(dirName, desName);
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
    desName = desName + "_dp";
    dirName = "." + desName;
    DesignWriteCurrentNetlist(dirName, desName);
    DesignRunFastPlaceDetailedPlacer(dirName, desName);
    break;
  case ENV_OURPLACER_DP:
    break;
  default: break;
  }
}
