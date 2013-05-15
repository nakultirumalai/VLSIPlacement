# include <AnalyticalSolve.h>

void
genGetCstrBoundsWL(Design& myDesign, HyperGraph& myGraph, 
		 vector<Cell*>& listOfCells,
		 MSKidxt **sub, double *cstrUBounds,
		 MSKidxt **subj, MSKrealt **bu, MSKrealt **bl,
		 MSKboundkeye **bk, unsigned int& numCstrs) 
{
  double totalDelay;
  double maxx, maxy, minx, miny;
  unsigned int numVars;
  unsigned int numCells;

  *subj = (MSKidxt *)malloc(sizeof(MSKidxt) * numCstrs);
  *bu = (MSKrealt *)malloc(sizeof(MSKrealt) * numCstrs);
  *bl = (MSKrealt *)malloc(sizeof(MSKrealt) * numCstrs);
  *bk = (MSKboundkeye *)malloc(sizeof(MSKboundkeye) * numCstrs);

  totalDelay = myDesign.DesignGetClockPeriod();
  for (int i = 0; i < numCstrs; i++) {
    (*subj)[i] = (*sub)[i];
    (*bu)[i] = 118.0 - cstrUBounds[i];
    (*bl)[i] = 0.0;
    (*bk)[i] = MSK_BK_UP;
    cout << "For constraint " << i << " Upper bound : " << (*bu)[i] << " Lower bound : " << (*bl)[i] << endl;
  }
}

