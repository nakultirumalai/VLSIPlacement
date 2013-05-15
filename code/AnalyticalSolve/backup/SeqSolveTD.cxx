# include <AnalyticalSolve.h>

void
getTimingParams(double *RPERUNITLENX, double *RPERUNITLENY, 
		double *CPERUNITLENX, double *CPERUNITLENY)
{
  *RPERUNITLENX = 1; // 1 ohm/um 
  *RPERUNITLENY = 1; // 1 ohm/um
  *CPERUNITLENX = 1; // 10-4 pf/um = 10-16 F 
  *CPERUNITLENY = 1; // 10-4 pf/um
}

double
getDesignPathAllowedDelay(Design& myDesign, HyperGraph& myGraph,
			  unsigned int edgeIdx)
{
  double clockPeriod;
  double maxDelay;
  double totalCellDelay;
  double allowedInterConnectDelay;

  allowedInterConnectDelay = 0.0;
  return (allowedInterConnectDelay);
  /* Find total cell delay on path */
  Net* dummyNetPtr = (Net *)myGraph.GetEdgeObject(edgeIdx);
  
  totalCellDelay = PathGetCellDelay(dummyNetPtr);
  clockPeriod = myDesign.DesignGetClockPeriod();
  /* subtract total cell day from min{clockPeriod, maxDelay} */
  allowedInterConnectDelay = clockPeriod - totalCellDelay;

  /* Remaining delay is the upper bound */
  return (allowedInterConnectDelay);
}

