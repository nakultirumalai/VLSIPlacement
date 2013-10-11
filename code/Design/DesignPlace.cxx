# include <Design.h>

bool clusterShapeDebug = false;
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


bool swapClustersGetHPWL(Cell *clusterCell1, Cell *clusterCell2, 
			 bool useWeightedHPWL, ulong &oldHPWL, 
			 ulong &newHPWL)
{
  Net *netPtr;
  Pin *pinPtr;
  Cell *cellPtr;
  map<Net*, bool> visitedNets;
  double cell1Xpos, cell2Xpos, cell1Ypos, cell2Ypos;
  double oldHPWLDbl, newHPWLDbl;
  double oldHPWLActual, newHPWLActual;
  double netHPWL;
  double netWeight;
  double maxx, minx, maxy, miny;
  double pinXpos, pinYpos;
  uint xHPWL, yHPWL;
  uint numPins;
  bool rtv;

  rtv = false;
  cell1Xpos = (*clusterCell1).CellGetXpos();
  cell1Ypos = (*clusterCell1).CellGetYpos();
  cell2Xpos = (*clusterCell2).CellGetXpos();
  cell2Ypos = (*clusterCell2).CellGetYpos();
  (*clusterCell1).CellSetXpos(cell2Xpos);
  (*clusterCell1).CellSetYpos(cell2Ypos);
  (*clusterCell2).CellSetXpos(cell1Xpos);
  (*clusterCell2).CellSetYpos(cell1Ypos);

  oldHPWLDbl = 0; newHPWLDbl = 0;
  oldHPWLActual = 0; newHPWLActual = 0;
  CELL_FOR_ALL_NETS((*clusterCell1), PIN_DIR_ALL, netPtr) {
    _KEY_EXISTS(visitedNets, netPtr) {
      continue;
    } 
    visitedNets[netPtr] = true;
    maxx = 0; minx = DBL_MAX; maxy = 0; miny = DBL_MAX;
    vector<Pin *> &netPins = (*netPtr).NetGetAllPinsVector();
    netWeight = (*netPtr).weight;
    numPins = (uint)((1 / netWeight) + 1);
    if (numPins > 50) {
      continue;
    }
    if (!useWeightedHPWL) {
      netWeight = 1;
    }
    netHPWL = (netPtr->maxx - netPtr->minx + netPtr->maxy - netPtr->miny);
    oldHPWLDbl += (netWeight) * (netHPWL);
    oldHPWLActual += netHPWL;
    VECTOR_FOR_ALL_ELEMS(netPins, Pin*, pinPtr) {
      if (pinPtr->isHidden) continue;
      cellPtr = (*pinPtr).ParentCell;
      pinXpos = pinPtr->xOffset + cellPtr->x;
      pinYpos = pinPtr->yOffset + cellPtr->y;
      if (maxx < pinXpos) maxx = pinXpos;
      if (minx > pinXpos) minx = pinXpos;
      if (maxy < pinYpos) maxy = pinYpos;
      if (miny > pinYpos) miny = pinYpos;
    } END_FOR;
    netHPWL = (maxx - minx + maxy - miny);
    newHPWLActual += netHPWL;
    newHPWLDbl += (netWeight) * netHPWL;
  } CELL_END_FOR;

  //  cout << "**CP 1" << endl;
  //  cout << "Old HPWL: " << oldHPWLActual << " New HPWL: " << newHPWLActual << endl;
  //  cout << "Old HPWL Weighted: " << oldHPWLDbl << " New HPWL Weighted:  " << newHPWLDbl << endl;
  //  cout << "**CP 1" << endl << endl;

  CELL_FOR_ALL_NETS((*clusterCell2), PIN_DIR_ALL, netPtr) {
    _KEY_EXISTS(visitedNets, netPtr) {
      continue;
    } 
    visitedNets[netPtr] = true;
    maxx = 0; minx = DBL_MAX; maxy = 0; miny = DBL_MAX;
    vector<Pin *> &netPins = (*netPtr).NetGetAllPinsVector();
    netWeight = (*netPtr).weight;
    numPins = (uint)((1 / netWeight) + 1);    
    if (numPins > 50) {
      continue;
    }
    if (!useWeightedHPWL) {
      netWeight = 1;
    }
    netHPWL = (netPtr->maxx - netPtr->minx + netPtr->maxy - netPtr->miny);
    oldHPWLActual += netHPWL;
    oldHPWLDbl += (netWeight) * netHPWL;
    VECTOR_FOR_ALL_ELEMS(netPins, Pin*, pinPtr) {
      if (pinPtr->isHidden) continue;
      cellPtr = (*pinPtr).ParentCell;
      pinXpos = pinPtr->xOffset + cellPtr->x;
      pinYpos = pinPtr->yOffset + cellPtr->y;
      if (maxx < pinXpos) maxx = pinXpos;
      if (minx > pinXpos) minx = pinXpos;
      if (maxy < pinYpos) maxy = pinYpos;
      if (miny > pinYpos) miny = pinYpos;
    } END_FOR;
    netHPWL = (maxx - minx + maxy - miny);
    newHPWLActual += netHPWL;
    newHPWLDbl += (netWeight) * netHPWL;
  } CELL_END_FOR;

  //  cout << "**CP 2" << endl;
  //  cout << "Old HPWL: " << oldHPWLActual << " New HPWL: " << newHPWLActual << endl;
  //  cout << "Old HPWL Weighted: " << oldHPWLDbl << " New HPWL Weighted:  " << newHPWLDbl << endl;
  //  cout << "**CP 2" << endl << endl;

  if (newHPWLDbl < oldHPWLDbl) {
    //    cout << "IMPROVEMENT FOUND!" << endl;
    rtv = true;
    MAP_FOR_ALL_KEYS(visitedNets, Net*, bool, netPtr) {
      (*netPtr).NetComputeHPWL(xHPWL, yHPWL);
    } END_FOR;
    oldHPWL = (ulong)oldHPWLActual;
    newHPWL = (ulong)newHPWLActual;
    //    exit(0);
  } else {
    (*clusterCell1).CellSetXpos(cell1Xpos);
    (*clusterCell1).CellSetYpos(cell1Ypos);
    (*clusterCell2).CellSetXpos(cell2Xpos);
    (*clusterCell2).CellSetYpos(cell2Ypos);
  }
  
  return (rtv);
}

