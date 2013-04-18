# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <AnalyticalSolve.h>

# define RPERLENGTHX 1 // in milli ohms per length
# define CPERLENGTHX 1 // in micro farad per length
# define RPERLENGTHY 1 // in micro farad per length
# define CPERLENGTHY 1 // in micro farad per length
# define TCLK 50000
# define GRID_COMPACTION_RATIO 1000 // to convert to micro

void 
MSKAPI printstr(void *handle, char str[]) 
{
  printf("%s", str);
}

void
mskGetObjectiveMatrix(vector<Cell *>& inputCells, HyperGraph& connectivityGraph,
		      map<unsigned int, pair<Pin*, Pin* > >& pinsOfEdges,
		      MSKidxt **qsubi, MSKidxt **qsubj, MSKrealt **qval, 
		      unsigned int &numValues)
{
  int i, j;
  double coeffuv, coeffu;
  Cell *cellPtri, *cellPtrj;
  unsigned int numCells;
  unsigned int numConstraints;
  vector<unsigned int> qsubi_vec, qsubj_vec;
  vector<double> qval_vec;
  
  numCells = inputCells.size();
  numConstraints = connectivityGraph.GetNumEdges();

  vector<double> squaredTerms(2 * numCells);
  for (i = 0; i < (2 * numCells); i++) {
    squaredTerms.push_back(0.0);
  }

  for (i = 0; i < numCells; i++) {
    Cell *cellPtri = inputCells[i];
    unsigned int edgeIdx;
    double edgeWeight;
    double coeffX, coeffY;
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight)  {
      pair<Pin*, Pin*> pinPair = pinsOfEdges[edgeIdx];
      Pin *pin1 = pinPair.first; Pin *pin2 = pinPair.second;
      Cell* cellPtr1 = &((*pin1).PinGetParentCell()); Cell* cellPtr2 = &((*pin2).PinGetParentCell());
      if ((*cellPtr1).CellGetName() != (*cellPtri).CellGetName()) {
	continue;
      }
      Cell &cell1 = *cellPtr1; Cell &cell2 = *cellPtr2;
      coeffX = (RPERLENGTHX) * (CPERLENGTHX) * edgeWeight;
      coeffY = (RPERLENGTHY) * (CPERLENGTHY) * edgeWeight;
      
      squaredTerms[i]+= coeffX;
      squaredTerms[i+numCells]+= coeffY;
      if (!cell2.CellIsTerminal()) {
	/* Contributes to the ij term here */
	for (j = 0; j < numCells; j++) {
	  if (cellPtr2 == inputCells[j]) {
	    break;
	  }
	}
	squaredTerms[j] += coeffX;
	squaredTerms[j+numCells] += coeffY;
	if (j > i) {
	  i = i + j; j = i - j; i = i - j;
	}
	qsubi_vec.push_back(i); qsubj_vec.push_back(j); qval_vec.push_back(-coeffX); 
	qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(j+numCells); qval_vec.push_back(-coeffY);
     }
    } HYPERGRAPH_END_FOR;
  }
  
  for (i = 0; i < numCells; i++) {
    if (squaredTerms[i] != 0.0) {
      qsubi_vec.push_back(i);
      qsubj_vec.push_back(i);
      qval_vec.push_back(squaredTerms[i]);

      qsubi_vec.push_back(i+numCells);
      qsubj_vec.push_back(i+numCells);
      qval_vec.push_back(squaredTerms[i+numCells]);
    }
  }
  *qsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());
  
  bool debugPrint = false;
  //  bool debugPrint = true;
  if (debugPrint) cout << "  i\tj\tvalue " << endl;

  for (i = 0; i < qsubi_vec.size(); i++) {
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = 2 * qval_vec[i];
    if (debugPrint) cout << "  " << (*qsubi)[i] << "\t" << (*qsubj)[i] << "\t" << (*qval)[i] << endl;
  }

  /* Printing the quadratic constraints matrices */
  if (debugPrint) {
    double **kthMatrix = (double **) malloc(sizeof(double*) * 2 * numCells);
    for (i = 0; i < 2 * numCells; i++) {
      kthMatrix[i] = (double *) malloc(sizeof(double) * 2 * numCells);
      for (j = 0; j <= i; j++) {
	kthMatrix[i][j] = 0.0;
      }
    }
    for (i = 0; i < qsubi_vec.size(); i++) {
      kthMatrix[(int)(*qsubi)[i]][(int)(*qsubj)[i]] = (double)(*qval)[i];
    }
    for (int idx = 0; idx < 2 * numCells; idx++) {
      cout << "\t";
      for (int jdx = 0; jdx < 2 * numCells; jdx++) {
	if (idx >= jdx) {
	  cout << kthMatrix[idx][jdx] << "\t";
	  kthMatrix[idx][jdx] = 0.0;
	} else {
	  cout << kthMatrix[jdx][idx] << "\t";
	  kthMatrix[idx][jdx] = 0.0;
	}
      }
      cout << endl;
    }
    free(kthMatrix);
  }
  numValues = qsubi_vec.size();
}