void
seqGetCstrBoundsTD(Design& myDesign, HyperGraph& myGraph, 
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
seqGetObjectiveMatrixTD(Design& myDesign, HyperGraph& connectivityGraph,
			vector<Cell *>& inputCells, MSKidxt **qsubi, 
			MSKidxt **qsubj, MSKrealt **qval, 
			unsigned int &numValues)
{
  Cell *cellPtri, *cellPtrj, *cellOfPinPtr;
  Pin *pinPtr;
  vector<unsigned int> qsubi_vec, qsubj_vec;
  vector<double> qval_vec;
  double RPERUNITLENX, RPERUNITLENY;
  double CPERUNITLENX, CPERUNITLENY;
  double edgeWeight, coeffX, coeffY;
  unsigned int numCells, numVars;
  unsigned int edgeIdx, idx1, idx2;
  int i, j;

  numCells = inputCells.size();
  numVars = 2 * numCells;
  vector<double> squaredTerms(numVars);

  for (i = 0; i < numCells; i++) {
    squaredTerms[i] = 0.0;
    squaredTerms[i+numCells] = 0.0;
  }

  getTimingParams(&RPERUNITLENX, &RPERUNITLENY, &CPERUNITLENX, &CPERUNITLENY);
  for (i = 0; i < numCells; i++) {
    cellPtri = inputCells[i];
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight)  {
      Net &relatedNet = *(Net *)connectivityGraph.GetEdgeObject(edgeIdx);
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	break;
      } NET_END_FOR;
      /* IMP : First pin on the net must belong to the 
	 sequential cell under consideration */
      if (cellOfPinPtr != cellPtri) continue;

      Pin *pin1 = pinPtr; Pin *pin2;
      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	Cell *cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtri; Cell &cell2 = *cellPtr2;
	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;
	squaredTerms[i]+= coeffX;  squaredTerms[i+numCells]+= coeffY;
	if (!cell2.CellIsTerminal()) {
	  /* Contributes to the ij term here */
	  for (j = 0; j < numCells; j++) if (cellPtr2 == inputCells[j]) break;
	  squaredTerms[j] += coeffX;
	  squaredTerms[j+numCells] += coeffY;
	  if (j > i) {
	    qsubi_vec.push_back(j); qsubj_vec.push_back(i); qval_vec.push_back(-coeffX); 
	    qsubi_vec.push_back(j+numCells); qsubj_vec.push_back(i+numCells); 
	    qval_vec.push_back(-coeffY);
	  } else {
	    qsubi_vec.push_back(i); qsubj_vec.push_back(j); qval_vec.push_back(-coeffX); 
	    qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(j+numCells); 
	    qval_vec.push_back(-coeffY);
	  }
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

  numValues = qsubi_vec.size();
}

/* Generate the linear array C for the minimization of the objective function */
void
seqGetObjectiveLinearArrayTD(Design &myDesign, HyperGraph& connectivityGraph, 
			     vector<Cell*>& inputCells, MSKidxt **subj, MSKrealt **val, 
			     double &constant, unsigned int &numValues)
{
  Cell *cellPtr1, *cellPtr2, *cellOfPinPtr;
  Pin *pinPtr;
  double RPERUNITLENX, RPERUNITLENY, CPERUNITLENX, CPERUNITLENY;
  double edgeWeight, coeffX, coeffY;
  vector<unsigned int>subj_vec;
  unsigned int numCells, numVars;
  unsigned int edgeIdx;
  int i,j;

  getTimingParams(&RPERUNITLENX, &RPERUNITLENY, &CPERUNITLENX, &CPERUNITLENY);
  numCells = inputCells.size();
  numVars = 2 * numCells;
  vector<double> val_vec(numVars);
  for (i = 0; i < numVars; i++) {
    val_vec[i] = 0.0;
  }
  numValues = 0;
  constant = 0.0;
  for (i = 0; i < numCells; i++) {
    cellPtr1 = inputCells[i];
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtr1, edgeIdx, edgeWeight)  {
      Net &relatedNet = *(Net *)connectivityGraph.GetEdgeObject(edgeIdx);
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	break;
      } NET_END_FOR;

      if (cellOfPinPtr != cellPtr1) continue;
      Pin *pin1 = pinPtr; 
      Pin *pin2;

      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	Cell *cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtr1; Cell &cell2 = *cellPtr2;
	double xOff1, yOff1, xOff2, yOff2;
	double cell1X, cell1Y, cell2X, cell2Y;
	double dOffx, dOffy;

	xOff1 = (double)(*pin1).PinGetXOffset() / GRID_COMPACTION_RATIO; 
	yOff1 = (double)(*pin1).PinGetYOffset() / GRID_COMPACTION_RATIO;
	xOff2 = (double)(*pin2).PinGetXOffset() / GRID_COMPACTION_RATIO; 
	yOff2 = (double)(*pin2).PinGetYOffset() / GRID_COMPACTION_RATIO;
	cell1X = (double)(cell1.CellGetXpos()) / GRID_COMPACTION_RATIO; 
	cell1Y = (double)(cell1.CellGetYpos()) / GRID_COMPACTION_RATIO;
	cell2X = (double)(cell2.CellGetXpos()) / GRID_COMPACTION_RATIO; 
	cell2Y = (double)(cell2.CellGetYpos()) / GRID_COMPACTION_RATIO;

	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;

	if (!cell2.CellIsTerminal()) {
	  /* Contributes to the ij term here */
	  for (j = 0; j < numCells; j++) if (cellPtr2 == inputCells[j]) break;
	  dOffx = (xOff1 - xOff2);
	  dOffy = (yOff1 - yOff2);

	  val_vec[i] += (2 * coeffX * dOffx);
	  val_vec[j] += -(2 * coeffX * dOffx);

	  val_vec[i+numCells] += (2 * coeffY * dOffy);
	  val_vec[j+numCells] += -(2 * coeffY * dOffy);
	} else {
	  dOffx = (xOff1 - cell2X);
	  dOffy = (yOff1 - cell2Y);

	  val_vec[i] += (2 * coeffX * dOffx);
	  val_vec[i+numCells] += (2 * coeffY * dOffy);
	}
	constant += dOffx * dOffx + dOffy * dOffy;
      } NET_END_FOR;
    } HYPERGRAPH_END_FOR;
  }

  vector<double> newValVec;
  for (int i = 0; i < val_vec.size(); i++) {
    if (val_vec[i] != 0.0) {
      subj_vec.push_back(i);
      newValVec.push_back(val_vec[i]);
    }
  }
  numValues = newValVec.size();
  *val = (MSKrealt *)malloc(sizeof(MSKrealt) * newValVec.size());
  *subj = (MSKidxt *)malloc(sizeof(MSKidxt) * newValVec.size());
  for (int i = 0; i < newValVec.size();i++) {
    (*val)[i] = newValVec[i];
    (*subj)[i] = subj_vec[i];
  }
}