void
Design::DesignDoClusterSwapping(void)
{
  Cell *clusterCell1, *clusterCell2;
  string clusterCellName1, clusterCellName2;
  string DesignName;
  double cell1Xpos, cell2Xpos, cell1Ypos, cell2Ypos;
  double stepTime;
  ulong oldXHPWL, newXHPWL, oldYHPWL, newYHPWL;
  ulong newTotalHPWL;
  ulong totalHPWL, bestHPWL;
  ulong totalXHPWL, totalYHPWL;
  ulong oldHPWL, newHPWL;
  uint dotCount, dotLimit, iterCount;
  bool improved;
  bool useWtHPWL;

  Env &DesignEnv = DesignGetEnv();
  useWtHPWL = DesignEnv.EnvGetUseWeightedHPWL();
  DesignName = DesignEnv.EnvGetDesignName();
  DesignComputeHPWL();
  DesignDumpNetDegreeProfile((DesignName + "_NetsPreSwapping.csv"));
  if (DesignEnv.EnvGetClusterSwapping()) {
    stepTime = getCPUTime();
    DesignComputeHPWL();
    totalHPWL = DesignGetHPWL();
    //    if (useWtHPWL) {
    //      DesignComputeWtHPWL(totalXHPWL, totalYHPWL, totalHPWL);
    //    }
    bestHPWL = totalHPWL;
    dotCount = 0; dotLimit = 200;
    iterCount = 1;
    while (1) {
      improved = false;
      cout << "BEGIN: Cluster swapping ITER: " << iterCount << " HPWL: " << totalHPWL << endl;
      map<Cell*, bool> visitedCells;
      DESIGN_FOR_ALL_CLUSTERS((*this), clusterCellName1, clusterCell1) {
	visitedCells[clusterCell1] = true;
	DESIGN_FOR_ALL_CLUSTERS((*this), clusterCellName2, clusterCell2) {
	  _KEY_EXISTS(visitedCells, clusterCell2) {
	    continue;
	  }
	  //	  if (clusterCell1 == clusterCell2) {
	  //	    continue;
	  //	  }
	  bool swapBenefits = swapClustersGetHPWL(clusterCell1, clusterCell2, useWtHPWL, oldHPWL, newHPWL);
	  if (swapBenefits) {
	    //	    cout << "Swapping benefitted" << endl;
	    //	    cout << "old HPWL:" << oldHPWL << "  new HPWL:" << totalHPWL << endl;
	    improved = true;
	    cout << "*" << flush;
	    this->DesignHPWL -= oldHPWL;
	    this->DesignHPWL += newHPWL;
	    totalHPWL = this->DesignHPWL;
	    //	    cout << "Swapping benefitted" << endl;
	    //	    cout << " Improved HPWL:" << totalHPWL << endl;
	  } 
	} DESIGN_END_FOR;
      } DESIGN_END_FOR;
      //      break;
      iterCount++;
      cout << endl;
      if (improved == false) {
	break;
      }
    }
    stepTime = getCPUTime() - stepTime;
    DesignEnv.EnvRecordClusterSwappingTime(stepTime);
  }
  DesignDumpNetDegreeProfile((DesignName + "_NetsPostSwapping.csv"));
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLAfterClusterSwapping(totalHPWL);
  cout << endl << "Swapping Done. CPU Time:" << stepTime << endl;
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

  DesignDumpNetDegreeProfile((DesignName + "_NetsPreCluster.csv"));
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
  DesignDumpNetDegreeProfile((DesignName + "_NetsPostCluster.csv"));
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

  DesignName = DesignEnv.EnvGetDesignName();
  stepTime = getCPUTime();
  map<string, Cell*> DesignClusterMap = DesignGetClusters();
  map<string, Cell*>::iterator mapIter;
  for (mapIter = DesignClusterMap.begin(); mapIter != DesignClusterMap.end(); mapIter++) {
    cellName = mapIter->first;
    cellPtr = mapIter->second;
    DesignUnclusterLargeCluster(cellPtr, /* noDissolve */false);
  }
  DesignDumpNetDegreeProfile((DesignName + "_NetsPostUncluster.csv"));
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
  string DesignName;
  string finalOutputName;

  Env &DesignEnv = this->DesignEnv;
  DesignName = DesignEnv.EnvGetDesignName();
  finalOutputName = DesignName + "postMP";
  string MPlacerLogFile = DesignName + "_shaping_log";
  netModel = DesignEnv.EnvGetNetModel();
  
  switch (solverType) {
  case ENV_SOLVER_QUADRATIC_MOSEK:
    //    DesignSolveForAllCellsMosekIter();
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
    //ProfilerStart("ClusterSwapping");
    DesignDoClusterSwapping();
    //ProfilerStop();
    DesignFillCellsInCluster();
    DesignDoClusterFlipping();

    /*************************************/
    /***      BEGIN SHAPING FLOW      ***/
    /************************************/
    DesignDoClusterShaping();
    DesignComputeHPWL();
    cout << "BEFORE FLOORPLANNING HPWL : " << DesignGetHPWL() << endl;
    changeDir("inputShaping");
    DesignWriteBookShelfOutput((*this), DesignName, true);
    changeDir("..");
    DesignRunMetaPlacerCapo("forShaping", DesignName, 1, 1, 0, MPlacerLogFile);
    changeDir("afterMP");
    DesignWriteBookShelfOutput((*this), DesignName);
    changeDir("..");
    DesignComputeHPWL();
    cout << "AFTER FLOORPLANNING HPWL : " << DesignGetHPWL() << endl;
    DesignResetOrientations();
    DesignWriteBookShelfOutput((*this), finalOutputName);

    DesignComputeHPWL();
    cout << "AFTER RESET ORIENTATIONS HPWL : " << DesignGetHPWL() << endl;
    //    return;
    break;
  default: cout << "Unknown solver type provided" << endl;
  };
}

