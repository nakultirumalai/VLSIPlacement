# include <mosek.h>
# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <QuadProg++.hh>
# include <AnalyticalSolve.h>

# define GRID_COMPACTION_RATIO 10000

void
MSKAPI printstr(void *handle, char str[]) 
{
  printf("%s", str);
}

void
mskGetObjectiveMatrixForDist(vector<Cell *>& listOfCells, HyperGraph& connectivityGraph,
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
  
  numCells = listOfCells.size();
  numConstraints = connectivityGraph.GetNumEdges();
  for (i=0;i<numCells;i++) {
    cellPtri = listOfCells[i];
    for (j=0;j<=i;j++) {
      cellPtrj = listOfCells[j];
      if (i == j) {
	if ((*cellPtri).CellIsPort()) {
	  continue;
	}
	unsigned int edgeIdx;
	unsigned int edgeWeight;
	double weight = 0;
	HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, 
					   edgeWeight) {
	  weight += edgeWeight;
	} HYPERGRAPH_END_FOR;
	qsubi_vec.push_back(i);
	qsubj_vec.push_back(i);
	qval_vec.push_back(((RPERLENGTHX) * (CPERLENGTHX) * (0.5) * weight));

	qsubi_vec.push_back(i + numCells);
	qsubj_vec.push_back(i + numCells);
	qval_vec.push_back(((RPERLENGTHY) * (CPERLENGTHY) * (0.5) * weight));
      } else {
	if ((*cellPtri).CellIsPort() || (*cellPtrj).CellIsPort()) {
	  continue;
	}
	if (connectivityGraph.HyperGraphNodesAreAdjacent(cellPtri, cellPtrj) >= 0) {
	  unsigned int edgeWeight;
	  double weight = 0;
	  HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECTS(connectivityGraph, cellPtri, cellPtrj, 
					      edgeIdx, edgeWeight) {
	    weight += edgeWeight;
	  } HYPERGRAPH_END_FOR;
	  qsubi_vec.push_back(i);
	  qsubj_vec.push_back(j);
	  qval_vec.push_back(-((RPERLENGTHX) * (CPERLENGTHX) * (0.5) * weight));

	  qsubi_vec.push_back(i + numCells);
	  qsubj_vec.push_back(j + numCells);
	  qval_vec.push_back(-((RPERLENGTHY) * (CPERLENGTHY) * (0.5) * weight));
	}
      }
    }
  }
  if (qsubi_vec.size() != qsubj_vec.size()) {
    _ASSERT_TRUE("Vector sizes are not the same");
  }
  if (qsubi_vec.size() != qval_vec.size()) {
    _ASSERT_TRUE("Vector sizes are not the same");
  }

  *qsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());
  
  bool debugPrint = false;
  if (debugPrint) cout << "  i\tj\tvalue " << endl;

  for (i = 0; i < qsubi_vec.size(); i++) {
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = qval_vec[i];
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


/* Generate the linear array C for the minimization of the objective
   function */
void
mskGetObjectiveLinearArrForDist(vector<Cell*> inputCells, HyperGraph& connectivityGraph, 
				map<unsigned int, pair<Pin*, Pin* > >& pinPairsOfEdge,
				MSKidxt **subj, MSKrealt **val, unsigned int &numNonZero)
{
  unsigned int numCells = inputCells.size();
  unsigned int numConstraints = connectivityGraph.GetNumEdges();
  
  double coeffVector[numCells*2];
  numNonZero = 0;

  for (int i = 0; i < numCells; i++) {
    Cell *cellPtri = inputCells[i];
    double coeffix = 0;
    double coeffiy = 0;
    unsigned int edgeWeight;
    double weight = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, 
				       edgeWeight) {
      double xoffset1, xoffset2;
      double yoffset1, yoffset2;
      Pin *pinPtr1 = pinPairsOfEdge[edgeIdx].first;
      Pin *pinPtr2 = pinPairsOfEdge[edgeIdx].second;
      Cell &cell1 = (*pinPtr1).PinGetParentCell();
      Cell &cell2 = (*pinPtr2).PinGetParentCell();
      
      if (cell2.CellGetName() == (*cellPtri).CellGetName()) {
	Pin *tmpPinPtr = pinPtr2;
	pinPtr2 = pinPtr1;
	pinPtr1 = tmpPinPtr;
      }
      
      if (cell1.CellIsPort()) {
	xoffset1 = ((double)(cell1.CellGetXpos()))/GRID_COMPACTION_RATIO;
	yoffset1 = ((double)(cell1.CellGetYpos()))/GRID_COMPACTION_RATIO;
      } else {
	xoffset1 = ((double)((*pinPtr1).PinGetXOffset()))/GRID_COMPACTION_RATIO; 
	yoffset1 = ((double)((*pinPtr1).PinGetYOffset()))/GRID_COMPACTION_RATIO; 
      }
      
      if (cell2.CellIsPort()) {
	xoffset2 = ((double)(cell2.CellGetXpos()))/GRID_COMPACTION_RATIO;
	yoffset2 = ((double)(cell2.CellGetYpos()))/GRID_COMPACTION_RATIO;
      } else {
	xoffset2 = ((double)(*pinPtr2).PinGetXOffset())/GRID_COMPACTION_RATIO;
	yoffset2 = ((double)(*pinPtr2).PinGetYOffset())/GRID_COMPACTION_RATIO;
      }

      weight+= edgeWeight;

      coeffix += weight * (RPERLENGTHX) * (CPERLENGTHX) * (0.5) * ((xoffset1 - xoffset2));
      coeffiy += weight * (RPERLENGTHY) * (CPERLENGTHY) * (0.5) * ((yoffset1 - yoffset2));
    } HYPERGRAPH_END_FOR;
    if (coeffix != 0)  numNonZero++;
    coeffVector[i] = (-4 * coeffix);
    coeffVector[i + numCells] = (-4 * coeffiy);
  }
  
  *subj = (MSKidxt *)malloc(sizeof(MSKidxt) * (2 * numNonZero));
  *val = (MSKrealt *)malloc(sizeof(MSKrealt) * (2 * numNonZero));
  
  unsigned int k = 0;
  for (int i = 0; i < numCells; i++) {
    if (coeffVector[i] == 0.0) {
      continue;
    }
    (*subj)[k] = i;
    (*val)[k] = coeffVector[i];
    k++;
  }
}