void
seqGetQuadraticCstrMatrixTD(Design& myDesign, HyperGraph& connectivityGraph, 
			    vector<Cell *>& inputCells, MSKidxt **qcsubk, 
			    MSKidxt **qcsubi, MSKidxt **qcsubj, MSKrealt **qcval, 
			    unsigned int &numValues)
{
  Cell *cellPtr1, *cellPtr2, *cellOfPinPtr;
  Pin *pinPtr;
  vector<unsigned int> qsubi_vec, qsubj_vec, qsubk_vec;
  vector<double> qval_vec;
  double RPERUNITLENX, RPERUNITLENY;
  double CPERUNITLENX, CPERUNITLENY;
  double edgeWeight, coeffX, coeffY;
  unsigned int numCells, numVars, cstrNum;
  unsigned int edgeIdx, idx1, idx2;
  int i, j;

  numCells = inputCells.size();
  numVars = 2 * numCells;

  cstrNum = 0;
  getTimingParams(&RPERUNITLENX, &RPERUNITLENY, &CPERUNITLENX, &CPERUNITLENY);
  for (i = 0; i < numCells; i++) {
    Cell *cellPtr1 = inputCells[i];
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtr1, edgeIdx, edgeWeight) {
      Net &relatedNet = *(Net *)connectivityGraph.GetEdgeObject(edgeIdx);
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	break;
      } NET_END_FOR;
      
      if (cellOfPinPtr != cellPtr1) continue;
      Pin *pin1 = pinPtr; Pin *pin2;
      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	Cell *cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtr1; Cell &cell2 = *cellPtr2;
	coeffX = RPERUNITLENX * CPERUNITLENX * edgeWeight;
	coeffY = RPERUNITLENY * CPERUNITLENY * edgeWeight;

	qsubk_vec.push_back(cstrNum); 
	qsubi_vec.push_back(i); qsubj_vec.push_back(i);
	qval_vec.push_back(coeffX);
	qsubk_vec.push_back(cstrNum); 
	qsubi_vec.push_back(i+numCells); qsubj_vec.push_back(i+numCells);
	qval_vec.push_back(coeffY);

	if (!cell2.CellIsTerminal()) {
	  /* Contributes to the ij term here */
	  for (j = 0; j < numCells; j++) if (cellPtr2 == inputCells[j]) break;
	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(j); qsubj_vec.push_back(j); 
	  qval_vec.push_back(coeffX); 
	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(j+numCells); qsubj_vec.push_back(j+numCells);
	  qval_vec.push_back(coeffY);
	  idx1 = i; idx2 = j;
	  if (j > i) {
	    idx1 = j; idx2 = i;
	  }
	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(idx1); qsubj_vec.push_back(idx2);
	  qval_vec.push_back(-coeffX);
	  qsubk_vec.push_back(cstrNum); 
	  qsubi_vec.push_back(idx1+numCells); qsubj_vec.push_back(idx2+numCells);
	  qval_vec.push_back(-coeffY);
	}
      } NET_END_FOR;
    } HYPERGRAPH_END_FOR;
  }

  *qcsubk = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubk_vec.size());
  *qcsubi = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubi_vec.size());
  *qcsubj = (MSKidxt *)malloc(sizeof(MSKidxt) * qsubj_vec.size());
  *qcval = (MSKrealt *)malloc(sizeof(MSKrealt) * qval_vec.size());

  numValues = qval_vec.size();
}