void
Design::DesignRunExternalPlacer(EnvGlobalPlacerType globalPlacerType, 
				EnvFlatPlacerType flatPlacer) 
{
  double globalPlacementTime;
  int statusCode;
  string placerPath, placerLogFile;
  string DesignName;
  string clusterDesignName, clustDir;
  bool useFlatPlacer;

  Env &topEnv = DesignGetEnv();
  DesignName = topEnv.EnvGetDesignName();
  clusterDesignName = DesignName + "_clust";
  clustDir = "." + DesignName + "_clust";
  
  useFlatPlacer = true;
  if (flatPlacer == ENV_NO_EXTERNAL_FP) {
    useFlatPlacer = false;    
  }

  /* Write out the current clustered netlist into the folder .designName_clust */
  DesignWriteCurrentNetlist(clustDir, clusterDesignName);
  if (globalPlacerType == ENV_NTUPLACE_GP || flatPlacer == ENV_NTUPLACE_FP) {
    /* Run NTUPlace for global placement. The variable 
       NTUPLACE_FULL_PATH has to be set for NTUPlace to 
       run successfully */
    statusCode = DesignRunNTUPlace("." + clusterDesignName, clusterDesignName, 
				   globalPlacementTime, true, true, false, useFlatPlacer,
				   placerLogFile);
    topEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
    cout << "Completed running placer: NTUPlace ... Returned status: " 
	 << statusCode << endl;
  } else if (globalPlacerType == ENV_FAST_PLACE_GP || flatPlacer == ENV_FAST_PLACE_FP) {
    /* Run FastPlace for global placement. The variable 
       FASTPLACE_FULL_PATH has to be set for FastPlace to 
       run successfully */
    statusCode = DesignRunFastPlace("." + clusterDesignName, clusterDesignName,
				    globalPlacementTime, true, true, false, useFlatPlacer,
				    placerLogFile);
    topEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
    cout << "Completed running placer: FastPlace ... Returned status: " 
	 << statusCode << endl;
  } else if (globalPlacerType == ENV_MPL6_GP || flatPlacer == ENV_MPL6_FP) {
    /* Run MPL6 for global placement. The variable 
       MPL6_FULL_PATH has to be set for mPL6 to 
       run successfully */
    statusCode = DesignRunMPL6("." + clusterDesignName, clusterDesignName,
			       globalPlacementTime, true, true, false, useFlatPlacer);
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
  EnvFlatPlacerType flatPlacerType;
  EnvSolverType solverType;
  double totalHPWL;
  Env &DesignEnv = this->DesignEnv;
  
  globalPlacerType = DesignEnv.EnvGetGlobalPlacerType();
  flatPlacerType = DesignEnv.EnvGetFlatPlacerType();
  solverType = DesignEnv.EnvGetSolverType();
  string DesignName = DesignEnv.EnvGetDesignName();
  if (globalPlacerType == ENV_NO_PLACEMENT) {
    return;
  }
  _STEP_BEGIN("Global placement");  
  /* While doing global placement, it might be necessary to run other 
     placers to do global placement since macro spreading is not 
     yet implemented in our placer */
  if (globalPlacerType == ENV_NO_EXTERNAL_GP &&
      flatPlacerType == ENV_NO_EXTERNAL_FP) {
    //    ProfilerStart("Global placement");
    /***************************/
    /* Optional: Do clustering */
    /***************************/
    /* Record start time of clustering */
    DesignCoarsenNetlist();
    /* Record end time of clustering */

    /* Record start time of global placement */
    DesignEnv.EnvSetGlobalPlacementStartTime();
    DesignRunInternalPlacer(solverType);
    /* Record end time of global placement */
    DesignEnv.EnvRecordGlobalPlacementTime();
    //    ProfilerStop();
  } else {
    if (globalPlacerType != ENV_NO_EXTERNAL_GP &&
	flatPlacerType != ENV_NO_EXTERNAL_FP) {
      cout << "Error: Either the -global_placer option or the -flat_placer option must be specified" << endl;
    }
    clock_t start, end;
    double cpuTimeSpent;
    time_t timer1;
    time_t timer2;
    time(&timer1);
    //    DesignEnv.EnvSetGlobalPlacementStartTime();
    DesignRunExternalPlacer(globalPlacerType, flatPlacerType);
    time(&timer2);
    cpuTimeSpent = difftime(timer2, timer1);
    //    cout << "Time taken: " << cpuTimeSpent << endl;
    //DesignEnv.EnvRecordGlobalPlacementTime(cpuTimeSpent);
  }
  cout << "Dumping cluster information: POST-TOP LEVEL PLACEMENT" << endl;
  DesignDumpClusterInfo((DesignName + ".csv"));
  //  ProfilerStart("Unclustering");
  DesignComputeHPWL();
  totalHPWL = DesignGetHPWL();
  DesignEnv.EnvSetHPWLTotalGlobal(totalHPWL);
  //DesignCollapseClusters();
  //  ProfilerStop();
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
    DesignEnv.EnvRecordLegalizationTime(cpuTimeSpent);
    break;
  }
  DesignComputeHPWL();
  DesignEnv.EnvSetHPWLAfterLegalization(DesignGetHPWL());
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
  DesignComputeHPWL();
  DesignEnv.EnvSetHPWLAfterDetailedPlacement(DesignGetHPWL());
}

