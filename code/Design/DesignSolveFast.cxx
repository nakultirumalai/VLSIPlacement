# include <Design.h>
# include <mosek.h>

void
MSKAPI printDesignSolverOutput(void *handle, char str[])
{
  printf("%s", str);
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
			map<Cell *, uint>& linMap)
{
  vector<uint> subi_vecx, subj_vecx, sub_vecx;
  vector<uint> subi_vecy, subj_vecy, sub_vecy;
  vector<double> valij_vecx, val_vecx;
  vector<double> valij_vecy, val_vecy;
  uint i;

  getObjectiveCliqueModelXY(myDesign, myGraph, inputCells, 
			    subi_vecx, subj_vecx, valij_vecx, 
			    sub_vecx, val_vecx, 
			    subi_vecy, subj_vecy, valij_vecy, 
			    sub_vecy, val_vecy, 
			    constantx, constanty, quadMap, linMap);

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

  //  cout << "subix\tsubjx\tvalijx\tsubiy\tsubjy\tvalijy" << endl;
  for (i = 0; i < subi_vecx.size(); i++) {
    (*qsubix)[i] = subi_vecx[i];
    (*qsubjx)[i] = subj_vecx[i];
    (*qvalijx)[i] = 2 * valij_vecx[i];
    (*qsubiy)[i] = subi_vecy[i];
    (*qsubjy)[i] = subj_vecy[i];
    (*qvalijy)[i] = 2 * valij_vecy[i];
    //    cout << (*qsubix)[i] << "\t" << (*qsubjx)[i] << "\t" 
    //    << (*qvalijx)[i] << "\t" << (*qsubiy)[i] << "\t"
    //    << (*qsubjy)[i] << "\t" << (*qvalijy)[i] << endl;
  }
  numValuesQuad = subi_vecx.size();

  //  cout << "subx\tvalx\tsuby\tvaly" << endl;
  cout << "subx\tvalx" << endl;
  for (i = 0; i < sub_vecx.size(); i++) {
    (*qsubx)[i] = sub_vecx[i];
    (*qvalx)[i] = 2 * val_vecx[i];
    (*qsuby)[i] = sub_vecy[i];
    (*qvaly)[i] = 2 * val_vecy[i];
    cout << (*qsubx)[i] << "\t" << (*qvalx)[i] <<endl; //<< "\t" 
      //	 << (*qsuby)[i] << "\t" << (*qvaly)[i] << endl;
  }

  numValuesLin = sub_vecx.size();
}

bool
initMosek(MSKenv_t *env, MSKtask_t *task, uint numVars, uint numCstrs)

{
  MSKrescodee r;
  bool rtv;

  rtv = true;
  r = MSK_makeenv(env, NULL, NULL, NULL, NULL);
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
      if (r == MSK_RES_OK) r = MSK_putmaxnumvar(*task, numVars);
      if (r == MSK_RES_OK) r = MSK_putmaxnumcon(*task, numCstrs);
      if (r == MSK_RES_OK) r = MSK_putmaxnumanz(*task, numVars);
      if (r == MSK_RES_OK) r = MSK_append(*task, MSK_ACC_CON, numCstrs);
      if (r == MSK_RES_OK) r = MSK_append(*task, MSK_ACC_VAR, numVars);
    } else rtv = false;
  } else rtv = false;
  
  return (rtv);
}

bool 
putVarBounds(MSKtask_t *task, uint numVarBounds,
	     MSKidxt *subb, MSKrealt *bu, 
	     MSKrealt *bl, MSKboundkeye *bk)
{
  MSKrescodee r;
  bool rtv;

  rtv = true;
  r = MSK_putboundlist(*task, MSK_ACC_VAR, numVarBounds, subb, bk, bl, bu);
  if (r != MSK_RES_OK) {
    rtv = false;
  }
  
  return (rtv);
}

