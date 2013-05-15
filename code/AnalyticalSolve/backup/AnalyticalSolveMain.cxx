# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <AnalyticalSolve.h>

# define GRID_COMPACTION_RATIO 1000 // to convert to micro

void 
MSKAPI printstr(void *handle, char str[]) 
{
  printf("%s", str);
}

void
mskGetObjectiveMatrix(Design& myDesign, HyperGraph& connectivityGraph,
		      vector<Cell *>inputCells, MSKidxt **qsubi, 
		      MSKidxt **qsubj, MSKrealt **qval, unsigned int &numValues)
{
  int i, j;
  double coeffuv, coeffu;
  Cell *cellPtri, *cellPtrj;
  unsigned int numCells;
  unsigned int numConstraints;
  vector<unsigned int> qsubi_vec, qsubj_vec;
  vector<double> qval_vec;
  double RPERUNITLENX, RPERUNITLENY;
  double CPERUNITLENX, CPERUNITLENY;

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
      Net *relatedNet = connectivityGraph.GetEdgeObject(edgeIdx);
      Pin *pinPtr;
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	Cell* cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	if (cellOfPinPtr == cellPtri) break;
      } NET_END_FOR;
      Pin *pin1 = pinPtr; 
      Pin *pin2;

      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	Cell *cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtri; Cell &cell2 = *cellPtr2;
	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;
      
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
      } NET_END_FOR;
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

  for (i = 0; i < qsubi_vec.size(); i++) {
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = 2 * qval_vec[i];
  }

  bool debugPrint = false;
  /* Printing the quadratic objective matrix */
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
mskGetQuadraticCstrMatrix(Design& myDesign, HyperGraph& connectivityGraph, 
			  vector<Cell *>& inputCells, MSKidxt **qsubk, 
			  MSKidxt **qsubi, MSKidxt **qsubj, MSKrealt **qval, 
			  unsigned int &numValues)
{
  Cell *cellPtri, *cellPtrj;
  int i, j;
  double coeffuv, coeffu;
  unsigned int numCells;
  unsigned int cstrNum;
  unsigned int numConstraints;
  vector<unsigned int> qsubk_vec, qsubi_vec, qsubj_vec;
  vector<double> qval_vec;
  double RPERUNITLENX, RPERUNITLENY;
  double CPERUNITLENX, CPERUNITLENY;

  numCells = inputCells.size();
  numConstraints = connectivityGraph.GetNumEdges();

  cstrNum = 0;
  mskGetTimingParams(&RPERUNITLENX, &RPERUNITLENY, &CPERUNITLENX, &CPERUNITLENY);

  for (i = 0; i < numCells; i++) {
    Cell *cellPtri = inputCells[i];
    unsigned int edgeIdx;
    double edgeWeight;
    double coeffX, coeffY;
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight) {
      Net *relatedNet = connectivityGraph.GetEdgeObject(edgeIdx);
      Pin *pinPtr;
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	Cell* cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	if (cellOfPinPtr == cellPtri) break;
      } NET_END_FOR;
      
      Pin *pin1 = pinPtr; 
      Pin *pin2;

      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	Cell *cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtri; 
	Cell &cell2 = *cellPtr2;
	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;
	
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
	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(i); qsubj_vec.push_back(i);
	  qval_vec.push_back(coeffX);

	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(j); qsubj_vec.push_back(j); 
	  qval_vec.push_back(coeffX); 

	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(i); qsubj_vec.push_back(j);
	  qval_vec.push_back(-coeffX);

	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(i+numCells);
	  qval_vec.push_back(coeffY);

	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(j+numCells); qsubj_vec.push_back(j+numCells); 
	  qval_vec.push_back(coeffY); 

	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(j+numCells);
	  qval_vec.push_back(-coeffY);
	}
      } NET_END_FOR;
    } HYPERGRAPH_END_FOR;
  }

  *qsubk = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubk_vec.size());
  *qsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());

  for (i = 0; i < qsubk_vec.size(); i++) {
    (*qsubk)[i] = qsubk_vec[i];
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = 2 * qval_vec[i];
  }

  numValues = qval_vec.size();

  /* Printing the quadratic constraints matrices */
  bool debugPrint = false;
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
	/* Print the old kth matrix and Clear the kth matrix */
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
}

