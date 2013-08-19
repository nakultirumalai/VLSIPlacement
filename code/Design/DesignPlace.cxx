# include <Design.h>

void
Design::DesignFillCellsInCluster(void)
{
  Cell *clusterCell;
  string clusterCellName;
  double totalHPWL, totalClusterPlacementTime;
  double clusterPlacementTime;
  uint count;

  count = 0;
  Env &DesignEnv = DesignGetEnv();
  EnvFlowType flowType = DesignEnv.EnvGetFlowType();
  totalClusterPlacementTime = 0;
  if (flowType != ENV_PLACE_CLUSTERS_PRE_TOP) {
    DESIGN_FOR_ALL_CLUSTERS((*this), clusterCellName, clusterCell) {
      DesignClusterPlaceCells(clusterCell, clusterPlacementTime);
      totalClusterPlacementTime += clusterPlacementTime;
      count++;
    } DESIGN_END_FOR;
  }
  clusterPlacementTime = DesignEnv.EnvGetClusterFillingTime();
  clusterPlacementTime += totalClusterPlacementTime;
  DesignEnv.EnvRecordClusterFillingTime(clusterPlacementTime);
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLAfterClusterFill(totalHPWL);
}

void
Design::DesignDoClusterFlipping(void)
{
  double totalHPWL, stepTime;
  bool improved;
  Env &DesignEnv = DesignGetEnv();

  if (DesignEnv.EnvGetClusterMirroring()) {
    stepTime = getCPUTime();
    while (1) {
      improved = DesignFlipClusters(true);
      if (!improved) break;
    }
    while (1) {
      improved = DesignFlipClusters(false);
      if (!improved) break;
    }
    stepTime = getCPUTime() - stepTime;
    DesignEnv.EnvRecordClusterMirroringTime(stepTime);
  }
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLAfterClusterMirroring(totalHPWL);
}

void
Design::DesignDoClusterSwapping(void)
{
  Cell *clusterCell1, *clusterCell2;
  string clusterCellName1, clusterCellName2;
  double cell1Xpos, cell2Xpos, cell1Ypos, cell2Ypos;
  double stepTime;
  ulong oldXHPWL, newXHPWL, oldYHPWL, newYHPWL;
  ulong newTotalHPWL;
  ulong totalHPWL, bestHPWL;
  uint dotCount, dotLimit, iterCount;
  bool improved;
  Env &DesignEnv = DesignGetEnv();

  if (DesignEnv.EnvGetClusterSwapping()) {
    stepTime = getCPUTime();
    DesignComputeHPWL();
    totalHPWL = DesignGetHPWL();
    bestHPWL = totalHPWL;
    dotCount = 0; dotLimit = 200;
    iterCount = 1;
    while (1) {
      improved = false;
      cout << "BEGIN: Cluster swapping ITER: " << iterCount << " HPWL: " << totalHPWL << endl;
      DESIGN_FOR_ALL_CLUSTERS((*this), clusterCellName1, clusterCell1) {
	DESIGN_FOR_ALL_CLUSTERS((*this), clusterCellName2, clusterCell2) {
	  cell1Xpos = (*clusterCell1).CellGetXpos();
	  cell1Ypos = (*clusterCell1).CellGetYpos();
	  cell2Xpos = (*clusterCell2).CellGetXpos();
	  cell2Ypos = (*clusterCell2).CellGetYpos();
	  (*clusterCell1).CellSetXpos(cell2Xpos);
	  (*clusterCell1).CellSetYpos(cell2Ypos);
	  (*clusterCell2).CellSetXpos(cell1Xpos);
	  (*clusterCell2).CellSetYpos(cell1Ypos);
	  DesignFindModifiedHPWL(clusterCell1);
	  DesignFindModifiedHPWL(clusterCell2);
	  totalHPWL = DesignGetHPWL();
	  if (totalHPWL < bestHPWL) {
	    bestHPWL = totalHPWL;
	    improved = true;
	    cout << "*" << flush;
	  } else {
	    (*clusterCell1).CellSetXpos(cell1Xpos);
	    (*clusterCell1).CellSetYpos(cell1Ypos);
	    (*clusterCell2).CellSetXpos(cell2Xpos);
	    (*clusterCell2).CellSetYpos(cell2Ypos);
	    DesignFindModifiedHPWL(clusterCell1);
	    DesignFindModifiedHPWL(clusterCell2);
	  }
	} DESIGN_END_FOR;
      } DESIGN_END_FOR;
      iterCount++;
      cout << endl;
      if (iterCount > 2) {
	break;
      }
      if (improved == false) {
	break;
      }
    }
    stepTime = getCPUTime() - stepTime;
    DesignEnv.EnvRecordClusterSwappingTime(stepTime);
  }
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLAfterClusterSwapping(totalHPWL);
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
    DesignDoKWayClustering(myGraph, true);
    break;
  case ENV_FIRST_CHOICE_CLUSTERING:
    cout << "Clustering: First choice clustering. Not ready yet " << endl;
    break;
  case ENV_BEST_CHOICE_CLUSTERING:
    cout << "Clustering: Best choice clustering " << endl << endl;
    if (clusterNumRows > 0) {
      DesignDoBestChoiceClusteringCstr(myGraph, inputMaxArea, inputMaxWidth,
				       true, clusterRatio);
    } else {
      DesignDoBestChoiceClustering(myGraph, clusterRatio);
    }
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

  achievedClusteringRatio = DesignGetNumTopCells() / achievedClusteringRatio;
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
  double totalHPWL, stepTime;
  vector<Cell*> cellsToSolve;
  Env &DesignEnv = this->DesignEnv;
  string DesignName, cellName;
  string clustDir;
  HyperGraph &myGraph = DesignGetGraph();

  stepTime = getCPUTime();
  map<string, Cell*> DesignClusterMap = DesignGetClusters();
  map<string, Cell*>::iterator mapIter;
  for (mapIter = DesignClusterMap.begin(); mapIter != DesignClusterMap.end(); mapIter++) {
    cellName = mapIter->first;
    cellPtr = mapIter->second;
    DesignUnclusterLargeCluster(cellPtr, /* noDissolve */false);
  }
  DesignComputeHPWL();
  stepTime = getCPUTime() - stepTime;
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLAfterUnclustering(totalHPWL);
  DesignEnv.EnvRecordUnclusteringTime(stepTime);
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
    ProfilerStart("ClusterSwapping");
    DesignDoClusterSwapping();
    ProfilerStop();
    DesignFillCellsInCluster();
    DesignDoClusterFlipping();
    //    return;
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
			       globalPlacementTime, true, true, false);
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
  double totalHPWL;
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
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLTotalGlobal(totalHPWL);
  DesignCollapseClusters();
  //  ProfilerStop();
  DesignComputeHPWL();
  cout << "Quality : X-HPWL: " << DesignGetXHPWL() 
       << " Y-HPWL: " << DesignGetYHPWL() 
       << " TOTAL: " << DesignGetHPWL() << endl;

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
    DesignComputeHPWL();
    DesignEnv.EnvSetHPWLAfterLegalization(DesignGetHPWL());
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
    DesignComputeHPWL();
    DesignEnv.EnvSetHPWLAfterDetailedPlacement(DesignGetHPWL());
    DesignEnv.EnvRecordDetailedPlacementTime(cpuTimeSpent);
    break;
  case ENV_OURPLACER_DP:
    break;
  default: break;
  }
}