void 
Design::DesignSolveForAllCellsIterOld()
{
  void *cellObj;
  vector<Cell *> inputCells;
  vector<Cell *> cellsToSolve;
  vector<Cell *> cellsSortedByLeft;
  map<Cell *, uint> quadMap;
  map<Cell *, uint> linMap;
  map<uint, double> quadMapX, quadMapY;
  map<uint, double> linMapX, linMapY;
  double constantx, constanty;
  double peakUtilization, prevPeakUtil;
  uint numVars, numCstrs;
  uint numValuesQuad, numValuesLin;
  uint peakUtilBinIdx;
  uint nodeIdx;
  uint i;

  HyperGraph &myGraph = DesignGetGraph();

  /* Insert cells into a vector in the order of their indices in the 
     hypergraph */ 
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    cellsToSolve.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;

  DesignSetCellsToSolve(inputCells);
  numCstrs = 0;
  numVars = inputCells.size();

  /* Define MOSEK variables */
  MSKidxt *qsubix, *qsubjx, *qsubx;
  MSKidxt *qsubiy, *qsubjy, *qsuby;
  MSKidxt *subbx, *subby;
  MSKrealt *qvalijx, *qvalx;
  MSKrealt *qvalijy, *qvaly;
  MSKrealt *bux, *blx, *buy, *bly;
  MSKboundkeye *bkx, *bky;
  uint numXbounds, numYbounds;
  
  /* Build the quadratic objective matrix for the cells */
  getObjFuncInMosekFormat((*this), myGraph, inputCells, 
			  &qsubix, &qsubjx, &qsubx, 
			  &qsubiy, &qsubjy, &qsuby, 
			  &qvalijx, &qvalx, &qvalijy, 
			  &qvaly, numValuesQuad, numValuesLin, 
			  constantx, constantx, quadMap, linMap);

  /* Get the variable bounds */
  genGetVarBounds((*this), inputCells, &subbx, &bux, &blx, &bkx, 
		  numXbounds, SOLVE_FOR_X);
  genGetVarBounds((*this), inputCells, &subby, &buy, &bly, &bky, 
		  numYbounds, SOLVE_FOR_Y);

  /* Get the bounds for the variables */
  /* After obtaining the objective, initialize the difference vars */
  vector<double> diffLinearX(numValuesLin);
  vector<double> diffLinearY(numValuesLin);
  vector<double> diffQuadX(numValuesQuad);
  vector<double> diffQuadY(numValuesQuad);
  for (i = 0; i < numValuesLin; i++) {
    diffLinearX[i] = 0.0; diffLinearY[i] = 0.0;
  }
  for (i = 0; i < numValuesQuad; i++) {
    diffQuadX[i] = 0.0; diffQuadX[i] = 0.0;
  }

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

  /* MOSEK ENV */
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;
  bool opRes;
  uint itrCount;
  double stopThreshold;
  double x[numVars];
  double y[numVars];

  /**************************************************************/
  /* INITIALIZE MOSEK                                           */
  /**************************************************************/
  if (!initMosek(&env, &task, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return;
  }

  /**************************************************************/
  /* LOOP OF QUADRATIC SOLVING AND SPREADING BEINGS HERE        */
  /**************************************************************/
  prevPeakUtil = 0.0;
  itrCount = 0;
  stopThreshold = 0.5;
  exit(0);
  _STEP_BEGIN("Analytical solve and spread iterations");
  while (1) {
    /**************************************************************/
    /* SOLVER PART BEGINS HERE  :  SOLVE FOR X FIRST AND THEN Y   */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for X");
    opRes = putVarBounds(&task, numXbounds, subbx, bux, blx, bkx);
    if (opRes) r = MSK_putqobj(task, numValuesQuad, qsubix, qsubjx, qvalijx);
    if (r == MSK_RES_OK) r = MSK_putclist(task, numValuesLin, qsubx, qvalx);
    if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for X");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in X direction");
      r = MSK_optimizetrm(task, &trmcode);
      _STEP_END("Optimizing in X direction");
      MSK_solutionsummary(task, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolutionstatus(task, MSK_SOL_ITR, NULL, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, x);
	}
      }
    }

    /**************************************************************/
    /* SOLVE FOR Y NEXT                                           */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for Y");
    opRes = putVarBounds(&task, numYbounds, subby, buy, bly, bky);
    if (opRes) r = MSK_putqobj(task, numValuesQuad, qsubiy, qsubjy, qvalijy);
    if (r == MSK_RES_OK) r = MSK_putclist(task, numValuesLin, qsuby, qvaly);
    if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for Y");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in Y direction");
      r = MSK_optimizetrm(task, &trmcode);
      _STEP_END("Optimizing in Y direction ");
      MSK_solutionsummary(task, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolutionstatus(task, MSK_SOL_ITR, NULL, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, y);
	}
      }
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
    /* ASSIGN LOCATIONS TO CELLS                                  */
    /**************************************************************/
    _STEP_BEGIN("Creating Bins");
    DesignCreateBins(8000,4000);
    _STEP_END("Creating Bins");

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
	 << " Mem usage: " << getMemUsage() << MEM_USAGE_UNIT << endl;

    /**************************************************************/
    /* STOPPING CONDITION                                         */
    /**************************************************************/
    if ((prevPeakUtil > peakUtilization) && 
	((prevPeakUtil - peakUtilization) < stopThreshold)) {
      cout << "Global placement complete" << endl;
      string plotFileName;
      plotFileName = "gp.plt";
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
    DesignSpreadCellsFast(myGraph, qvalijx, qvalijy, qvalx, qvaly, 
    			  quadMap, linMap);
    _STEP_END("Perform cell spreading");

    /**************************************************************/
    /* REMOVE BINS                                                */
    /**************************************************************/
    _STEP_BEGIN("Perform bin removal");
    DesignClearBins();
    _STEP_END("Perform bin removal");
  }
  _STEP_END("Analytical solve and spread iterations");
  MSK_deletetask(&task);
  MSK_deleteenv(&env);
}
  