inline
void printShapeIndexVector(vector<uint> &shapeIndices)
{
  uint shapeIdx;
  cout << "VECTOR INDICATING SHAPE INDICES" << endl;
  VECTOR_FOR_ALL_ELEMS(shapeIndices, uint, shapeIdx) {
    cout << shapeIdx << "  ";
  } END_FOR;
  cout << endl;
}
    
void
Design::DesignDoClusterShapingOld(void)
{
  Cell *clusterCellPtr;
  string clusterCellName;
  ulong designInitHPWL, designEndHPWL;
  ulong iterBeginHPWL, iterEndHPWL;
  uint maxx, maxy;
  ulong minHPWL;
  uint numShapesChanged, iterCount;
  long iterDiffHPWL, overallDiffHPWL;
  bool shapeChanged;
  double iterPercentImp, percentImp;
  
  cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "                  BEGIN SHAPE SELECTION TO IMPROVE DESIGN           " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  
  DesignGetBoundingBox(maxx, maxy);
  DesignComputeHPWL();
  designInitHPWL = DesignGetHPWL();
  minHPWL = designInitHPWL;
  
  cout << "\nINITIAL HPWL BEFORE SHAPING  :  " << designInitHPWL << endl;
  iterCount = 0;
  do {
    numShapesChanged = 0;
    shapeChanged = false;
    DesignComputeHPWL();
    iterBeginHPWL = DesignGetHPWL();
    
    if (clusterShapeDebug) {
      cout << "BEGIN ITERATION : " << iterCount << endl;
    }
    
    /* Iterate over each cluster and choose the best shape for each cluster
       based on the improvement in HPWL. (GREEDY algorithm) */
    DESIGN_FOR_ALL_CLUSTERS((*this), clusterCellName, clusterCellPtr) {
      if (clusterShapeDebug) {
	cout << "^^DEBUG : " << endl;
	cout << "\tProcessing Cluster : " << clusterCellName << endl;
      }
     
      DesignGetBestShapeForCluster(clusterCellPtr, maxx, maxy, minHPWL, shapeChanged);
      if (shapeChanged) {
	numShapesChanged++;
      }
      shapeChanged = false;
    } DESIGN_END_FOR;
    
  
    DesignComputeHPWL();
    iterEndHPWL = DesignGetHPWL();
    iterDiffHPWL = iterBeginHPWL - iterEndHPWL;
    iterPercentImp = ((double)iterDiffHPWL / iterBeginHPWL) * 100;
    cout << "\n===============================================" << endl;
    cout << "\tSHAPING STATS FOR ITERATION " << iterCount << endl;
    cout << "===============================================" << endl;
    cout << endl;
    cout << "\tBEGIN HPWL               : " << iterBeginHPWL << endl;
    cout << "\tEND HPWL                 : " << iterEndHPWL << endl;
    if (iterDiffHPWL > 0) {
      cout << "\tRESULT OF SHAPING        : IMPROVED" << endl;
      cout << "\tPERCENT IMPROVED         : " << iterPercentImp << "%" << endl;
    } else if (iterDiffHPWL == 0) {
      cout << "\tRESULT OF SHAPING        : NO CHANGE" << endl;
    } else if (iterDiffHPWL < 0) {
      _ASSERT_TRUE("ERROR!!! Degradation in HPWL found beyond default\n");
    }
    iterCount++;
    cout << "\tNUMBER OF SHAPES CHANGED : " << numShapesChanged << endl;
  } while ((iterEndHPWL < iterBeginHPWL) && (iterCount < 100));

  DesignComputeHPWL();
  designEndHPWL = DesignGetHPWL();
  overallDiffHPWL = designInitHPWL - designEndHPWL;
  percentImp = ((double)overallDiffHPWL / designInitHPWL) * 100;

  cout << "\n**********************************************************" << endl;
  cout << "  GLOBAL STATISTICS FOR SHAPING" << endl;
  cout << "**********************************************************" << endl;
  cout << "\tINITIAL HPWL FOR DESIGN         : " << designInitHPWL << endl;
  cout << "\tFINAL HPWL FOR DESIGN           : " << designEndHPWL << endl;
  cout << "\tDIFFERENCE IN HPWL              : " << overallDiffHPWL << endl;
  cout << "\tPERCENTAGE IMPROVEMENT          : " << percentImp << "%" << endl;
  cout << "\tNUMBER OF ITERATIONS REQUIRED   : " << iterCount << endl;
  cout << "\n**********************************************************" << endl;
}

