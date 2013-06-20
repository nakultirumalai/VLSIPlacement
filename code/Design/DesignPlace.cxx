# include <Design.h>

void
Design::DesignCoarsenNetlist(void)
{
  Cell *cellPtr;
  Pin *pinPtr;
  vector<Cell *> listOfCells;
  Env &DesignEnv = this->DesignEnv;
  string DesignName, cellName;
  string clustDir;
  uint numPinsBefore, numPinsAfter;

  numPinsBefore = 0; numPinsAfter = 0;
  cout << "Coarsening netlist" << endl;
  HyperGraph &myGraph = DesignGetGraph();
  //DesignDoClusterTest();
  DesignDoBestChoiceClustering(myGraph);
  cout << "Finished coarsening" << endl;
  
  /* Rebuild hypergraph after coarsening */
  cout << "Rebuilding hypergraph.." << endl;
  
  DesignRebuildGraph();
  HyperGraph &myGraph2 = DesignGetGraph();
  cout << "After clustering:" << endl
       << " Number of nodes:" << myGraph2.GetNumNodes() << endl
       << " Number of edges:" << myGraph2.GetNumEdges() 
       << endl;
  DesignName = DesignEnv.EnvGetDesignName();

  /* Write out the output of the clustered netlist here */
  clustDir = "." + DesignName + "_clust";
  makeDir(clustDir);
  chdir(clustDir.data());
  DesignWriteBookShelfOutput((*this), (DesignName + "_clust"));
  chdir("..");
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
  string clusterDesignName;

  Env &topEnv = DesignGetEnv();
  DesignName = topEnv.EnvGetDesignName();
  clusterDesignName = DesignName + "_clust";

  /* assume that the clustered netlist has been created in 
     .designName_clust folder */
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
  ProfilerStart("Start profiler");
  DesignCoarsenNetlist();
  ProfilerStop();
  return;

  _STEP_BEGIN("Global placement");
  /* While doing global placement, it might be necessary to run other 
     placers to do global placement since macro spreading is not 
     yet implemented in our placer */
  if (globalPlacerType == ENV_NO_EXTERNAL_GP) {
    DesignRunInternalPlacer(solverType);
  } else {
    DesignRunExternalPlacer(globalPlacerType);
  }
  _STEP_END("Global placement");  
}
