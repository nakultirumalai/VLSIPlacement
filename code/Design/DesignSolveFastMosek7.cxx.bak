# include <Design.h>

void
MSKAPI printDesignSolverOutput(void *handle, const char str[])
{
  //  printf("%s", str);
}

void
getObjFuncInMosekFormat(Design &myDesign, HyperGraph &myGraph,
                        vector<Cell *> &inputCells, 
			MSKidxt **qsubix, MSKidxt **qsubjx, MSKidxt **qsubx, 
			MSKidxt **qsubiy, MSKidxt **qsubjy, MSKidxt **qsuby, 
			MSKrealt **qvalijx, MSKrealt **qvalx, 
			MSKrealt **qvalijy, MSKrealt **qvaly, 
			uint &numValuesQuad, uint &numValuesLin, 
			double &constantx, double &constanty,
			map<Cell *, uint>& quadMap,
			map<Cell *, uint>& linMap,
			EnvNetModel netModel)
{
  vector<uint> subi_vecx, subj_vecx, sub_vecx;
  vector<uint> subi_vecy, subj_vecy, sub_vecy;
  vector<double> valij_vecx, val_vecx;
  vector<double> valij_vecy, val_vecy;
  uint i;

  if (netModel == ENV_CLIQUE_MODEL) {
    getObjectiveCliqueModelXY(myDesign, myGraph, inputCells, 
			      subi_vecx, subj_vecx, valij_vecx, 
			      sub_vecx, val_vecx, 
			      subi_vecy, subj_vecy, valij_vecy, 
			      sub_vecy, val_vecy, 
			      constantx, constanty, quadMap, linMap);
  } else if (netModel == ENV_STAR_MODEL) {
    getObjectiveStarModelXY(myDesign, myGraph, inputCells, 
			      subi_vecx, subj_vecx, valij_vecx, 
			      sub_vecx, val_vecx, 
			      subi_vecy, subj_vecy, valij_vecy, 
			      sub_vecy, val_vecy, 
			      constantx, constanty, quadMap, linMap);
  }    
  
  if (0) {
    cout << "=========================" << endl;
    cout << "Printing linear matrix X:" << endl;
    cout << "=========================" << endl;
    for (i = 0; i < val_vecx.size(); i++) {
      cout << val_vecx[i] << endl;
    }
    cout << "=========================" << endl;
    cout << "Printing linear matrix Y:" << endl;
    cout << "=========================" << endl;
    for (i = 0; i < val_vecy.size(); i++) {
      cout << val_vecy[i] << endl;
    }
    exit(0);
  }
    
  *qsubix = (MSKidxt *) malloc(sizeof(MSKidxt) * subi_vecx.size());
  *qsubiy = (MSKidxt *) malloc(sizeof(MSKidxt) * subi_vecy.size());
  *qsubjx = (MSKidxt *) malloc(sizeof(MSKidxt) * subj_vecx.size());
  *qsubjy = (MSKidxt *) malloc(sizeof(MSKidxt) * subj_vecy.size());
  *qvalijx = (MSKrealt *) malloc(sizeof(MSKrealt) * valij_vecx.size());
  *qvalijy = (MSKrealt *) malloc(sizeof(MSKrealt) * valij_vecy.size());
  *qsubx = (MSKidxt *) malloc(sizeof(MSKidxt) * sub_vecx.size());
  *qsuby = (MSKidxt *) malloc(sizeof(MSKidxt) * sub_vecy.size());
  *qvalx = (MSKrealt *) malloc(sizeof(MSKrealt) * val_vecx.size());
  *qvaly = (MSKrealt *) malloc(sizeof(MSKrealt) * val_vecy.size());

  for (i = 0; i < subi_vecx.size(); i++) {
    (*qsubix)[i] = subi_vecx[i];
    (*qsubjx)[i] = subj_vecx[i];
    (*qvalijx)[i] = 2 * valij_vecx[i];
    (*qsubiy)[i] = subi_vecy[i];
    (*qsubjy)[i] = subj_vecy[i];
    (*qvalijy)[i] = 2 * valij_vecy[i];
  }
  numValuesQuad = subi_vecx.size();

  for (i = 0; i < sub_vecx.size(); i++) {
    (*qsubx)[i] = sub_vecx[i];
    (*qvalx)[i] = 2 * val_vecx[i];
    (*qsuby)[i] = sub_vecy[i];
    (*qvaly)[i] = 2 * val_vecy[i];
  }
  if (0) {
    uint numVars = inputCells.size();
    cout << "=========================" << endl;
    cout << "Printing linear matrix X:" << endl;
    cout << "=========================" << endl;
    for (i = 0; i < numVars; i++) {
      cout << (*qvalx)[i] << endl;
    }
    cout << "=========================" << endl;
    cout << "Printing linear matrix Y:" << endl;
    cout << "=========================" << endl;
    for (i = 0; i < numVars; i++) {
      cout << (*qvaly)[i] << endl;
    }
    exit(0);
  }
  numValuesLin = sub_vecx.size();
}