void
mskGetQuadraticCstrMatrix(vector<Cell *>& inputCells, HyperGraph& connectivityGraph,
			  map<unsigned int, pair<Pin*, Pin* > >& pinsOfEdges,
			  MSKidxt **qsubk, MSKidxt **qsubi, MSKidxt **qsubj, 
			  MSKrealt **qval, unsigned int &numValues)
{
  Cell *cellPtri, *cellPtrj;
  int i, j;
  double coeffuv, coeffu;
  unsigned int numCells;
  unsigned int cstrNum;
  unsigned int numConstraints;
  vector<unsigned int> qsubk_vec, qsubi_vec, qsubj_vec;
  vector<double> qval_vec;

  numCells = inputCells.size();
  numConstraints = connectivityGraph.GetNumEdges();

  cstrNum = 0;
  for (i = 0; i < numCells; i++) {
    Cell *cellPtri = inputCells[i];
    unsigned int edgeIdx;
    double edgeWeight;
    double coeffX, coeffY;
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight) {
      pair<Pin*, Pin*> pinPair = pinsOfEdges[edgeIdx];
      
      Pin *pin1 = pinPair.first;
      Pin *pin2 = pinPair.second;
      
      Cell* cellPtr1 = &((*pin1).PinGetParentCell());
      Cell* cellPtr2 = &((*pin2).PinGetParentCell());
      
      if ((*cellPtr1).CellGetName() != (*cellPtri).CellGetName()) {
	continue;
      }
      
      Cell &cell1 = *cellPtr1;
      Cell &cell2 = *cellPtr2;
      
      coeffX = edgeWeight * (RPERLENGTHX) * (CPERLENGTHX); coeffY = edgeWeight * (RPERLENGTHY) * (CPERLENGTHY);

      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i); qsubj_vec.push_back(i); qval_vec.push_back(coeffX);
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(i+numCells); qval_vec.push_back(coeffY);

      /* Now cell1 cannot be a terminal cell; Check if cell2 is a terminal */
      if (!cell2.CellIsTerminal()) {
	/* Contributes to the ij term here */
	for (j = 0; j < numCells; j++) {
	  if (cellPtr2 == inputCells[j]) {
	    break;
	  }
	}
	qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(j); qsubj_vec.push_back(j); qval_vec.push_back(coeffX);
	qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(j+numCells); qsubj_vec.push_back(j+numCells); qval_vec.push_back(coeffY);
	if (j > i) {
	  i = i + j; j = i - j; i = i - j;
	}
	qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i); qsubj_vec.push_back(j);	qval_vec.push_back(-coeffX); 
	qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(j+numCells); qval_vec.push_back(-coeffY);
     }
      cstrNum++;
    } HYPERGRAPH_END_FOR;
  }
  *qsubk = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubk_vec.size());
  *qsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());

  bool debugPrint = false;
  //  bool debugPrint = true;
  if (debugPrint) cout << "  k\ti\tj\tvalue " << endl;
  for (i = 0; i < qsubk_vec.size(); i++) {
    (*qsubk)[i] = qsubk_vec[i];
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = 2 * qval_vec[i];
    if (debugPrint) cout << "  " << (*qsubk)[i] << "\t" << (*qsubi)[i] << "\t" << (*qsubj)[i] << "\t" << (*qval)[i] << endl;
  }

  /* Printing the quadratic constraints matrices */
  if (debugPrint) {
    unsigned int currentk = 0;
    double **kthMatrix = (double **) malloc(sizeof(double*) * 2 * numCells);
    for (i = 0; i < 2 * numCells; i++) {
      kthMatrix[i] = (double *) malloc(sizeof(double) * 2 * numCells);
      for (j = 0; j <= i; j++) {
	kthMatrix[i][j] = 0.0;
      }
    }
    for (i = 0; i < qsubk_vec.size(); i++) {
      if (currentk != qsubk_vec[i]) {
	/* Print the old kth matrix and 
	   Clear the kth matrix */
	currentk = qsubk_vec[i];
	cout << "Printing " << (currentk - 1) << "th constraint matrix" << endl;
	for (int idx = 0; idx < 2 * numCells; idx++) {
	  cout << "\t";
	  for (int jdx = 0; jdx < 2 * numCells ; jdx++) {
	    if (idx >= jdx) {
	      cout << kthMatrix[idx][jdx] << "\t";
	      kthMatrix[idx][jdx] = 0.0;
	    } else {
	      cout << kthMatrix[jdx][idx] << "\t";
	      kthMatrix[idx][jdx] = 0.0;
	    }
	  }
	  cout << endl;
	}
      } 
      kthMatrix[(int)(*qsubi)[i]][(int)(*qsubj)[i]] = (double)(*qval)[i];
    }
    cout << "Printing " << (currentk - 1) << "th constraint matrix" << endl;
    for (int idx = 0; idx < 2 * numCells; idx++) {
      cout << "\t";
      for (int jdx = 0; jdx < 2 * numCells; jdx++) {
	if (idx >= jdx) {
	  cout << kthMatrix[idx][jdx] << "\t";
	  kthMatrix[idx][jdx] = 0.0;
	} else {
	  cout << kthMatrix[jdx][idx] << "\t";
	  kthMatrix[idx][jdx] = 0.0;
	}
      }
      cout << endl;
    }
    free(kthMatrix);
  }
  numValues = qsubk_vec.size();
}