/* Get linear array of constraints */
void 
seqGetLinearCstrArrayTD(Design& myDesign, HyperGraph& connectivityGraph, 
			vector<Cell*>& inputCells, MSKidxt **sub, MSKintt **ptrb, 
			MSKintt **ptre, MSKidxt **asub, MSKrealt **aval, 
			double** cstrUBounds, unsigned int &numNonZero)
{
  Cell *cellPtr1, *cellPtr2, *cellOfPinPtr;
  Pin *pinPtr;
  vector<unsigned int> sub_vec;
  vector<unsigned int> ptrb_vec;
  vector<unsigned int> ptre_vec;
  vector<unsigned int> asub_vec;
  vector<double> aval_vec;
  vector<double> aconst_vec;
  double RPERUNITLENX, RPERUNITLENY, CPERUNITLENX, CPERUNITLENY;
  unsigned int cstrNum, numCells, edgeIdx;
  double edgeWeight, coeffX, coeffY;
  double allowedPathDelay;
  double dOffx, dOffy;
  double constTerm;
  unsigned int i, j;
  
  cstrNum = 0;
  numCells = inputCells.size();
  /* sub here holds the indices of the columns */
  /* asub here holds the indices of the constraints */

  getTimingParams(&RPERUNITLENX, &RPERUNITLENY, &CPERUNITLENX, &CPERUNITLENY);

  for (i = 0; i < numCells; i++) {
    cellPtr1 = inputCells[i];
    coeffX = 0; coeffY = 0;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtr1, edgeIdx, edgeWeight)  {
      Net &relatedNet = *(Net *)connectivityGraph.GetEdgeObject(edgeIdx);
      NET_FOR_ALL_PINS(relatedNet, pinPtr) {
	cellOfPinPtr = &((*pinPtr).PinGetParentCell());
	break;
      } NET_END_FOR;

      if (cellOfPinPtr != cellPtr1) continue;
      Pin *pin1 = pinPtr; 
      Pin *pin2;

      dOffx = 0.0; dOffy = 0.0; constTerm = 0.0;
      NET_FOR_ALL_PINS(relatedNet, pin2) {
	if (pin2 == pin1) continue;
	cellPtr2 = &((*pin2).PinGetParentCell());
	Cell &cell1 = *cellPtr1; 
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

	coeffX = (RPERUNITLENX) * (CPERUNITLENX) * edgeWeight;
	coeffY = (RPERUNITLENY) * (CPERUNITLENY) * edgeWeight;

	sub_vec.push_back(cstrNum); 
	ptrb_vec.push_back(asub_vec.size());
	if (!cell2.CellIsTerminal()) {
	  /* Contributes to the ij term here */
	  for (j = 0; j < numCells; j++) if (cellPtr2 == inputCells[j]) break;

	  dOffx = xOff1 - xOff2;
	  dOffy = yOff1 - yOff2;

	  asub_vec.push_back(i); aval_vec.push_back(2 * coeffX * dOffx);
	  asub_vec.push_back(j); aval_vec.push_back(-2 * coeffX * dOffx);

	  asub_vec.push_back(i+numCells); aval_vec.push_back(2 * coeffY * dOffy);
	  asub_vec.push_back(j+numCells); aval_vec.push_back(-2 * coeffY * dOffy);
	} else {
	  dOffx = xOff1 - cell2X;
	  dOffy = yOff1 - cell2Y;

	  asub_vec.push_back(i); aval_vec.push_back(2 * coeffX * dOffx);
	  asub_vec.push_back(i+numCells); aval_vec.push_back(2 * coeffY * dOffy);
	}
	constTerm = (coeffX * dOffx * dOffx + coeffY * dOffy * dOffy);
      } NET_END_FOR;
      allowedPathDelay = getDesignPathAllowedDelay(myDesign, connectivityGraph,
						   edgeIdx);
      aconst_vec.push_back(allowedPathDelay + constTerm);
      ptre_vec.push_back(asub_vec.size());
      cstrNum++;
    } HYPERGRAPH_END_FOR;
  }
  
  *sub = (MSKidxt *) malloc(sizeof(MSKidxt)*sub_vec.size());
  *ptrb = (MSKidxt *) malloc(sizeof(MSKidxt)*ptrb_vec.size());
  *ptre = (MSKidxt *) malloc(sizeof(MSKidxt)*ptre_vec.size());
  *asub = (MSKidxt *) malloc(sizeof(MSKidxt)*asub_vec.size());
  *aval = (MSKrealt *) malloc(sizeof(MSKrealt)*aval_vec.size());
  *cstrUBounds = (double *) malloc(sizeof(double)*aconst_vec.size());

  for (i = 0; i < sub_vec.size(); i++) (*sub)[i] = sub_vec[i];
  for (i = 0; i < ptrb_vec.size(); i++) (*ptrb)[i] = ptrb_vec[i];
  for (i = 0; i < ptre_vec.size(); i++) (*ptre)[i] = ptre_vec[i];
  for (i = 0; i < aval_vec.size(); i++) (*aval)[i] = aval_vec[i];
  for (i = 0; i < asub_vec.size(); i++) (*asub)[i] = asub_vec[i];
  for (i = 0; i < aconst_vec.size(); i++) (*cstrUBounds)[i] = aconst_vec[i];

  numNonZero = sub_vec.size();
}