void
getVarBoundsInMosekFormat(Design &myDesign, vector<Cell *>inputCells,
			  MSKint32t **subbx, MSKrealt **blx, MSKrealt **bux, 
			  MSKboundkeye **bkx,
			  MSKint32t **subby, MSKrealt **bly, MSKrealt **buy, 
			  MSKboundkeye **bky,
			  MSKint32t &numXbounds, MSKint32t &numYbounds,
			  bool useVarBounds)
{
  uint maxx, maxy;
  double upperBound, lowerBound, delta;
  unsigned int numCells;

  myDesign.DesignGetBoundingBox(maxx, maxy);
  numCells = inputCells.size();

  numXbounds = numCells;
  numYbounds = numCells;
  
  *subbx = (MSKint32t *) malloc(sizeof(MSKint32t) * numCells);
  *subby = (MSKint32t *) malloc(sizeof(MSKint32t) * numCells);
  *blx = (MSKrealt *) malloc(sizeof(MSKrealt) * numCells);
  *bux = (MSKrealt *) malloc(sizeof(MSKrealt) * numCells);
  *bkx = (MSKboundkeye *) malloc(sizeof(MSKboundkeye) * numCells);
  *bly = (MSKrealt *) malloc(sizeof(MSKrealt) * numCells);
  *buy = (MSKrealt *) malloc(sizeof(MSKrealt) * numCells);
  *bky = (MSKboundkeye *) malloc(sizeof(MSKboundkeye) * numCells);

  delta = 0.001;
  lowerBound = 0.0 + delta;
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *(inputCells[i]);
    upperBound = ((double)maxx)/GRID_COMPACTION_RATIO -
      ((double)thisCell.CellGetWidth()) / GRID_COMPACTION_RATIO;
    (*blx)[i] = lowerBound;
    (*bux)[i] = upperBound;
    if (useVarBounds == true) {
      (*bkx)[i] = MSK_BK_RA;
    } else {
      (*bkx)[i] = MSK_BK_FR;      
    }
    (*subbx)[i] = i;
    upperBound = ((double)maxx)/GRID_COMPACTION_RATIO -
      ((double)thisCell.CellGetWidth()) / GRID_COMPACTION_RATIO;
    (*bly)[i] = lowerBound;
    (*buy)[i] = upperBound;
    if (useVarBounds == true) {
      (*bky)[i] = MSK_BK_RA;
    } else {
      (*bky)[i] = MSK_BK_FR;      
    }
    (*subby)[i] = i;
  }
}


bool
initMosek(MSKenv_t *env, MSKtask_t *task, uint numVars, uint numCstrs)

{
  MSKrescodee r;
  bool rtv;

  rtv = true;
  r = MSK_makeenv(env, NULL);
  if (r == MSK_RES_OK) {
    MSK_linkfunctoenvstream(*env, MSK_STREAM_LOG, NULL, printDesignSolverOutput);
  } else rtv = false;

  /* Initialize the environment */
  r = MSK_initenv(*env);
  if (r == MSK_RES_OK) {
    /* Create the optimization task */
    r = MSK_maketask(*env, numCstrs, numVars, task);
    if (r == MSK_RES_OK) {
      r = MSK_linkfunctotaskstream(*task, MSK_STREAM_LOG, NULL, printDesignSolverOutput);
      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_appendcons(*task, numCstrs);
      if (r == MSK_RES_OK) r = MSK_appendvars(*task, numVars);
    } else rtv = false;
  } else rtv = false;
  
  return (rtv);
}