/* Generate the linear array C for the minimization of the objective
   function */
void
mskGetObjectiveLinearArray(vector<Cell*> inputCells, HyperGraph& connectivityGraph, 
			   map<unsigned int, pair<Pin*, Pin* > >& pinsOfEdges,
			   MSKidxt **subj, MSKrealt **val, unsigned int &numNonZero)
{
  unsigned int i,j;
  unsigned int numCells = inputCells.size();
  unsigned int numConstraints = connectivityGraph.GetNumEdges();
  
  vector<double> val_vec(2 * numCells);
  vector<unsigned int>subj_vec;
  numNonZero = 0;
  for (i = 0; i < (2 * numCells); i++) {
    val_vec[i] = 0.0;
  }
  for (i = 0; i < numCells; i++) {
    Cell *cellPtri = inputCells[i];
    unsigned int edgeIdx;
    double edgeWeight;
    double coeffX, coeffY;
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight)  {
      pair<Pin*, Pin*> pinPair = pinsOfEdges[edgeIdx];
      Pin *pin1 = pinPair.first; Pin *pin2 = pinPair.second;
      double xOff1, yOff1, xOff2, yOff2;
      double cell1X, cell1Y, cell2X, cell2Y;
      unsigned int cell1Width, cell1Height, cell2Width, cell2Height;

      xOff1 = (double)(*pin1).PinGetXOffset() / GRID_COMPACTION_RATIO; yOff1 = (double)(*pin1).PinGetYOffset() / GRID_COMPACTION_RATIO;
      xOff2 = (double)(*pin2).PinGetXOffset() / GRID_COMPACTION_RATIO; yOff2 = (double)(*pin2).PinGetYOffset() / GRID_COMPACTION_RATIO;

      Cell* cellPtr1 = &((*pin1).PinGetParentCell()); Cell* cellPtr2 = &((*pin2).PinGetParentCell());
      if ((*cellPtr1).CellGetName() != (*cellPtri).CellGetName()) {
	continue;
      }
      Cell &cell1 = *cellPtr1; Cell &cell2 = *cellPtr2;

      cell1X = (double)(cell1.CellGetXpos()) / GRID_COMPACTION_RATIO; cell1Y = (double)(cell1.CellGetYpos()) / GRID_COMPACTION_RATIO;
      cell2X = (double)(cell2.CellGetXpos()) / GRID_COMPACTION_RATIO; cell2Y = (double)(cell2.CellGetYpos()) / GRID_COMPACTION_RATIO;
      cell1Width = cell1.CellGetWidth(); cell1Height = cell1.CellGetHeight(); 
      cell2Width = cell2.CellGetWidth(); cell2Height = cell2.CellGetHeight();

      coeffX = (RPERLENGTHX) * (CPERLENGTHX) * edgeWeight;
      coeffY = (RPERLENGTHY) * (CPERLENGTHY) * edgeWeight;

      if (!cell2.CellIsTerminal()) {
	/* Contributes to the ij term here */
	for (j = 0; j < numCells; j++) {
	  if (cellPtr2 == inputCells[j]) {
	    break;
	  }
	}
	if (j > i) {
	  i = i + j; j = i - j; i = i - j;
	}
	val_vec[i] += (2 * coeffX * (xOff1 - xOff2));
	val_vec[j] += -(2 * coeffX * (xOff1 - xOff2));

	val_vec[i+numCells] += (2 * coeffY * (yOff1 - yOff2));
	val_vec[j+numCells] += -(2 * coeffY * (yOff1 - yOff2));
      } else {
	val_vec[i] += (2 * coeffX * (xOff1 - cell2X));

	val_vec[i+numCells] += (2 * coeffY * (yOff1 - cell2Y));
      }
    } HYPERGRAPH_END_FOR;
  }

  vector<double> newValVec;
  for (int i = 0; i < val_vec.size(); i++) {
    if (val_vec[i] != 0.0) {
      subj_vec.push_back(i);
      newValVec.push_back(val_vec[i]);
      numNonZero++;
    }
  }
  
  *val = (MSKrealt *)malloc(sizeof(MSKrealt) * newValVec.size());
  *subj = (MSKidxt *)malloc(sizeof(MSKidxt) * newValVec.size());
  for (int i = 0; i < newValVec.size();i++) {
    (*val)[i] = newValVec[i];
    (*subj)[i] = subj_vec[i];
  }
  cout << endl;
}


