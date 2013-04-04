# include <mosek.h>
# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <QuadProg++.hh>
# include <AnalyticalSolve.h>

# define RPERLENGTHX 1 // in milli ohms per length
# define CPERLENGTHX 1 // in micro farad per length
# define RPERLENGTHY 1 // in micro farad per length
# define CPERLENGTHY 1 // in micro farad per length
# define TCLK 2

void
MSKAPI printstr(void *handle, char str[]) 
{
  printf("%s", str);
}

void
mskGetObjectiveMatrix(vector<Cell *>& listOfCells, HyperGraph& connectivityGraph,
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

void
mskGetQuadraticCstrMatrix(vector<Cell *>& listOfCells, HyperGraph& connectivityGraph,
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

  numCells = listOfCells.size();
  numConstraints = connectivityGraph.GetNumEdges();

  cstrNum = 0;
  for (i=0;i<numCells;i++) {
    cellPtri = listOfCells[i];
    for (j=0;j<i;j++) {
      cellPtrj = listOfCells[j];
      long int edgeIdx = connectivityGraph.HyperGraphNodesAreAdjacent(cellPtri, cellPtrj);
      if (edgeIdx < 0) {
	continue;
      }
      /* 
	 Now the cells i & j have an edge between them, populate 
	 qii, qjj, qij and q(i+n)(i+n) q(j+n)(j+n) 
       */
      /* Populate for x */
      /* first the ii coeff */
      double constX, constY;
      double weight;
      weight = connectivityGraph.GetEdgeWeight(edgeIdx);
      constX = (RPERLENGTHX) * (CPERLENGTHX) * (0.5) * weight;
      constY = (RPERLENGTHY) * (CPERLENGTHY) * (0.5) * weight;

      /* Populate for x */
      /* Next the ii coeff */
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i); qsubj_vec.push_back(i); qval_vec.push_back((2 * (constX))); 
      /* Next the jj coeff */
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(j); qsubj_vec.push_back(j); qval_vec.push_back((2 * (constX))); 
      /* Next the ij coeff */
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i); qsubj_vec.push_back(j); qval_vec.push_back((-2 * (constX))); 
      /* Next the ji coeff */
      //      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i); qsubj_vec.push_back(j); qval_vec.push_back((-2 * (constX))); 
      
      /* Populate for y */
      /* Next the ii coeff */
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i + numCells); qsubj_vec.push_back(i + numCells); qval_vec.push_back((2 * (constY))); 
      /* Next the jj coeff */
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(j + numCells); qsubj_vec.push_back(j + numCells); qval_vec.push_back((2 * (constY))); 
      /* Next the ij coeff */
      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(i + numCells); qsubj_vec.push_back(j + numCells); qval_vec.push_back((-2 * (constY))); 
      /* Next the ji coeff */
      //      qsubk_vec.push_back(cstrNum); qsubi_vec.push_back(j + numCells); qsubj_vec.push_back(i + numCells); qval_vec.push_back((-2 * (constY))); 

      cstrNum++;
    }
  }
  *qsubk = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubk_vec.size());
  *qsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());

  bool debugPrint = false;
  if (debugPrint) cout << "  k\ti\tj\tvalue " << endl;
  for (i = 0; i < qsubk_vec.size(); i++) {
    (*qsubk)[i] = qsubk_vec[i];
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = qval_vec[i];
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
      xoffset1 = ((double)((*pinPtr1).PinGetXOffset()))/10000; xoffset2 = ((double)(*pinPtr2).PinGetXOffset())/10000;
      yoffset1 = ((double)((*pinPtr1).PinGetYOffset()))/10000; yoffset2 = ((double)(*pinPtr2).PinGetYOffset())/10000;
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


/* Get linear array of constraints */
void 
msgGetLinearCstrArray(vector<Cell*> inputCells, HyperGraph& connectivityGraph, 
		      map<unsigned int, pair<Pin*, Pin* > >& pinPairsOfEdge,
		      MSKidxt **sub, MSKintt **ptrb, MSKintt **ptre, MSKidxt **asub,
		      MSKrealt **aval, unsigned int &numNonZero)
{
  vector<unsigned int> sub_vec;
  vector<unsigned int> ptrb_vec;
  vector<unsigned int> ptre_vec;
  vector<unsigned int> asub_vec;
  vector<double> aval_vec;
  unsigned int i, j;
  
  /* sub here holds the indices of the columns */
  /* asub here holds the indices of the constraints */
  unsigned int cstrNum = 0;
  unsigned int numCells = inputCells.size();
  for (i=0;i<numCells;i++) {
    Cell *cellPtri = inputCells[i];
    for (j=0;j<numCells;j++) {
      if (j == i) continue;
      Cell *cellPtrj = inputCells[j];
      long int edgeIdx = connectivityGraph.HyperGraphNodesAreAdjacent(cellPtri, cellPtrj);
      if (edgeIdx < 0) {
	continue;
      }
      unsigned int xoffset1, xoffset2;
      unsigned int yoffset1, yoffset2;
      unsigned int edgeWeight = connectivityGraph.GetEdgeWeight(edgeIdx);
      unsigned int weight;
      double coeffix, coeffiy;
      Pin *pinPtr1 = pinPairsOfEdge[edgeIdx].first;
      Pin *pinPtr2 = pinPairsOfEdge[edgeIdx].second;
      Cell &cell1 = (*pinPtr1).PinGetParentCell();
      Cell &cell2 = (*pinPtr2).PinGetParentCell();

      if (cell2.CellGetName() == (*cellPtri).CellGetName()) {
	Pin *tmpPinPtr = pinPtr2;
	pinPtr2 = pinPtr1;
	pinPtr1 = tmpPinPtr;
      }

      xoffset1 = (*pinPtr1).PinGetXOffset(); xoffset2 = (*pinPtr2).PinGetXOffset();
      yoffset1 = (*pinPtr1).PinGetYOffset(); yoffset2 = (*pinPtr2).PinGetYOffset();
      weight+= edgeWeight;

      coeffix += weight * (RPERLENGTHX) * (CPERLENGTHX) * (0.5) * (xoffset1 - xoffset2);
      coeffiy += weight * (RPERLENGTHY) * (CPERLENGTHY) * (0.5) * (yoffset1 - yoffset2);

      sub_vec.push_back(cstrNum);
      ptrb_vec.push_back(asub_vec.size() - 1); 

      asub_vec.push_back(i);
      aval_vec.push_back(4 * coeffix);

      asub_vec.push_back(i + numCells);
      aval_vec.push_back(4 * coeffix);

      asub_vec.push_back(j);
      aval_vec.push_back(-4 * coeffix);

      aval_vec.push_back(j + numCells);
      aval_vec.push_back(-4 * coeffix);

      ptre_vec.push_back(asub_vec.size());
      cstrNum++;
    }
  }
  
  *sub = (MSKidxt *) malloc(sizeof(MSKidxt)*sub_vec.size());
  *ptrb = (MSKidxt *) malloc(sizeof(MSKidxt)*ptrb_vec.size());
  *ptre = (MSKidxt *) malloc(sizeof(MSKidxt)*ptre_vec.size());
  *asub = (MSKidxt *) malloc(sizeof(MSKidxt)*asub_vec.size());
  *aval = (MSKrealt *) malloc(sizeof(MSKrealt)*aval_vec.size());

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

  numNonZero = aval_vec.size();
}

/* 
   Solve the quadratically constrained quadratic minimization problem:
   f = (1/2)(x^T)*Q0*(x) + c^T * x
   s.t. lkx <= (1/2)(x^T)*Q1*(x) + Ax <= b
 */
void 
mosekSolveQuadratic(Design& myDesign, vector<Cell *>& inputCells, HyperGraph& seqCellGraph, 
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
					      +MSK_INFINITY);
	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_VAR, j + numCells,
					      MSK_BK_RA,
					      0.0,
					      +MSK_INFINITY);
      }
      if (progressDebug) cout << "Bounds on variables applied successfully" << endl;
      
      /* Put bounds on the constraints */
      for (int j=0; j < numConstraints && r == MSK_RES_OK; j++) {
	if (r == MSK_RES_OK) r = MSK_putbound(task, MSK_ACC_CON, j,
					      MSK_BK_UP,
					      0,
					      TCLK);
      }
      if (progressDebug) cout << "Bounds on constraints applied successfully" << endl;

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
      mskGetQuadraticCstrMatrix(inputCells, seqCellGraph, pinPairsOfEdge,
				&qcsubk, &qcsubi, &qcsubj, &qcval, numNonZero);

      bool debugPrint = false;
      if (debugPrint) cout << "  \tk\ti\tj\tvalue " << endl;
      for (int i = 0; i < numNonZero; i++) {
	if (debugPrint) cout << "  " << i << ": " <<  qcsubk[i] << "\t" << qcsubi[i] << "\t" << qcsubj[i] << "\t" << qcval[i] << endl;
      }

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
      if (r == MSK_RES_OK) r = MSK_putobjsense (task , MSK_OBJECTIVE_SENSE_MINIMIZE);
      
      if (r == MSK_RES_OK) {
	if (progressDebug) cout << "Minimization sense objective added successfully" << endl;
	MSKrescodee trmcode;
	
	/* Run the optimizer */
	r = MSK_optimizetrm(task, &trmcode);
	
	/* Print a summary containing information 
	   about the solution for debugging purposes */
	MSK_solutionsummary(task, MSK_STREAM_LOG);
	if (r == MSK_RES_OK) {
	  cout << "Solution has been generated" << endl;
	} else {
	  cout << "Error while optimizing" << endl;
	}
      }
    }
    MSK_deletetask(&task);
  }
  MSK_deleteenv(&env);
}