bool 
putVarBounds(MSKtask_t *task, MSKint32t numVarBounds,
	     MSKint32t *subb, MSKrealt *bu, 
	     MSKrealt *bl, MSKboundkeye *bk)
{
  MSKrescodee r;
  bool rtv;

  rtv = true;
  r = MSK_putvarboundlist(*task, numVarBounds, subb, bk, bl, bu);
  if (r != MSK_RES_OK) {
    rtv = false;
  }
  
  return (rtv);
}

void 
Design::DesignSolveForAllCellsMosekIter()
{
  void *cellObj;
  ofstream logFile;
  vector<Cell *> inputCells;
  vector<Cell *> cellsSortedByLeft;
  map<Cell *, uint> quadMap;
  map<Cell *, uint> linMap;
  map<Cell*, uint>::iterator quadMapItr;
  map<Cell*, uint>::iterator linMapItr;
  map<uint, double> quadMapX, quadMapY;
  map<uint, double> linMapX, linMapY;
  string DesignPath, logFileName;
  string DirName;
  double constantx, constanty;
  double peakUtilization, prevPeakUtil;
  uint numVars, numCstrs;
  uint numValuesQuad, numValuesLin;
  uint peakUtilBinIdx;
  uint nodeIdx, i;
  bool useVarBounds;

  /* All the initilizations */
  Env &DesignEnv = (*this).DesignEnv;
  DesignPath = DesignEnv.EnvGetDesignPath();
  DirName = DesignPath + "/.solverData";
  useVarBounds = DesignEnv.EnvGetUseVarBounds();

  HyperGraph &myGraph = (*this).DesignGetGraph();

  /* Insert cells into a vector in the order of their indices in the 
     hypergraph */ 
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    //    cout << "Cell to solve: " << (*(Cell *)cellObj).CellGetName() << endl;
  } HYPERGRAPH_END_FOR;

  DesignSetCellsToSolve(inputCells);

  /* Define MOSEK variables */
  MSKidxt *qsubix, *qsubjx, *qsubx;
  MSKidxt *qsubiy, *qsubjy, *qsuby;
  MSKint32t *subbx, *subby;
  MSKrealt *qvalijx, *qvalx;
  MSKrealt *qvalijy, *qvaly;
  MSKrealt *bux, *blx, *buy, *bly;
  MSKboundkeye *bkx, *bky;
  MSKint32t numXbounds, numYbounds;
  
  /* Build the quadratic objective matrix for the cells */
  getObjFuncInMosekFormat((*this), myGraph, inputCells, 
			  &qsubix, &qsubjx, &qsubx, 
			  &qsubiy, &qsubjy, &qsuby, 
			  &qvalijx, &qvalx, &qvalijy, 
			  &qvaly, numValuesQuad, numValuesLin, 
			  constanty, constantx, quadMap, linMap,
			  DesignEnv.EnvGetNetModel());

  /* Get the variable bounds */
  getVarBoundsInMosekFormat((*this), inputCells, &subbx, &blx, &bux, &bkx, 
			    &subby, &bly, &buy, &bky, numXbounds, numYbounds,
			    useVarBounds);

  /* STORE THE ORIGINAL VALUES OF THE QUADRATIC TERMS IN A MAP */
  /* THIS WOULD BE A KEY-VALUE: INDEX-VALUE PAIR WHICH WOULD   */
  /* THEN BE USED TO REPLACE THE VALUES IN THE QVALIJ MATRIX   */
  Cell *cellPtr;
  uint qvalIdx;
  MAP_FOR_ALL_ELEMS(quadMap, Cell*, uint, cellPtr, qvalIdx) {
    quadMapX[qvalIdx] = qvalijx[qvalIdx];
    quadMapY[qvalIdx] = qvalijy[qvalIdx];
  } END_FOR;
  MAP_FOR_ALL_ELEMS(linMap, Cell*, uint, cellPtr, qvalIdx) {
    linMapX[qvalIdx] = qvalx[qvalIdx];
    linMapY[qvalIdx] = qvaly[qvalIdx];
  } END_FOR;

  numCstrs = 0;
  numVars = inputCells.size();

  /* MOSEK ENV */
  MSKenv_t envx, envy;
  MSKtask_t taskx, tasky;
  MSKrescodee r = MSK_RES_OK;
  bool opRes;
  uint itrCount;
  double stopThreshold;
  double x[numVars];
  double y[numVars];

  /**************************************************************/
  /* INITIALIZE MOSEK                                           */
  /**************************************************************/
  if (!initMosek(&envx, &taskx, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return;
  }
  if (!initMosek(&envy, &tasky, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return;
  }

  opRes = putVarBounds(&taskx, numXbounds, subbx, bux, blx, bkx);
  opRes = putVarBounds(&tasky, numYbounds, subby, buy, bly, bky);
  /**************************************************************/
  /* LOOP OF QUADRATIC SOLVING AND SPREADING BEINGS HERE        */
  /**************************************************************/
  prevPeakUtil = 0.0;
  itrCount = 0;
  stopThreshold = 0.5;
  _STEP_BEGIN("Analytical solve and spread iterations");
  while (1) {
    /**************************************************************/
    /* SOLVER PART BEGINS HERE  :  SOLVE FOR X FIRST AND THEN Y   */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for X");
    if (opRes) r = MSK_putqobj(taskx, numValuesQuad, qsubix, qsubjx, qvalijx);
    if (r == MSK_RES_OK) r = MSK_putclist(taskx, numValuesLin, qsubx, qvalx);
    if (r == MSK_RES_OK) r = MSK_putobjsense(taskx , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for X");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in X direction");
      r = MSK_optimizetrm(taskx, &trmcode);
      _STEP_END("Optimizing in X direction");
      MSK_solutionsummary(taskx, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolsta(taskx, MSK_SOL_ITR, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(taskx, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, x);
	}
      }
    }

    /**************************************************************/
    /* SOLVE FOR Y NEXT                                           */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for Y");
    if (opRes) r = MSK_putqobj(tasky, numValuesQuad, qsubiy, qsubjy, qvalijy);
    if (r == MSK_RES_OK) r = MSK_putclist(tasky, numValuesLin, qsuby, qvaly);
    if (r == MSK_RES_OK) r = MSK_putobjsense(tasky , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for Y");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in Y direction");
      r = MSK_optimizetrm(tasky, &trmcode);
      _STEP_END("Optimizing in Y direction ");
      MSK_solutionsummary(tasky, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolsta(tasky, MSK_SOL_ITR, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(tasky, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, y);
	}
      }
    }

    /**************************************************************/
    /* WRITE CELL LOCATIONS TO LOG FILE IN EACH ITERATION         */
    /* IN THE TEST MODE ONLY                                      */
    /**************************************************************/
    if (DesignEnv.EnvGetToolMode() == ENV_MODE_TEST) {
      if (!dirExists(DirName)) {
        if (!(0 == mkdir(DirName.data(), S_IRWXU | S_IRWXG | S_IRWXO))) {
          cout << "Error: Directory does not exist. Cannot create directory!!" << endl;
          exit(0);
        }
      }
      logFileName = DirName + "/MoselSolX_Itr" + getStrFromInt(itrCount) + ".txt";
      logFile.open(logFileName.data());
      for (i = 0; i < numVars; i++) {
	logFile << "X" << i << ": " << x[i] << endl;
      }
      logFile.close();
      logFileName = DirName + "/MoselSolY_Itr" + getStrFromInt(itrCount) + ".txt";
      logFile.open(logFileName.data());
      for (i = 0; i < numVars; i++) {
	logFile << "Y" << i << ": " << y[i] << endl;
      }
      logFile.close();
    }

    /**************************************************************/
    /* ASSIGN LOCATIONS TO CELLS                                  */
    /**************************************************************/
    _STEP_BEGIN("Assigning locations to cells");
    Cell *cellPtr;
    double xpos, ypos;
    for (i = 0; i < numVars; i++) {
      cellPtr = inputCells[i];
      xpos = x[i] * GRID_COMPACTION_RATIO; ypos = y[i] * GRID_COMPACTION_RATIO;
      (*cellPtr).CellSetXpos(dtoi(xpos));
      (*cellPtr).CellSetYpos(dtoi(ypos));
      CellSetDblX(cellPtr, xpos);
      CellSetDblY(cellPtr, ypos);
    }
    _STEP_END("Assigning locations to cells");

    /**************************************************************/
    /* BIN CREATION AND STRETCHING                                */
    /**************************************************************/
    _STEP_BEGIN("Creating Bins");
    DesignCreateBins();
    _STEP_END("Creating Bins");

    _STEP_BEGIN("Stretching Bins");
    DesignStretchBins();
    _STEP_END("Stretching Bins");

    /**************************************************************/
    /* PRINT THE PEAK UTILIZATION VALUE                           */
    /**************************************************************/
    peakUtilization = DesignGetPeakUtil();
    peakUtilBinIdx = DesignGetPeakUtilBinIdx();
    if (prevPeakUtil == 0.0) {
       prevPeakUtil = peakUtilization;
    }
    cout << "Iteration: " << itrCount++
	 << " Peak Utilization: " << peakUtilization
	 << " Bin index: " << peakUtilBinIdx 
	 << " Mem usage: " << getMemUsage() << MEM_USAGE_UNIT
	 << " CPU TIME:" << getCPUTime() << CPU_TIME_UNIT << endl;

    /**************************************************************/
    /* STOPPING CONDITION                                         */
    /**************************************************************/
    if (1 || ((prevPeakUtil > peakUtilization) && 
	      ((prevPeakUtil - peakUtilization) < stopThreshold))) {
      cout << "Global placement complete" << endl;
      string plotFileName;
      plotFileName = DesignEnv.EnvGetDesignName() + ".gp.plt";
      DesignPlotData("Title", plotFileName);
      break;
    } else {
      prevPeakUtil = peakUtilization;
    }

    /**************************************************************/
    /* RESET LINEAR AND QUADRATIC VALUES                          */
    /**************************************************************/
    _STEP_BEGIN("Reset quadratic and linear term values");
    MAP_FOR_ALL_ELEMS(quadMap, Cell*, uint, cellPtr, qvalIdx) {
      qvalijx[qvalIdx] = quadMapX[qvalIdx];
      qvalijy[qvalIdx] = quadMapY[qvalIdx];
    } END_FOR;
    MAP_FOR_ALL_ELEMS(linMap, Cell*, uint, cellPtr, qvalIdx) {
      qvalx[qvalIdx] = linMapX[qvalIdx];
      qvaly[qvalIdx] = linMapY[qvalIdx];
    } END_FOR;
    _STEP_END("Reset quadratic and linear term values");

    /**************************************************************/
    /* ADD SPREADING FORCES                                       */
    /**************************************************************/
    _STEP_BEGIN("Perform cell spreading");
    double cellXpos, cellYpos;
    double pseudoPinX, pseudoPinY;
    double springConstant;
    double coeffX, coeffY;
    uint quadCellIdx, linCellIdx;
    for (i = 0; i < numVars; i++) {
      Cell &thisCell = (*(Cell *)(inputCells[i]));
      Bin &cellBin = *((Bin *)CellGetBin(&thisCell));
      cellXpos = CellGetDblX(&thisCell);
      cellYpos = CellGetDblY(&thisCell);
      DesignSpreadCreatePseudoPort(thisCell, cellBin, cellXpos, cellYpos,
				   pseudoPinX, pseudoPinY, springConstant);
      coeffX = 2 * springConstant; coeffY = coeffX;
      _KEY_EXISTS_WITH_VAL(quadMap, (&thisCell), quadMapItr) {
	quadCellIdx = quadMapItr->second;
      } else {
	cout << "SEVERE ERROR QUAD: ENTRY FOR cell: (PTR: " << &thisCell << ") "
	     << thisCell.CellGetName() << " not found in quadMap" << endl;
	exit(0);
      }
      _KEY_EXISTS_WITH_VAL(linMap, (&thisCell), linMapItr) {
	linCellIdx = linMapItr->second;
      } else {
	cout << "SEVERE ERROR LINEAR: ENTRY FOR cell: (PTR: " << &thisCell << ") "
	     << thisCell.CellGetName() << " not found in linMap" << endl;
	exit(0);
      }

      /* Update the diagonals of the quadratic matrix */
      qvalijx[quadCellIdx] += coeffX;
      qvalijy[quadCellIdx] += coeffY;
      /* Update the linear matrix */
      coeffX = coeffX * (-pseudoPinX / GRID_COMPACTION_RATIO);
      coeffY = coeffY * (-pseudoPinY / GRID_COMPACTION_RATIO);

      qvalx[linCellIdx] += (coeffX);
      qvaly[linCellIdx] += (coeffY);
    }
    _STEP_END("Perform cell spreading");

    /**************************************************************/
    /* REMOVE BINS                                                */
    /**************************************************************/
    _STEP_BEGIN("Perform bin removal");
    DesignClearBins();
    _STEP_END("Perform bin removal");
  }
  _STEP_END("Analytical solve and spread iterations");
  MSK_deletetask(&taskx);
  MSK_deleteenv(&envx);
  MSK_deletetask(&tasky);
  MSK_deleteenv(&envy);
}