/* Generate the linear array C for the minimization of the objective function */
void
mskGetObjectiveLinearArray(Design &myDesign, HyperGraph& connectivityGraph, 
			   vector<Cell*> inputCells, MSKidxt **subj, MSKrealt **val, 
			   unsigned int &numNonZero)
{
  unsigned int i,j;
  unsigned int numCells = inputCells.size();
  unsigned int numConstraints = connectivityGraph.GetNumEdges();
  double RPERUNITLENX, RPERUNITLENY, CPERUNITLENX, CPERUNITLENY;
  vector<double> val_vec(2 * numCells);
  vector<unsigned int>subj_vec;
  numNonZero = 0;

  mskGetTimingParams(&RPERUNITLENX, &RPERUNITLENY, &CPERUNITLENX, &CPERUNITLENY);

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
      Net *relatedNet = connectivityGraph.GetEdgeObject(edgeIdx);
      Pin *pinPtr;
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	Cell* cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	if (cellOfPinPtr == cellPtri) break;
      } NET_END_FOR;
      
      Pin *pin1 = pinPtr; 
      Pin *pin2;

      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	Cell *cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtri; 
	Cell &cell2 = *cellPtr2;

	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;

	double xOff1, yOff1, xOff2, yOff2;
	double cell1X, cell1Y, cell2X, cell2Y;

	xOff1 = (double)(*pin1).PinGetXOffset() / GRID_COMPACTION_RATIO; 
	yOff1 = (double)(*pin1).PinGetYOffset() / GRID_COMPACTION_RATIO;
	xOff2 = (double)(*pin2).PinGetXOffset() / GRID_COMPACTION_RATIO; 
	yOff2 = (double)(*pin2).PinGetYOffset() / GRID_COMPACTION_RATIO;
	cell1X = (double)(cell1.CellGetXpos()) / GRID_COMPACTION_RATIO; 
	cell1Y = (double)(cell1.CellGetYpos()) / GRID_COMPACTION_RATIO;
	cell2X = (double)(cell2.CellGetXpos()) / GRID_COMPACTION_RATIO; 
	cell2Y = (double)(cell2.CellGetYpos()) / GRID_COMPACTION_RATIO;

	cell1Width = cell1.CellGetWidth() / GRID_COMPACTION_RATIO;
	cell1Height = cell1.CellGetHeight() / GRID_COMPACTION_RATIO;
	cell2Width = cell2.CellGetWidth() / GRID_COMPACTION_RATIO;
	cell2Height = cell2.CellGetHeight() / GRID_COMPACTION_RATIO;

	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;

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
      } NET_END_FOR;
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
}


/* Get linear array of constraints */
void 
mskGetLinearCstrArray(vector<Cell*> inputCells, HyperGraph& connectivityGraph, 
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
   Invoke the solver here and set the values of x and y returned
   by the solver as positions of the cells 
 */
void 
mosekSolveQuadraticTop(Design& myDesign, HyperGraph& seqCellGraph, 
		       vector<Cell *>& inputCells) 
{
  double<vector> X =  mosekSolveQuadratic(myDesign, seqCellGraph, inputCells, 
					  (2 * inputCells.size()),
					  (seqCellGraph.GetNumEdges()), 
					  mskGetObjectiveMatrix, 
					  mskGetObjectiveLinearArray, 
					  mskGetQuadraticCstrMatrix,
					  mskGetLinearCstrArray);

  unsigned int numCells = inputCells.size();
  for (int i = 0; i < X.size(); i++) {
    Cell &thisCell = *((Cell*)inputCells[i]);
    thisCell.CellSetXpos(X[i]); thisCell.CellSetYpos(X[i+numCells]);
  }
  
  return;
}
