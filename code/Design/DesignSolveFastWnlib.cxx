# include <Design.h>
# include <ConjGradSolver.h>
# include <wnlib/wnconj.h>
# include <cblas.h>

SpMat globalSpMatX;
SpMat globalSpMatY;
double *bMatX;
double *bMatY;

double objFuncX(double *x)
{
  double rtv;
  uint len;

  /* Compute xT . A . x + bT * x */
  len = globalSpMatX.GetNumDiagonalElems();
  double *p = new double[len];
  double *b = bMatX;

  mult(globalSpMatX, x, p);
  rtv = ddot(len,x,1,p,1);
  rtv+= ddot(len,x,1,b,1);
  delete []p;

  return (rtv);
}

void 
gradFuncX(double *grad, double *x) 
{
  uint len = globalSpMatX.GetNumDiagonalElems();
  /* Compute A . x + bT  which is the gradient */
  double *b = bMatX;

  mult(globalSpMatX, x, grad);
  daxpy(len, 1.0, b, 1, grad, 1);
}

double objFuncY(double *y)
{
  double rtv;
  uint len;

  /* Compute xT . A . x + bT * x */
  len = globalSpMatY.GetNumDiagonalElems();
  double *p = new double[len];
  double *b = bMatY;

  mult(globalSpMatY, y, p);
  rtv = ddot(len,y,1,p,1);
  rtv+= ddot(len,y,1,b,1);
  delete []p;

  return (rtv);
}

void 
gradFuncY(double *grad, double *y) 
{
  uint len = globalSpMatY.GetNumDiagonalElems();
  /* Compute A . x + bT  which is the gradient */
  double *b = bMatY;

  mult(globalSpMatY, y, grad);
  daxpy(len, 1.0, b, 1, grad, 1);
}


void
getObjFuncInConjGradFormatNew(Design &myDesign, HyperGraph &myGraph,
			      vector<Cell *> &inputCells, 
			      SpMat **matX, SpMat **matY, 
			      double **bx, double **by,
			      double &constantx, double &constanty,
			      map<Cell *, uint>& quadMap,
			      map<Cell *, uint>& linMap,
			      EnvNetModel netModel)
{
  vector<uint> subi_vecx, subj_vecx, sub_vecx;
  vector<uint> subi_vecy, subj_vecy, sub_vecy;
  vector<double> valij_vecx, val_vecx;
  vector<double> valij_vecy, val_vecy;
  uint numVars, i;

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

  numVars = inputCells.size();
  *matX = new SpMat(subi_vecx, subj_vecx, valij_vecx);
  *matY = new SpMat(subi_vecy, subj_vecy, valij_vecy);
  *bx = (double *) malloc(sizeof(double) * numVars);
  *by = (double *) malloc(sizeof(double) * numVars);
  
  for (i = 0; i < numVars; i++) {
    (*bx)[i] = 0.0;
    (*by)[i] = 0.0;
  }
  for (i = 0; i < sub_vecx.size(); i++) {
    (*bx)[sub_vecx[i]] = -val_vecx[i];
  }
  for (i = 0; i < sub_vecy.size(); i++) {
    (*by)[sub_vecy[i]] = -val_vecy[i];
  }
}