void
genGetObjectiveMatrixWL(Design& myDesign, HyperGraph& connectivityGraph,
			vector<Cell *>& inputCells, MSKidxt **qsubi, 
			MSKidxt **qsubj, MSKrealt **qval, 
			unsigned int &numValues)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtr, *nextPinPtr;
  vector<unsigned int> qsubi_vec, qsubj_vec;
  vector<double> qval_vec;
  double edgeWeight, coeffX, coeffY;
  unsigned int numCells, numVars, edgeIdx;
  unsigned int cellIdxi, cellIdxj;
  int i, j;

  numCells = inputCells.size();
  numVars = 2 * numCells;
  vector<double> squaredTerms(numVars);
  map<unsigned int, map<unsigned int, double> > nonSquaredTerms;
  for (i = 0; i < numVars; i++) {
    squaredTerms[i] = 0.0;
  }
  coeffX = 0; coeffY = 0;
  HYPERGRAPH_FOR_ALL_EDGES(connectivityGraph, edgeIdx, edgeWeight) {
    Net &relatedNet = *(Net *)connectivityGraph.GetEdgeObject(edgeIdx);
    map<Pin *, bool> visitedPins;
    NET_FOR_ALL_PINS(relatedNet, pinPtr) {
      cellPtri = &((*pinPtr).PinGetParentCell());
      if ((*cellPtri).CellIsTerminal()) continue;
      for (i = 0; i < numCells; i++) { 
	if (cellPtri == inputCells[i]) {
	  cellIdxi = i;
	  break; 
	}
      }
      visitedPins[pinPtr] = true;
      NET_FOR_ALL_PINS(relatedNet, nextPinPtr) {
	_KEY_EXISTS(visitedPins, nextPinPtr) {
	  continue;
	}
	coeffX = edgeWeight;
	coeffY = edgeWeight;
	cellPtrj = &((*nextPinPtr).PinGetParentCell());
	if ((*cellPtrj).CellIsTerminal()) {
	  squaredTerms[cellIdxi] += coeffX * PULL_FACTOR;
	  squaredTerms[cellIdxi + numCells] += coeffY * PULL_FACTOR;
	  continue;
	} else {
	  squaredTerms[cellIdxi] += coeffX;
	  squaredTerms[cellIdxi + numCells] += coeffY;
	}
	for (j = 0; j < numCells; j++) { 
	  if (cellPtrj == inputCells[j]) {
	    cellIdxj = j;
	    break; 
	  }
	}
	/* Add the squared terms here */
	squaredTerms[cellIdxj] += coeffX;
	squaredTerms[cellIdxj + numCells] += coeffY;

	/* Add the nonsquared quadratic terms here */
	unsigned int idx1, idx2;
	if (cellIdxi < cellIdxj) { idx1 = cellIdxj; idx2 = cellIdxi; }
	else { idx1 = cellIdxi; idx2 = cellIdxj; }
	qsubi_vec.push_back(idx1);
	qsubj_vec.push_back(idx2);
	qval_vec.push_back((-0.5 * coeffX));

	idx1 += + numCells; idx2 += numCells;
	qsubi_vec.push_back(idx1);
	qsubj_vec.push_back(idx2);
	qval_vec.push_back((-0.5 * coeffY));
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
  
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
  
  unsigned int idx1, idx2, value;
  *qsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());

  for (i = 0; i < qsubi_vec.size(); i++) {
    (*qsubi)[i] = qsubi_vec[i];
    (*qsubj)[i] = qsubj_vec[i];
    (*qval)[i] = 2 * qval_vec[i];
  }
  bool debugPrint = false;
  if (debugPrint) {
    cout << "i\tj\tValue" << endl;
    for (i = 0; i < qsubi_vec.size(); i++) {
      cout << (*qsubi)[i] << "\t" << (*qsubj)[i] << "\t" << (*qval)[i] << endl;
      if ((*qsubi)[i] == (*qsubj)[i]) {
	unsigned int index = (*qsubi)[i];
	if (index >= numCells) { index -= numCells; }
	Cell &thisCell = *(Cell *)inputCells[index];
	cout << "\t CELL: " << thisCell.CellGetName() << endl;
      }
    }
  }

  numValues = qsubi_vec.size();
}

/* Generate the linear array C for the minimization of the objective function */
void
genGetObjectiveLinearArrayWL(Design &myDesign, HyperGraph& connectivityGraph, 
			     vector<Cell*>& inputCells, MSKidxt **subj, MSKrealt **val, 
			     double &constant, unsigned int &numValues)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtr, *nextPinPtr;
  unsigned int numCells, numVars, edgeIdx;
  vector<unsigned int>subj_vec;
  double edgeWeight, coeffX, coeffY;
  unsigned int cellIdxi, cellIdxj;
  int i, j;

  numCells = inputCells.size();
  numVars = 2 * numCells;
  vector<double> val_vec(numVars);
  for (i = 0; i < numVars; i++) {
    val_vec[i] = 0.0;
  }
  numValues = 0;
  constant = 0.0;
  coeffX = 0; coeffY = 0;
  HYPERGRAPH_FOR_ALL_EDGES(connectivityGraph, edgeIdx, edgeWeight) {
    Net &relatedNet = *(Net *)connectivityGraph.GetEdgeObject(edgeIdx);
    unsigned int numPins = relatedNet.NetGetPinCount();
    coeffX = edgeWeight / numPins; coeffY = edgeWeight / numPins;
    NET_FOR_ALL_PINS(relatedNet, pinPtr) {
      double xOff1, yOff1, xOff2, yOff2;
      double cell1X, cell1Y, cell2X, cell2Y;
      double dOffx, dOffy;
      cellPtri = &((*pinPtr).PinGetParentCell());
      /* Skip inputs for cells marked as terminal cells */
      if ((*cellPtri).CellIsTerminal()) continue;
      Cell &cell1 = *cellPtri;
      bool cellFound = false;
      for (i = 0; i < numCells; i++) { 
	if (cellPtri == inputCells[i]) {
	  cellIdxi = i;
	  cellFound = true;
	  break; 
	}
      }
      if (cellFound == false) {
	cout << "Error: Cell not found in input list" << endl;
	exit(0);
      }
      xOff1 = ((double)(*pinPtr).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
      yOff1 = ((double)(*pinPtr).PinGetYOffset()) / GRID_COMPACTION_RATIO;
      cell1X = ((double)(cell1.CellGetXpos())) / GRID_COMPACTION_RATIO; 
      cell1Y = ((double)(cell1.CellGetYpos())) / GRID_COMPACTION_RATIO;

      NET_FOR_ALL_PINS(relatedNet, nextPinPtr) {
	cellPtrj = &((*nextPinPtr).PinGetParentCell());
	Cell &cell2 = *cellPtrj;
	xOff2 = (double)(*nextPinPtr).PinGetXOffset() / GRID_COMPACTION_RATIO;
	yOff2 = (double)(*nextPinPtr).PinGetYOffset() / GRID_COMPACTION_RATIO;
	cell2X = (double)(cell2.CellGetXpos()) / GRID_COMPACTION_RATIO; 
	cell2Y = (double)(cell2.CellGetYpos()) / GRID_COMPACTION_RATIO;
	if (!cell2.CellIsTerminal()) {
	  dOffx = xOff1 - xOff2;
	  dOffy = yOff1 - yOff2;
	} else {
	  dOffx = xOff1 - cell2X;
	  dOffy = yOff1 - cell2Y;
	}
	if (cell2.CellIsTerminal()) {
	  if (cell2.CellIsSequential()) {
	    val_vec[cellIdxi] += coeffX * (dOffx) * PULL_FACTOR;
	    val_vec[cellIdxi+numCells] += coeffY * (dOffy) * PULL_FACTOR;
	  } else {
	    val_vec[cellIdxi] += coeffX * (dOffx);
	    val_vec[cellIdxi+numCells] += coeffY * (dOffy);
	  }	    
	} else {
	  val_vec[cellIdxi] += coeffX * (dOffx);
	  val_vec[cellIdxi+numCells] += coeffY * (dOffy);
	}
	constant += (coeffX * dOffx * dOffx) + (coeffY * dOffy * dOffy);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;

  *val = (MSKrealt *)malloc(sizeof(MSKrealt) * val_vec.size());
  *subj = (MSKidxt *)malloc(sizeof(MSKidxt) * val_vec.size());
  //  cout << "i\tValue" << endl;
  for (int i = 0; i < val_vec.size();i++) {
    (*val)[i] = 2 * val_vec[i];
    (*subj)[i] = i;
    //    cout << (*subj)[i] << "\t" << (*val)[i] << endl;
  }
  numValues = val_vec.size();
}

void
genGetQuadraticCstrMatrixWL(Design& myDesign, HyperGraph& connectivityGraph, 
			    vector<Cell *>& inputCells, MSKidxt **qcsubk, 
			    MSKidxt **qcsubi, MSKidxt **qcsubj, MSKrealt **qcval, 
			    unsigned int &numValues)
{

}

/* Get linear array of constraints */
void 
genGetLinearCstrArrayWL(Design& myDesign, HyperGraph& connectivityGraph, 
			vector<Cell*>& inputCells, MSKidxt **sub, MSKintt **ptrb, 
			MSKintt **ptre, MSKidxt **asub, MSKrealt **aval, 
			double** cstrUBounds, unsigned int &numNonZero)
{

}

/* 
   Invoke the solver here and set the values of x and y returned
   by the solver as positions of the cells 
 */
void 
genSolveQOWL(Design& myDesign, HyperGraph& myGraph, 
		    vector<Cell *>& inputCells) 
{
  vector<double> X;
  X =  mosekSolveQCQO(myDesign, myGraph, inputCells, 
		      (2 * inputCells.size()),
		      0, 
		      genGetObjectiveMatrixWL, 
		      genGetObjectiveLinearArrayWL,
		      NIL(quadCstrFunc),
		      NIL(linCstrFunc),
		      mskGetVarBounds,
		      NIL(cstrBoundFunc));

  unsigned int numCells = inputCells.size();
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *((Cell*)inputCells[i]);
    thisCell.CellSetXpos((X[i] * GRID_COMPACTION_RATIO)); thisCell.CellSetYpos((X[i+numCells] * GRID_COMPACTION_RATIO));
  }
  
  return;
}

void 
genSolveQCQOWL(Design& myDesign, HyperGraph& myGraph, 
		    vector<Cell *>& inputCells) 
{
  vector<double> X;
  X =  mosekSolveQCQO(myDesign, myGraph, inputCells, 
		      (2 * inputCells.size()),
		      0, 
		      genGetObjectiveMatrixWL, 
		      genGetObjectiveLinearArrayWL,
		      NULL,
		      NULL,
		      mskGetVarBounds,
		      NULL);

  unsigned int numCells = inputCells.size();
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *((Cell*)inputCells[i]);
    thisCell.CellSetXpos((X[i] * GRID_COMPACTION_RATIO)); 
    thisCell.CellSetYpos((X[i+numCells] * GRID_COMPACTION_RATIO));
  }
  
  return;
}