/* Get linear array of constraints */
void 
mskGetLinearCstrArray(vector<Cell*> inputCells, HyperGraph& connectivityGraph, 
		      map<unsigned int, pair<Pin*, Pin* > >& pinsOfEdges,
		      MSKidxt **sub, MSKintt **ptrb, MSKintt **ptre, MSKidxt **asub,
		      MSKrealt **aval, double**aconst, unsigned int &numNonZero)
{
  vector<unsigned int> sub_vec;
  vector<unsigned int> ptrb_vec;
  vector<unsigned int> ptre_vec;
  vector<unsigned int> asub_vec;
  vector<double> aval_vec;
  vector<double> aconst_vec;
  unsigned int i, j;
  
  /* sub here holds the indices of the columns */
  /* asub here holds the indices of the constraints */
  unsigned int cstrNum = 0;
  unsigned int numCells = inputCells.size();
  for (i=0;i<numCells;i++) {
    Cell *cellPtri = inputCells[i];
    unsigned int edgeIdx;
    double edgeWeight;
    double coeffX, coeffY, constTerm; 
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight) {
      pair<Pin*, Pin*> pinPair = pinsOfEdges[edgeIdx];
      
      Pin *pin1 = pinPair.first;
      Pin *pin2 = pinPair.second;
      double xOff1, yOff1, xOff2, yOff2;
      double cell1X, cell1Y, cell2X, cell2Y;

      xOff1 = (double)(*pin1).PinGetXOffset() / GRID_COMPACTION_RATIO; yOff1 = (double)(*pin1).PinGetYOffset() / GRID_COMPACTION_RATIO;
      xOff2 = (double)(*pin2).PinGetXOffset() / GRID_COMPACTION_RATIO; yOff2 = (double)(*pin2).PinGetYOffset() / GRID_COMPACTION_RATIO;

      Cell* cellPtr1 = &((*pin1).PinGetParentCell()); Cell* cellPtr2 = &((*pin2).PinGetParentCell());
      if ((*cellPtr1).CellGetName() != (*cellPtri).CellGetName()) {
	continue;
      }
      
      Cell &cell1 = *cellPtr1; Cell &cell2 = *cellPtr2;
      
      cell1X = (double)(cell1.CellGetXpos()) / GRID_COMPACTION_RATIO; cell1Y = (double)(cell1.CellGetYpos()) / GRID_COMPACTION_RATIO;
      cell2X = (double)(cell2.CellGetXpos()) / GRID_COMPACTION_RATIO; cell2Y = (double)(cell2.CellGetYpos()) / GRID_COMPACTION_RATIO;

      sub_vec.push_back(cstrNum); 

      ptrb_vec.push_back(asub_vec.size());
      if (!cell2.CellIsTerminal()) {
	coeffX = 2 * edgeWeight * (RPERLENGTHX) * (CPERLENGTHX) * (xOff1 - xOff2); 
	coeffY = 2 * edgeWeight * (RPERLENGTHY) * (CPERLENGTHY) * (yOff1 - yOff2);
      
	for (j = 0; j < numCells; j++) {
	  if (cellPtr2 == inputCells[j]) {
	    break;
	  }
	}

	asub_vec.push_back(i); aval_vec.push_back(coeffX);
	asub_vec.push_back(i + numCells); aval_vec.push_back(coeffY);

	asub_vec.push_back(j); aval_vec.push_back(-coeffX);
	aval_vec.push_back(j + numCells); aval_vec.push_back(-coeffY);

	aconst_vec.push_back((coeffX * coeffX) + (coeffY * coeffY));
      } else {
	coeffX = 2 * edgeWeight * (RPERLENGTHX) * (CPERLENGTHX) * (xOff1 - cell2X);
	coeffY = 2 * edgeWeight * (RPERLENGTHY) * (CPERLENGTHY) * (yOff1 - cell2Y);

	asub_vec.push_back(i); aval_vec.push_back(coeffX);
	asub_vec.push_back(i + numCells); aval_vec.push_back(coeffY);
	
	aconst_vec.push_back((coeffX * coeffX) + (coeffY * coeffY));
      }
      ptre_vec.push_back(asub_vec.size());
      cstrNum++;
    } HYPERGRAPH_END_FOR;
  }
  
  *sub = (MSKidxt *) malloc(sizeof(MSKidxt)*sub_vec.size());
  *ptrb = (MSKidxt *) malloc(sizeof(MSKidxt)*ptrb_vec.size());
  *ptre = (MSKidxt *) malloc(sizeof(MSKidxt)*ptre_vec.size());
  *asub = (MSKidxt *) malloc(sizeof(MSKidxt)*asub_vec.size());
  *aval = (MSKrealt *) malloc(sizeof(MSKrealt)*aval_vec.size());
  *aconst = (double *) malloc(sizeof(double)*aconst_vec.size());

  for (i = 0; i < sub_vec.size(); i++) {
    (*sub)[i] = sub_vec[i];
  }
  for (i = 0; i < ptrb_vec.size(); i++) {
    (*ptrb)[i] = ptrb_vec[i];
  }
  for (i = 0; i < ptre_vec.size(); i++) {
    (*ptre)[i] = ptre_vec[i];
  }
  for (i = 0; i < aval_vec.size(); i++) {
    (*aval)[i] = aval_vec[i];
  }
  for (i = 0; i < asub_vec.size(); i++) {
    (*asub)[i] = asub_vec[i];
  }
  for (i = 0; i < aconst_vec.size(); i++) {
    (*aconst)[i] = aconst_vec[i];
  }

  numNonZero = aval_vec.size();
  bool debugPrint = false;
  //  bool debugPrint = true;
  /* Print the linear constraints here */
  if (debugPrint) {
    unsigned int startIdx, endIdx;
    for (i = 0; i < cstrNum; i++) {
      startIdx = (*ptrb)[i];
      endIdx = (*ptre)[i];
      cout << "Constraint number: " << i << endl;
      cout << "Start idx in ptrb: " << startIdx << " End idx in ptre: " << endIdx << endl;
      cout << "asub:" << endl;
      for (j = startIdx; j < endIdx; j++) {
	cout << "\t" << (*asub)[j];
      }
      cout << endl;
      cout << "aval:" << endl;
      for (j = startIdx; j < endIdx; j++) {
	cout << "\t" << (*aval)[j];
      }
      cout << endl << endl;
      cout << "Constant term for the constraint: " << (*aconst)[i] << endl;
    }
  }
}