void
Design::DesignSolveForAllCellsConjGradWnLibIter(void)
{
  void *cellObj;
  ofstream logFile;
  vector<Cell *> inputCells;
  map<Cell *, uint> quadMap;
  map<Cell *, uint>::iterator quadMapItr;
  map<uint, double> quadMapX, quadMapY;
  map<Cell *, uint> linMap;
  map<Cell *, uint>::iterator linMapItr;
  map<uint, double> linMapX, linMapY;
  string DesignPath, DesignName, logFileName;
  string DirName;
  string plotFileName;
  double prevPeakUtil, itrCount, stopThreshold;
  uint peakUtilBinIdx;
  uint numVars, nodeIdx;
  uint qvalIdx, i;
  SpMat *matX, *matY;
  double *bx, *by;
  double constantx, constanty;
  
  /* All the initilizations */
  Env &DesignEnv = (*this).DesignEnv;
  DesignPath = DesignEnv.EnvGetDesignPath();
  DirName = DesignPath + "/.solverData";
  DesignName = DesignEnv.EnvGetDesignName();

  HyperGraph &myGraph = (*this).DesignGetGraph();

  /* Insert cells into a vector in the order of their indices in the 
     hypergraph */ 
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    cellsToSolve.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;

  DesignSetCellsToSolve(inputCells);

  getObjFuncInConjGradFormatNew((*this), myGraph, inputCells, &matX, &matY, 
				&bx, &by, constantx, constanty, quadMap,
				linMap, DesignEnv.EnvGetNetModel());

  /* STORE THE ORIGINAL VALUES OF THE QUADRATIC TERMS IN A MAP */
  /* THIS WOULD BE A KEY-VALUE: INDEX-VALUE PAIR WHICH WOULD   */
  /* THEN BE USED TO REPLACE THE VALUES IN THE QVALIJ MATRIX   */
  Cell *cellPtr;
  MAP_FOR_ALL_ELEMS(quadMap, Cell*, uint, cellPtr, qvalIdx) {
    quadMapX[qvalIdx] = (*matX).GetValue(qvalIdx);
    quadMapY[qvalIdx] = (*matY).GetValue(qvalIdx);
  } END_FOR;
  MAP_FOR_ALL_ELEMS(linMap, Cell*, uint, cellPtr, qvalIdx) {
    linMapX[qvalIdx] = bx[qvalIdx];
    linMapY[qvalIdx] = by[qvalIdx];
  } END_FOR;
  
  numVars = inputCells.size();
  double x[numVars];
  double y[numVars];
  double eps=1e-13;
  for (i = 0; i < numVars; i++) {
    x[i] = 0.0;
    y[i] = 0.0;
  }

  /**************************************************************/
  /* LOOP OF QUADRATIC SOLVING AND SPREADING BEINGS HERE        */
  /**************************************************************/
  prevPeakUtil = 0.0;
  itrCount = 0;
  stopThreshold = 0.1;
  uint max_iterations = INT_MAX;
  int code;
  double val_min;
  _STEP_BEGIN("Analytical solve and spread iterations");
  while (1) {
    bMatX = new double[numVars];
    bMatY = new double[numVars];
    globalSpMatX = (*matX);
    globalSpMatY = (*matY);
    for (i = 0; i < numVars; i++) bMatX[i] = -bx[i];
    for (i = 0; i < numVars; i++) bMatY[i] = -by[i];
    /**************************************************************/
    /* SOLVER PART BEGINS HERE  :  SOLVE FOR X FIRST AND THEN Y   */
    /**************************************************************/
    wn_conj_gradient_method(&code, &val_min, x, numVars, objFuncX, gradFuncX, 
			    max_iterations);
    if (code == WN_SUCCESS) {
      cout << "Optimization successfully done" << endl;
    } else {
      cout << "Optimization unsuccessfull" << endl;
      exit(0);
    }

    /**************************************************************/
    /* SOLVER PART BEGINS HERE  :  SOLVE FOR X FIRST AND THEN Y   */
    /**************************************************************/
    wn_conj_gradient_method(&code, &val_min, y, numVars, objFuncY, gradFuncY, 
			    max_iterations);
    if (code == WN_SUCCESS) {
      cout << "Optimization successfully done" << endl;
    } else {
      cout << "Optimization unsuccessfull" << endl;
      exit(0);
    }
    exit(0);
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
      logFileName = DirName + "/ConjGradX_Itr" + getStrFromInt(itrCount) + ".txt";
      logFile.open(logFileName.data());
      for (i = 0; i < numVars; i++) {
	logFile << "X" << i << ": " << x[i] << endl;
      }
      logFile.close();
      logFileName = DirName + "/ConjGradY_Itr" + getStrFromInt(itrCount) + ".txt";
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
      xpos = x[i] * GRID_COMPACTION_RATIO; 
      ypos = y[i] * GRID_COMPACTION_RATIO;
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
    if ((itrCount < 0) || 
	((prevPeakUtil > peakUtilization) && 
	 ((prevPeakUtil - peakUtilization) < stopThreshold))) {
      cout << "Global placement complete" << endl;
      plotFileName = DesignName + ".gp.plt";
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
      (*matX).SetValue(qvalIdx, quadMapX[qvalIdx]);
      (*matY).SetValue(qvalIdx, quadMapY[qvalIdx]);
    } END_FOR;
    MAP_FOR_ALL_ELEMS(linMap, Cell*, uint, cellPtr, qvalIdx) {
      bx[qvalIdx] = linMapX[qvalIdx];
      by[qvalIdx] = linMapY[qvalIdx];
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
      //      cout << "Created pseudo port at: (" << pseudoPinX << "," << pseudoPinY << ")   Spring constant:" << springConstant << endl;
      coeffX = springConstant; coeffY = coeffX;
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
      //      cout << "Updated the quad value of cell index " << quadCellIdx << " from " << (*matX).GetValue(quadCellIdx);
      (*matX).AddValue(quadCellIdx, coeffX);
      //      cout << " to " << (*matX).GetValue(quadCellIdx) << endl;
      //      cout << "Updated the quad value of cell index " << quadCellIdx << " from " << (*matY).GetValue(quadCellIdx);
      (*matY).AddValue(quadCellIdx, coeffY);
      //cout << " to " << (*matY).GetValue(quadCellIdx) << endl;
      
      /* Update the linear matrix */
      coeffX = coeffX * (pseudoPinX / GRID_COMPACTION_RATIO);
      coeffY = coeffY * (pseudoPinY / GRID_COMPACTION_RATIO);
      //      cout << "Updated the lin value of cell index " << linCellIdx << " from " << bx[linCellIdx];
      bx[i] += (coeffX);
      //      cout << " to " << bx[linCellIdx] << endl;
      //      cout << "Updated the lin value of cell index " << linCellIdx << " from " << by[linCellIdx];
      by[i] += (coeffY);
      //      cout << " to " << by[linCellIdx] << endl;
    }
    _STEP_END("Perform cell spreading");

    /**************************************************************/
    /* REMOVE BINS                                                */
    /**************************************************************/
    _STEP_BEGIN("Perform bin removal");
    DesignClearBins();
    _STEP_END("Perform bin removal");

    for (i = 0; i < numVars; i++) {
      x[i] = 0.0;
      y[i] = 0.0;
    }
  }
  _STEP_END("Analytical solve and spread iterations");
}