/* 
   Invoke the solver here and set the values of x and y returned
   by the solver as positions of the cells 
 */
void 
seqSolveQOTD(Design& myDesign, HyperGraph& seqCellGraph, 
		    vector<Cell *>& inputCells) 
{
  vector<double> X;

  X =  mosekSolveQCQO(myDesign, seqCellGraph, inputCells, 
		      (2 * inputCells.size()),
		      (seqCellGraph.GetNumEdges()), 
		      seqGetObjectiveMatrixTD, 
		      seqGetObjectiveLinearArrayTD, 
		      seqGetQuadraticCstrMatrixTD,
		      seqGetLinearCstrArrayTD,
		      mskGetVarBounds, 
		      seqGetCstrBoundsTD);

  unsigned int numCells = inputCells.size();
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *((Cell*)inputCells[i]);
    thisCell.CellSetXpos((X[i] * GRID_COMPACTION_RATIO)); thisCell.CellSetYpos((X[i+numCells] * GRID_COMPACTION_RATIO));
    cout << "Cell: " << thisCell.CellGetName() << "\t" << thisCell.CellGetXpos() << "\t" << thisCell.CellGetYpos() << endl;
  }
}

void 
seqSolveQCQOTD(Design& myDesign, HyperGraph& seqCellGraph, 
		    vector<Cell *>& inputCells) 
{
  vector<double> X;

  X =  mosekSolveQCQO(myDesign, seqCellGraph, inputCells, 
		      (2 * inputCells.size()),
		      (seqCellGraph.GetNumEdges()), 
		      seqGetObjectiveMatrixTD, 
		      seqGetObjectiveLinearArrayTD, 
		      seqGetQuadraticCstrMatrixTD,
		      seqGetLinearCstrArrayTD,
		      mskGetVarBounds, 
		      seqGetCstrBoundsTD);

  unsigned int numCells = inputCells.size();
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *((Cell*)inputCells[i]);
    thisCell.CellSetXpos((X[i] * GRID_COMPACTION_RATIO)); thisCell.CellSetYpos((X[i+numCells] * GRID_COMPACTION_RATIO));
    cout << "Cell: " << thisCell.CellGetName() << "\t" << thisCell.CellGetXpos() << "\t" << thisCell.CellGetYpos() << endl;
  }
}