void 
Design::DesignSolveForAllCellsIter()
{
  void *cellObj;
  vector<Cell *> inputCells;
  vector<Cell *> cellsToSolve;
  vector<Cell *> cellsSortedByLeft;
  map<Cell *, uint> quadMap;
  map<Cell *, uint> linMap;
  map<uint, double> quadMapX, quadMapY;
  map<uint, double> linMapX, linMapY;
  double constantx, constanty;
  double peakUtilization, prevPeakUtil;
  uint numVars, numCstrs;
  uint numValuesQuad, numValuesLin;
  uint peakUtilBinIdx;
  uint nodeIdx;
  uint i;

  HyperGraph &myGraph = DesignGetGraph();

  /* Insert cells into a vector in the order of their indices in the 
     hypergraph */ 
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    cellsToSolve.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;

  DesignSetCellsToSolve(inputCells);
  numCstrs = 0;
  numVars = inputCells.size();

  /* Define MOSEK variables */
  MSKidxt *qsubix, *qsubjx, *qsubx;
  MSKidxt *qsubiy, *qsubjy, *qsuby;
  MSKidxt *subbx, *subby;
  MSKrealt *qvalijx, *qvalx;
  MSKrealt *qvalijy, *qvaly;
  MSKrealt *bux, *blx, *buy, *bly;
  MSKboundkeye *bkx, *bky;
  uint numXbounds, numYbounds;
  
  /* Build the quadratic objective matrix for the cells */
  getObjFuncInMosekFormat((*this), myGraph, inputCells, 
			  &qsubix, &qsubjx, &qsubx, 
			  &qsubiy, &qsubjy, &qsuby, 
			  &qvalijx, &qvalx, &qvalijy, 
			  &qvaly, numValuesQuad, numValuesLin, 
			  constantx, constantx, quadMap, linMap);

  /* Get the variable bounds */
  genGetVarBounds((*this), inputCells, &subbx, &bux, &blx, &bkx, 
		  numXbounds, SOLVE_FOR_X);
  genGetVarBounds((*this), inputCells, &subby, &buy, &bly, &bky, 
		  numYbounds, SOLVE_FOR_Y);

  /* Get the bounds for the variables */
  /* After obtaining the objective, initialize the difference vars */
  vector<double> diffLinearX(numValuesLin);
  vector<double> diffLinearY(numValuesLin);
  vector<double> diffQuadX(numValuesQuad);
  vector<double> diffQuadY(numValuesQuad);
  for (i = 0; i < numValuesLin; i++) {
    diffLinearX[i] = 0.0; diffLinearY[i] = 0.0;
  }
  for (i = 0; i < numValuesQuad; i++) {
    diffQuadX[i] = 0.0; diffQuadX[i] = 0.0;
  }

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

  /* MOSEK ENV */
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;
  bool opRes;
  uint itrCount;
  double stopThreshold;
  double x[numVars];
  double y[numVars];

  /**************************************************************/
  /* INITIALIZE MOSEK                                           */
  /**************************************************************/
  if (!initMosek(&env, &task, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return;
  }

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
    opRes = putVarBounds(&task, numXbounds, subbx, bux, blx, bkx);
    if (opRes) r = MSK_putqobj(task, numValuesQuad, qsubix, qsubjx, qvalijx);
    if (r == MSK_RES_OK) r = MSK_putclist(task, numValuesLin, qsubx, qvalx);
    if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for X");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in X direction");
      r = MSK_optimizetrm(task, &trmcode);
      _STEP_END("Optimizing in X direction");
      MSK_solutionsummary(task, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolutionstatus(task, MSK_SOL_ITR, NULL, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, x);
	}
      }
    }

    /**************************************************************/
    /* SOLVE FOR Y NEXT                                           */
    /**************************************************************/
    _STEP_BEGIN("Adding objective and var bounds for Y");
    opRes = putVarBounds(&task, numYbounds, subby, buy, bly, bky);
    if (opRes) r = MSK_putqobj(task, numValuesQuad, qsubiy, qsubjy, qvalijy);
    if (r == MSK_RES_OK) r = MSK_putclist(task, numValuesLin, qsuby, qvaly);
    if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);
    _STEP_END("Adding objective and var bounds for Y");
    if (r == MSK_RES_OK) {
      MSKrescodee trmcode;
      _STEP_BEGIN("Optimizing in Y direction");
      r = MSK_optimizetrm(task, &trmcode);
      _STEP_END("Optimizing in Y direction ");
      MSK_solutionsummary(task, MSK_STREAM_LOG);
      if (r == MSK_RES_OK) {
	MSKsolstae solsta;
	MSK_getsolutionstatus(task, MSK_SOL_ITR, NULL, &solsta);
	if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	  MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, y);
	}
      }
    }

    break;
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
    /* ASSIGN LOCATIONS TO CELLS                                  */
    /**************************************************************/
    _STEP_BEGIN("Creating Bins");
    DesignCreateBins(8000,4000);
    _STEP_END("Creating Bins");
    peakUtilization = DesignGetPeakUtil();
    peakUtilBinIdx = DesignGetPeakUtilBinIdx();
    if (prevPeakUtil == 0.0) {
       prevPeakUtil = peakUtilization;
    }
    
    cout << "Iteration: " << itrCount++
	 << " Peak Utilization: " << peakUtilization
	 << " Bin index: " << peakUtilBinIdx 
	 << " Mem usage: " << getMemUsage() << MEM_USAGE_UNIT << endl;

    /**************************************************************/
    /* STOPPING CONDITION                                         */
    /**************************************************************/
    if ((prevPeakUtil > peakUtilization) && 
	((prevPeakUtil - peakUtilization) < stopThreshold)) {
      cout << "Global placement complete" << endl;
      string plotFileName;
      plotFileName = "gp.plt";
      DesignPlotData("Title", plotFileName);
      break;
    } else {
      prevPeakUtil = peakUtilization;
    }

    /**************************************************************/
    /* STRETCH BINS                                               */
    /**************************************************************/
    _STEP_BEGIN("Stretching Bins");
    DesignStretchBins();
    _STEP_END("Stretching Bins");

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
    for (i = 0; i < numVars; i++) {
      Cell &thisCell = (*(inputCells[i]));
      Bin &thisBin = *((Bin *)CellGetBin(&thisCell));
      DesignSpreadCreatePseudoPort(thisCell, thisBin,
				   x[i], y[i], qvalijx, qvalijy, qvalx, 
				   qvaly, quadMap, linMap);
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
  MSK_deletetask(&task);
  MSK_deleteenv(&env);
}
  