/* 
   Solve the quadratically constrained quadratic minimization problem:
   f = (1/2)(x^T)*Q0*(x) + c^T * x
   s.t. lkx <= (1/2)(x^T)*Q1*(x) + Ax <= b
 */
void 
mosekSolveQuadratic(Design& myDesign, vector<Cell *>& inputCells, 
		    vector<Cell *>& inputPorts, HyperGraph& seqCellGraph, 
		    map<unsigned int, pair<Pin*, Pin* > >& pinPairsOfEdge)
{
  unsigned int numConstraints;
  unsigned int numCells;
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;
  bool progressDebug = true;
  numCells = inputCells.size();
  numConstraints = seqCellGraph.GetNumEdges();
  
  /* Create the mosek environment */
  r = MSK_makeenv(&env, NULL, NULL, NULL, NULL);

  /* Check if the return code is okay */
  if (r == MSK_RES_OK) {
    MSK_linkfunctoenvstream(env, MSK_STREAM_LOG, NULL, printstr);
  }

  /* Define the array of variables */
  double xx[2 * numCells];

  /* Initialize the environment */
  r = MSK_initenv(env);
  numConstraints = 0;
  if (r == MSK_RES_OK) {
    /* Create the optimization task */
    r = MSK_maketask(env, numConstraints, 2 * numCells, &task);

    if (r == MSK_RES_OK) {
      if (progressDebug) cout << "Task created successfully" << endl;
      r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);
      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_putmaxnumvar(task, 2 * numCells);
      if (r == MSK_RES_OK) r = MSK_putmaxnumcon(task, numConstraints);
      if (r == MSK_RES_OK) r = MSK_putmaxnumanz(task, 2 * numCells);
      
      /* Append 'numConstraints' empty constraints. The constraints will initially have no bounds. */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_CON, numConstraints);
      
      /* Append '2 * numCells', the variables will initially be fixed to zero (x = 0) */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_VAR, 2 * numCells);

      /* Optionally add a constant term to the objective. */
      if (r == MSK_RES_OK) r = MSK_putcfix(task, 0.0);
      if (progressDebug) cout << "Fixed constant term Cf to 0 successfully" << endl;

      /* Put bounds on the variables */
      for (int j=0; j < numCells && r == MSK_RES_OK; j++) {
	/* Set the bounds on the variable j */
	/* blx[j] <= x_j <= bux[j] */
	/* bound key that must be used is MSK_BK_RA which says that the variable
	   is range bound */
	/* Put the upper and lower bound on all variables: both X and Ys */
	double lowerX, lowerY, upperX, upperY;
	//lowerX = myDesign.DesignGetXLower(); upperX = myDesign.DesignGetXUpper(); 
	//lowerY = myDesign.DesignGetYLower(); upperY = myDesign.DesignGetYUpper(); 
	lowerX = 0.0; upperX = 11.9; lowerY = 0.0; upperY = 11.9;

	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_VAR, j,
					      MSK_BK_RA,
					      0.0,
					      118.00);
	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_VAR, j + numCells,
					      MSK_BK_RA,
					      0.0,
					      118.00);
      }
      if (progressDebug) cout << "Bounds on variables applied successfully" << endl;
      
      MSKidxt *qcsubk; MSKidxt *qcsubi; MSKidxt *qcsubj; MSKrealt *qcval;
      unsigned int numNonZero;

      /* Input the objective quadratic matrix Q */
      mskGetObjectiveMatrix(inputCells, seqCellGraph, pinPairsOfEdge,
			    &qcsubi, &qcsubj, &qcval, numNonZero);

      if (r == MSK_RES_OK) r = MSK_putqobj(task, numNonZero, qcsubi, qcsubj, 
					   qcval);
      if (progressDebug) cout << "Objective quadratic function coefficients added successfully" << endl;

      /* Input the linear part of the quadratic objective function which is C (not C^t) */
      MSKidxt *subj; MSKrealt *val;
      mskGetObjectiveLinearArray(inputCells, seqCellGraph, pinPairsOfEdge,
				 &subj, &val, numNonZero);

      if (r == MSK_RES_OK) r = MSK_putclist(task, numNonZero, subj, val);
      if (progressDebug) cout << "Objective linear function coefficients added successfully" << endl;

      /* Input the quadratic constraint matrices for k constraints */
      /*
      mskGetQuadraticCstrMatrix(inputCells, seqCellGraph, pinPairsOfEdge,
				&qcsubk, &qcsubi, &qcsubj, &qcval, numNonZero);
      if (r == MSK_RES_OK) r = MSK_putqcon(task, numNonZero, qcsubk, qcsubi, qcsubj, qcval);
      */
      if (progressDebug) cout << "Constraint quadratic function coefficients added successfully" << endl;
			     
      /* Input the linear constraint vectors for k constraints */
      MSKintt *ptrb; MSKintt *ptre;
      MSKidxt *sub; MSKidxt *asub; MSKrealt *aval;
      double *consts;
      /*
      mskGetLinearCstrArray(inputCells, seqCellGraph, pinPairsOfEdge,
			    &sub, &ptrb, &ptre, &asub, &aval, &consts, numNonZero);

      if (r == MSK_RES_OK) r = MSK_putaveclist(task, MSK_ACC_CON, numConstraints,
					       sub, ptrb, ptre, asub, aval);
      /* Put bounds on the constraints 
      for (int j=0; j < numConstraints && r == MSK_RES_OK; j++) {
	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_CON, j,
					      MSK_BK_FR,
					      0.0,
					      (TCLK - consts[j]));
      }
      */

      if (progressDebug) cout << "Bounds on constraints applied successfully" << endl;


      if (progressDebug) cout << "Constraint linear function coefficients added successfully" << endl;
      
      /* Spicify minimize/maximize */
      if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);

      if (r == MSK_RES_OK) {
	if (progressDebug) cout << "Minimization sense objective added successfully" << endl;
	MSKrescodee trmcode;
	
	/* Run the optimizer */
	r = MSK_optimizetrm(task, &trmcode);
	
	/* Print a summary containing information about the solution for debugging purposes */
	MSK_solutionsummary(task, MSK_STREAM_LOG);

	if (r == MSK_RES_OK) {
	  MSKsolstae solsta;
	  MSK_getsolutionstatus(task, MSK_SOL_ITR, NULL, &solsta);

	  switch(solsta) {
	  case MSK_SOL_STA_OPTIMAL: 
	  case MSK_SOL_STA_NEAR_OPTIMAL: 
	    MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, 2 * numCells, xx);
	    cout << "Optimal primal solution found\n";
	    for (int i = 0; i < numCells; i++) {
	      Cell& myCell = *(inputCells[i]);
	      //	      cout << "Cell Name: " << myCell.CellGetName() << " Original name: " << myCell.CellGetOrigName() << endl;
	      double xpos = xx[i];
	      double ypos = xx[i+numCells];

	      //	      cout << "Before rounding up: " << endl;
	      //	      cout << "x[" << i << "]: " << xpos << "  " << "y[" << i << "]: " << ypos << endl;

	      xpos *= GRID_COMPACTION_RATIO;
	      ypos *= GRID_COMPACTION_RATIO;
	      
	      xpos = dround(xpos);
	      ypos = dround(ypos);

	      //	      cout << "After rounding up: " << endl;
	      //	      cout << "x[" << i << "]: " << xpos << "  " << "y[" << i << "]: " << ypos << endl;
	      unsigned int xpos_int = (int)xpos;
	      unsigned int ypos_int = (int)ypos;
	      if ((xpos - (double)xpos_int) >= 0.5) xpos_int++; 
	      if ((ypos - (double)ypos_int) >= 0.5) ypos_int++;

	      //	      cout << "After conversion to integers: " << endl;
	      //	      cout << "x[" << i << "]: " << xpos_int << "  " << "y[" << i << "]: " << ypos_int << endl;

	      myCell.CellSetXpos(xpos_int);
	      myCell.CellSetYpos(ypos_int);
	    }
	    break;
	  case MSK_SOL_STA_DUAL_INFEAS_CER:
	  case MSK_SOL_STA_PRIM_INFEAS_CER:
	  case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
	  case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER:
	    cout << "Primal or dual infeasibility certificate found" << endl;
	    break;
	  case MSK_SOL_STA_UNKNOWN:
	    cout << "The status of the solution could not be determined" << endl;
	    break;
	  default: 
	    cout << "Other solution status" << endl;
	    break;
	  };
	} else {
	  cout << "Error while optimizing" << endl;
	}
      }
    }
    MSK_deletetask(&task);
  }
  MSK_deleteenv(&env);
}