void
Design::DesignDoClusterShaping(void)
{
  Cell *clusterCellPtr;
  vector<uint> shapeIndices;
  /* Required to store the last accepted shapes for restoring,
     if move is not accepted */
  vector<uint> laShapeIndices;
  string clusterCellName;
  ulong designInitHPWL, designEndHPWL;
  ulong iterBeginHPWL, iterEndHPWL;
  uint maxx, maxy, clusterIdx;
  ulong minHPWL;
  uint numShapesChanged, iterCount;
  uint numClusters, numShapeVariations;
  long iterDiffHPWL, overallDiffHPWL;
  bool shapeChanged;
  double iterPercentImp, percentImp;

  Env &DesignEnv = DesignGetEnv();
  numClusters = DesignEnv.EnvGetNumClusters();
  numShapeVariations = DesignEnv.EnvGetNumShapeVariations();
  
  cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "                  BEGIN SHAPING AND MACRO PLACEMENT           " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  
  DesignGetBoundingBox(maxx, maxy);
  DesignComputeHPWL();
  designInitHPWL = DesignGetHPWL();
  cout << "\nINITIAL HPWL BEFORE SHAPING  :  " << designInitHPWL << endl;
  
  if (clusterShapeDebug) {
    cout << "Number of Clusters : " << numClusters << endl;
  }
  
  /* Assign shapes randomly to all the clusters initially */
  DesignGetClusterShapes(shapeIndices, numShapeVariations, numClusters);
  
  /* The initial shape assignment is stored before the annealer is invoked */
  laShapeIndices = shapeIndices;
  //printShapeIndexVector(shapeIndices);
  
  /* Change the cluster to the assigned shapes and also change the placement
     of cells inside the cluster */
  DesignChangeClusterShapes(shapeIndices);

  /* Invoke the annealer to find the optimal solution */
  




}

void
Design::DesignResetOrientations(void) 
{
  uint numClustersReset;
  numClustersReset = DesignResetAllClustersOrient();
  cout << "NUMBER OF CLUSTERS FLIPPED TO RESET: " << numClustersReset << endl;
}