/* 
   Solve the quadratically constrained quadratic minimization problem:
   f = (1/2)(x^T)*Q0*(x) + c^T * x
   s.t. lkx <= (1/2)(x^T)*Q1*(x) + Ax <= b
 */
void 
mosekSolveQuadraticDist(Design& myDesign, vector<Cell *>& seqCells, 
			vector<Cell *> &ports,
			HyperGraph& seqCellGraph, 
			map<unsigned int, pair<Pin*, Pin* > >& pinPairsOfEdge)
{
  unsigned int numConstraints;
  unsigned int numCells;
  unsigned int numPorts;
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;
  bool progressDebug = true;

  numPorts = ports.size();
  numCells = seqCells.size();
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
  if (r == MSK_RES_OK) {
    /* Create the optimization task */
    r = MSK_maketask(env, numConstraints, 2 * numCells, &task);

    if (r == MSK_RES_OK) {
      if (progressDebug) cout << "Task created successfully" << endl;
      r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);
      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_putmaxnumvar(task, 2 * numCells);
      if (r == MSK_RES_OK) r = MSK_putmaxnumcon(task, numConstraints);

      // OPTIONAL: if (r == MSK_RES_OK) r = MSK_putmaxnumanz(task, NUMANZ); // REVISIT
      
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
	lowerX = 0.0; upperX = 15.0; lowerY = 0.0; upperY = 15.0;

	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_VAR, j,
					      MSK_BK_RA,
					      0.0,
					      15.0);
	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_VAR, j + numCells,
					      MSK_BK_RA,
					      0.0,
					      15.0);
      }
      if (progressDebug) cout << "Bounds on variables applied successfully" << endl;
      
      /* Put bounds on the constraints */
      for (int j=0; j < numConstraints && r == MSK_RES_OK; j++) {
	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_CON, j,
					      MSK_BK_FR,
					      0.0,
					      TCLK);
      }
      if (progressDebug) cout << "Bounds on constraints applied successfully" << endl;

      MSKidxt *qcsubk; MSKidxt *qcsubi; MSKidxt *qcsubj; MSKrealt *qcval;
      unsigned int numNonZero;

      /* Input the objective quadratic matrix Q */
      mskGetObjectiveMatrixForDist(inputCells, seqCellGraph, pinPairsOfEdge,
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
      mskGetQuadraticCstrMatrix(inputCells, seqCellGraph, pinPairsOfEdge,
				&qcsubk, &qcsubi, &qcsubj, &qcval, numNonZero);

      if (r == MSK_RES_OK) r = MSK_putqcon(task, numNonZero, qcsubk, qcsubi, qcsubj, qcval);
      if (progressDebug) cout << "Constraint quadratic function coefficients added successfully" << endl;
			     
      /* Input the linear constraint vectors for k constraints */
      MSKintt *ptrb; MSKintt *ptre;
      MSKidxt *sub; MSKidxt *asub; MSKrealt *aval;
      msgGetLinearCstrArray(inputCells, seqCellGraph, pinPairsOfEdge,
			    &sub, &ptrb, &ptre, &asub, &aval, numNonZero);

      if (r == MSK_RES_OK) r = MSK_putaveclist(task, MSK_ACC_CON, numConstraints,
					       sub, ptrb, ptre, asub, aval);
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
	    MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numCells, xx);
	    cout << "Optimal primal solution found\n";
	    for (int i = 0; i < numCells; i++) {
	      //	      Cell& myCell = *(inputCells[i]);
	      //	      if (myCell.CellIsPort()) continue;
	      //	      cout << "Cell Name: " << myCell.CellGetName() << " Original name: " << myCell.CellGetOrigName() << endl;
	      double xpos = xx[i];
	      double ypos = xx[i+numCells];

	      cout << "Before rounding up: " << endl;
	      cout << "x[" << i << "]: " << xpos << "  " << "y[" << i << "]: " << ypos << endl;

	      xpos *= GRID_COMPACTION_RATIO;
	      ypos *= GRID_COMPACTION_RATIO;
	      
	      xpos = dround(xpos);
	      ypos = dround(ypos);

	      cout << "After rounding up: " << endl;
	      cout << "x[" << i << "]: " << xpos << "  " << "y[" << i << "]: " << ypos << endl;
	      unsigned int xpos_int = (int)xpos;
	      unsigned int ypos_int = (int)ypos;
	      if ((xpos - (double)xpos_int) >= 0.5) xpos_int++; 
	      if ((ypos - (double)ypos_int) >= 0.5) ypos_int++;

	      cout << "After conversion to integers: " << endl;
	      cout << "x[" << i << "]: " << xpos_int << "  " << "y[" << i << "]: " << ypos_int << endl;

	      //	      myCell.CellSetXpos(xpos_int);
	      //	      myCell.CellSetYpos(ypos_int);
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