# if 0
void 
Design::DesignSolveForAllCellsMosekIterSolveBoth()
{
  void *cellObj;
  ofstream logFile;
  vector<Cell *> inputCells;
  vector<Cell *> cellsToSolve;
  vector<Cell *> cellsSortedByLeft;
  map<Cell *, uint> quadMap;
  map<Cell *, uint> linMap;
  map<uint, double> quadMap;
  map<uint, double> linMap;
  map<Cell *, double>::iterator quadMapItr;
  map<Cell *, double>::iterator linMapItr;
  string DesignPath, logFileName;
  double constantx, constanty;
  double peakUtilization, prevPeakUtil;
  uint numVars, numCstrs;
  uint numValuesQuad, numValuesLin;
  uint peakUtilBinIdx;
  uint nodeIdx;
  uint i;

  /* All the initilizations */
  Env &DesignEnv = (*this).DesignEnv;
  DesignPath = DesignEnv.EnvGetDesignPath();

  HyperGraph &myGraph = (*this).DesignGetGraph();

  /* Insert cells into a vector in the order of their indices in the 
     hypergraph */ 
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    cellsToSolve.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;

  DesignSetCellsToSolve(inputCells);

  /* Define MOSEK variables */
  MSKint32t *qsubi, *qsubj, *qsub;
  MSKint32t *subb;
  MSKrealt *qvalij, *qval;
  MSKrealt *bu, *bl;
  MSKboundkeye *bk;
  MSKint32t numBounds;
  
  /* Build the quadratic objective matrix for the cells */
  getObjFuncInMosekFormatBoth((*this), myGraph, inputCells, 
			      qsubi, qsubj, qvalij,
			      qsub, qval,
			      numValuesQuad, numValuesLin, 
			      constantx, constantx, quadMap, linMap,
			      DesignEnv.EnvGetNetModel());

  /* Get the variable bounds */
  getVarBoundsInMosekFormat((*this), inputCells, &subbx, &blx, &bux, &bkx, 
			    &subby, &bly, &buy, &bky, numXbounds, numYbounds);

  /* STORE THE ORIGINAL VALUES OF THE QUADRATIC TERMS IN A MAP */
  /* THIS WOULD BE A KEY-VALUE: INDEX-VALUE PAIR WHICH WOULD   */
  /* THEN BE USED TO REPLACE THE VALUES IN THE QVALIJ MATRIX   */
  Cell *cellPtr;
  uint qvalIdx;
  MAP_FOR_ALL_ELEMS(quadMap, Cell*, uint, cellPtr, qvalIdx) {
    quadMapX[qvalIdx] = qvalijx[qvalIdx];
    quadMapY[qvalIdx] = qvalijy[qvalIdx];
  } END_FOR;
  MAP_FOR_ALL_ELEMS(linMap, Cell*, uint, cellPtr, qvalIdx) {
    linMapX[qvalIdx] = qvalx[qvalIdx];
    linMapY[qvalIdx] = qvaly[qvalIdx];
  } END_FOR;

  numCstrs = 0;
  numVars = inputCells.size();

  /* MOSEK ENV */
  MSKenv_t envx, envy;
  MSKtask_t taskx, tasky;
  MSKrescodee r = MSK_RES_OK;
  bool opRes;
  uint itrCount;
  double stopThreshold;
  double x[numVars];
  double y[numVars];

  /**************************************************************/
  /* INITIALIZE MOSEK                                           */
  /**************************************************************/
  if (!initMosek(&envx, &taskx, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return;
  }
  if (!initMosek(&envy, &tasky, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return;
  }

  opRes = putVarBounds(&taskx, numXbounds, subbx, bux, blx, bkx);
  opRes = putVarBounds(&tasky, numXbounds, subby, buy, bly, bky);
  /**************************************************************/
  /* LOOP OF QUADRATIC SOLVING AND SPREADING BEINGS HERE        */
  /**************************************************************/
  prevPeakUtil = 0.0;
  itrCount = 0;
  stopThreshold = 0.5;
  _STEP_BEGIN("Analytical solve and spread iterations");
  while (1) {
    /**************************************************************/
    /* SOLVER PART BEGINS HERE  :  SOLVE FOR X FIRST AND THEN Y   */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for X");
    if (opRes) r = MSK_putqobj(taskx, numValuesQuad, qsubix, qsubjx, qvalijx);
    if (r == MSK_RES_OK) r = MSK_putclist(taskx, numValuesLin, qsubx, qvalx);
    if (r == MSK_RES_OK) r = MSK_putobjsense(taskx , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for X");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in X direction");
      r = MSK_optimizetrm(taskx, &trmcode);
      _STEP_END("Optimizing in X direction");
      MSK_solutionsummary(taskx, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolsta(taskx, MSK_SOL_ITR, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(taskx, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, x);
	}
      }
    }

    /**************************************************************/
    /* SOLVE FOR Y NEXT                                           */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for Y");
    if (opRes) r = MSK_putqobj(tasky, numValuesQuad, qsubiy, qsubjy, qvalijy);
    if (r == MSK_RES_OK) r = MSK_putclist(tasky, numValuesLin, qsuby, qvaly);
    if (r == MSK_RES_OK) r = MSK_putobjsense(tasky , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for Y");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in Y direction");
      r = MSK_optimizetrm(tasky, &trmcode);
      _STEP_END("Optimizing in Y direction ");
      MSK_solutionsummary(tasky, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolsta(tasky, MSK_SOL_ITR, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(tasky, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, y);
	}
      }
    }

    /**************************************************************/
    /* WRITE CELL LOCATIONS TO LOG FILE IN EACH ITERATION         */
    /* IN THE TEST MODE ONLY                                      */
    /**************************************************************/
    if (DesignEnv.EnvGetToolMode() == ENV_MODE_TEST) {
      logFileName = DesignPath + ".toolData/MoselSolX_Itr" + getStrFromInt(itrCount) + ".txt";
      logFile.open(logFileName.data());
      for (i = 0; i < numVars; i++) {
	logFile << "X" << i << " :" << x[i] << endl;
      }
      logFile.close();
      logFileName = DesignPath + ".toolData/MoselSolY_Itr" + getStrFromInt(itrCount) + ".txt";
      logFile.open(logFileName.data());
      for (i = 0; i < numVars; i++) {
	logFile << "Y" << i << " :" << y[i] << endl;
      }
      logFile.close();
    }

    /**************************************************************/
    /* ASSIGN LOCATIONS TO CELLS                                  */
    /**************************************************************/
    _STEP_BEGIN("Assigning locations to cells");
    Cell *cellPtr;
    double xpos, ypos;
    for (i = 0; i < numVars; i++) {
      cellPtr = inputCells[i];
      xpos = x[i] * GRID_COMPACTION_RATIO; ypos = y[i] * GRID_COMPACTION_RATIO;
      (*cellPtr).CellSetXpos(dtoi(xpos));
      (*cellPtr).CellSetYpos(dtoi(ypos));
      CellSetDblX(cellPtr, xpos);
      CellSetDblY(cellPtr, ypos);
    }
    _STEP_END("Assigning locations to cells");

    /**************************************************************/
    /* BIN CREATION AND STRETCHING                                */
    /**************************************************************/
    _STEP_BEGIN("Creating Bins");
    DesignCreateBins();
    _STEP_END("Creating Bins");

    _STEP_BEGIN("Stretching Bins");
    DesignStretchBins();
    _STEP_END("Stretching Bins");

    /**************************************************************/
    /* PRINT THE PEAK UTILIZATION VALUE                           */
    /**************************************************************/
    peakUtilization = DesignGetPeakUtil();
    peakUtilBinIdx = DesignGetPeakUtilBinIdx();
    if (prevPeakUtil == 0.0) {
       prevPeakUtil = peakUtilization;
    }
    cout << "Iteration: " << itrCount++
	 << " Peak Utilization: " << peakUtilization
	 << " Bin index: " << peakUtilBinIdx 
	 << " Mem usage: " << getMemUsage() << MEM_USAGE_UNIT
	 << " CPU TIME:" << getCPUTime() << CPU_TIME_UNIT << endl;

    /**************************************************************/
    /* STOPPING CONDITION                                         */
    /**************************************************************/
    if ((prevPeakUtil > peakUtilization) && 
	((prevPeakUtil - peakUtilization) < stopThreshold)) {
      cout << "Global placement complete" << endl;
      string plotFileName;
      plotFileName = DesignEnv.EnvGetDesignName() + "gp.plt";
      DesignPlotData("Title", plotFileName);
    } else {
      prevPeakUtil = peakUtilization;
    }

    /**************************************************************/
    /* RESET LINEAR AND QUADRATIC VALUES                          */
    /**************************************************************/
    _STEP_BEGIN("Reset quadratic and linear term values");
    MAP_FOR_ALL_ELEMS(quadMap, Cell*, uint, cellPtr, qvalIdx) {
      qvalijx[qvalIdx] = quadMapX[qvalIdx];
      qvalijy[qvalIdx] = quadMapY[qvalIdx];
    } END_FOR;
    MAP_FOR_ALL_ELEMS(linMap, Cell*, uint, cellPtr, qvalIdx) {
      qvalx[qvalIdx] = linMapX[qvalIdx];
      qvaly[qvalIdx] = linMapY[qvalIdx];
    } END_FOR;
    _STEP_END("Reset quadratic and linear term values");

    /**************************************************************/
    /* ADD SPREADING FORCES                                       */
    /**************************************************************/
    _STEP_BEGIN("Perform cell spreading");
    double cellXpos, cellYpos;
    double pseudoPinX, pseudoPinY;
    double springConstant;
    double coeffX, coeffY;
    uint quadCellIdx, linCellIdx;
    for (i = 0; i < numVars; i++) {
      Cell &thisCell = (*(Cell *)(inputCells[i]));
      if (CellIsStarNode(&thisCell)) break;
      Bin &cellBin = *((Bin *)CellGetBin(&thisCell));
      cellXpos = CellGetDblX(&thisCell);
      cellYpos = CellGetDblY(&thisCell);
      DesignSpreadCreatePseudoPort(thisCell, cellBin, cellXpos, cellYpos,
				   pseudoPinX, pseudoPinY, springConstant);

      coeffX = 2 * springConstant; coeffY = coeffX;
      _KEY_EXISTS_WITH_VAL(quadMap, (&thisCell), quadMapItr) {
	quadCellIdx = quadMapItr->second;
      } else {
	cout << "SEVERE ERROR QUAD: ENTRY FOR cell: (PTR: " << &thisCell << ") "
	     << thisCell.CellGetName() << " not found in quadMap" << endl;
	exit(0);
      }
      _KEY_EXISTS_WITH_VAL(linMap, (&thisCell), linMapItr) {
	linCellIdx = linMapItr->second;
      } else {
	cout << "SEVERE ERROR LINEAR: ENTRY FOR cell: (PTR: " << &thisCell << ") "
	     << thisCell.CellGetName() << " not found in linMap" << endl;
	exit(0);
      }

      /* Update the diagonals of the quadratic matrix */
      qvalijx[quadCellIdx] += coeffX;
      qvalijy[quadCellIdx] += coeffY;

      /* Update the linear matrix */
      coeffX = coeffX * (-pseudoPinX / GRID_COMPACTION_RATIO);
      coeffY = coeffY * (-pseudoPinY / GRID_COMPACTION_RATIO);
      qvalx[linCellIdx] += (coeffX);
      qvaly[linCellIdx] += (coeffY);
    }
    _STEP_END("Perform cell spreading");

    /**************************************************************/
    /* REMOVE BINS                                                */
    /**************************************************************/
    _STEP_BEGIN("Perform bin removal");
    DesignClearBins();
    _STEP_END("Perform bin removal");
  }
  _STEP_END("Analytical solve and spread iterations");
  MSK_deletetask(&taskx);
  MSK_deleteenv(&envx);
  MSK_deletetask(&tasky);
  MSK_deleteenv(&envy);
}

# endif
