# include <Design.h>
# include <DesignIter.h>

bool debugPrint = false;

void 
getLogicConeRecur(Cell &cellObj, vector<Cell*>& relatedCells, 
		  vector<Pin* >& relatedPins, 
		  vector<unsigned int>& logicalDepths,
		  unsigned char direction, 
		  map<Cell*, unsigned int>& visitedCells,
		  unsigned int logicalDepth, bool noReg)
{
  Pin *pinPtr;
  visitedCells[&cellObj] = 1;
  logicalDepth++;
  /* The cell that is part to this part of the code can never 
     be a port or a sequential cell */
  if (debugPrint) {cout << "DBG: \t Next Cell " << cellObj.CellGetName() << " (" << cellObj.CellGetOrigName() << ") " << endl;}
  CELL_FOR_ALL_PINS(cellObj, direction, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    Pin *nextPinPtr;
    NET_FOR_ALL_PINS(connectedNet, nextPinPtr) {
      if ((*nextPinPtr).PinGetDirection() == direction) {
	continue;
      }
      Cell& nextCell = (*nextPinPtr).PinGetParentCell();
      if (visitedCells.find(&nextCell) != visitedCells.end()) {
	if (debugPrint) {cout << "DBG: \tTERMINATE RECURSION HERE BECAUSE CELL " << nextCell.CellGetName() << " IS VISITED"  << endl;}
	continue;
      } 
      if (nextCell.CellIsSequential() || nextCell.CellIsPort()) {
		if (debugPrint) cout<< "DBG: \tFrom Cell: " << cellObj.CellGetName() << " (" << cellObj.CellGetOrigName() << ") " << "   To Cell: " << nextCell.CellGetName() << " (" << nextCell.CellGetOrigName() << ") " << endl;
		if (debugPrint) cout<< "DBG: \tAdding pin " << (*nextPinPtr).PinGetName() << "   Direction: ";
	if ((*nextPinPtr).PinGetDirection() == PIN_DIR_INPUT) {
	  if (debugPrint) cout<< "Input" << endl;
	}
	if ((*nextPinPtr).PinGetDirection() == PIN_DIR_OUTPUT) {
	  if (debugPrint) cout<< "Output" << endl;
	}
	if (noReg == true) {
	  if (nextCell.CellIsSequential()) continue;
	  else { if (debugPrint) cout<< "DBG: Cell " << nextCell.CellGetName() << "is not sequential on the output side. Not skipped" << endl; }
	}
	relatedPins.push_back(nextPinPtr);
	relatedCells.push_back(&nextCell);
	logicalDepths.push_back(logicalDepth);
	if (debugPrint) cout<< "DBG: \tTERMINATE RECURSION HERE BECAUSE CELL " << nextCell.CellGetName() << " IS SEQUENTIAL "  << endl;
      } else {
	getLogicConeRecur(nextCell, relatedCells, relatedPins, logicalDepths,
			  direction, visitedCells, logicalDepth, noReg);
      }
    } END_FOR;
  } END_FOR;
}

/*******************************************************************************
  A POWERFUL TRAVERSAL FUNCTION WHICH STARTS WITH A SEQ. REGISTER AND
  RETURNS THE FOLLOWING:
  1) Pairs of pins: Pin of the sequential element, Pin of the related port 
     or the pin of the related sequential register
  2) Corresponding to each pair, a cell related to the sequential register
  3) The number of levels of logic between the sequential register and the 
     port / another seq. register
*******************************************************************************/
void
getLogicCone(Cell &cellObj, vector<Cell*>& relatedCells, 
	     vector<unsigned int>& logicalDepths,  
	     vector<pair<Pin*, Pin* > >& relatedPins, unsigned char direction,
	     bool noReg)
{
  map<Cell*, unsigned int> visitedCells;
  Pin *pinPtr;

  /* Iterate over all pins of the specified direction */
  visitedCells[&cellObj] = 1;
  CELL_FOR_ALL_PINS(cellObj, direction, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    Pin *nextPinPtr;
    NET_FOR_ALL_PINS(connectedNet, nextPinPtr) {
      if ((*nextPinPtr).PinGetDirection() == direction) {
	continue;
      }
      Cell &nextCell = (*nextPinPtr).PinGetParentCell();
      vector<Pin*> cellRelatedPins;
      vector<Cell *> cellRelatedCells;
      vector<unsigned int> relatedLogicalDepths;

      if (nextCell.CellIsSequential() || nextCell.CellIsPort()) {
	relatedPins.push_back(make_pair(pinPtr, nextPinPtr));
	relatedCells.push_back(&nextCell);
	logicalDepths.push_back((unsigned int)1);
	continue;
      } 
      getLogicConeRecur(nextCell, cellRelatedCells, cellRelatedPins, 
			relatedLogicalDepths, direction, visitedCells, 1,
			noReg);

      Pin *pinPtrLocal;
      Cell *cellPtrLocal;
      unsigned int logicalDepth;
      if (cellRelatedCells.size() != 0) {
	VECTOR_FOR_ALL_ELEMS(cellRelatedPins, Pin*, pinPtrLocal) {
	  relatedPins.push_back(make_pair(pinPtr, pinPtrLocal));
	} END_FOR;
	VECTOR_FOR_ALL_ELEMS(cellRelatedCells, Cell*, cellPtrLocal) {
	  relatedCells.push_back(cellPtrLocal);
	} END_FOR;
	VECTOR_FOR_ALL_ELEMS(relatedLogicalDepths, unsigned int, logicalDepth) {
	  logicalDepths.push_back(logicalDepth);
	} END_FOR;
      }

    } NET_END_FOR;
  } CELL_END_FOR;
  /* FOR DEBUGPRINTGING */
  debugPrint = false;
  if (1) {
    Cell *cellPtr;
    Pin *pinPtr1, *pinPtr2;
    unsigned int logicalDepth;
    if (debugPrint) cout<< "DBG: Obtained cone for cell " << cellObj.CellGetName() << " (" << cellObj.CellGetOrigName() << ")" << endl;
        if (debugPrint) cout<< "DBG: Size of relatedCells " << relatedCells.size() << "\tSize of pin pairs " << relatedPins.size();
        if (debugPrint) cout<< " \tSize of logical depth " << logicalDepths.size() << endl;
    unsigned int relatedCellSize = relatedCells.size();
    unsigned int relatedPinsSize = relatedPins.size();
    unsigned int logicalDepthsSize = logicalDepths.size();
    if (relatedCellSize != relatedPinsSize || relatedCellSize != logicalDepthsSize) {
      if (debugPrint) cout<< "Sizes are not the same" << endl;
    } else {
      for (int i = 0; i < relatedCellSize; i++) {
	Pin &relatedPin1 = *(Pin *)(relatedPins[i].first);
	Pin &relatedPin2 = *(Pin *)(relatedPins[i].second);
        Cell &relatedCell1 = relatedPin1.PinGetParentCell();
        Cell &relatedCell2 = relatedPin2.PinGetParentCell();
	if (debugPrint) cout<< "DBG:\t\t Cell: " << (*(relatedCells[i])).CellGetName() << " (" << (*(relatedCells[i])).CellGetOrigName() << ")" <<"  Pin1: " << relatedPin1.PinGetName() << " Pin2: " << relatedPin2.PinGetName() << " Logical Depth:" << logicalDepths[i] << endl;
      }
    }
  }
}


void
Design::DesignSolveForSeqCells(void)
{
  Design &myDesign = *this;
  HyperGraph seqCellGraph;
  string cellName;
  Cell *cellPtr;
  map<Cell*, bool> addedCells;
  map<unsigned int, pair<Pin*, Pin* > > pinPairsOfEdge;
  vector<Cell*> inputCells;
  
  _STEP_BEGIN("Building graph for sequential cells");
  /* Traverse the input cone of each sequential cell 
     to find a port or another cell to pair with */
  DESIGN_FOR_ALL_SEQ_CELLS(myDesign, cellName, cellPtr) {
    vector<Cell*> tmpRelatedCells;
    vector<pair<Pin*, Pin* > > tmpRelatedPins;
    vector<unsigned int> tmpLogicalDepths;
    vector<Cell*> relatedCells;
    vector<pair<Pin*, Pin* > > relatedPins;
    vector<unsigned int> logicalDepths;

    Cell& seqCell = *cellPtr;
    Cell* tmpCellPtr;
    pair<Pin*,Pin*> tmpPinPair;
    unsigned int tmpLogicalDepth;

    if (!((*cellPtr).CellIsSequential())) {
      if (debugPrint) cout<< "Error in iterator" << endl;
      exit(0);
    }
    /* Get the input logic cone */
    getLogicCone(seqCell, tmpRelatedCells, tmpLogicalDepths, tmpRelatedPins, 
		 PIN_DIR_INPUT, false);
    VECTOR_FOR_ALL_ELEMS(tmpRelatedCells, Cell*, tmpCellPtr) {
      relatedCells.push_back(tmpCellPtr);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpRelatedPins, pair<Pin* MCOMMA Pin*>, tmpPinPair) {
      relatedPins.push_back(tmpPinPair);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpLogicalDepths, unsigned int, tmpLogicalDepth) {
      logicalDepths.push_back(tmpLogicalDepth);
    } END_FOR;

    /* Clear vectors */
    tmpRelatedCells.clear(); tmpLogicalDepths.clear(); tmpRelatedPins.clear();
    getLogicCone(seqCell, tmpRelatedCells, tmpLogicalDepths, tmpRelatedPins, 
		 PIN_DIR_OUTPUT, true);
    VECTOR_FOR_ALL_ELEMS(tmpRelatedCells, Cell*, tmpCellPtr) {
      relatedCells.push_back(tmpCellPtr);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpRelatedPins, pair<Pin* MCOMMA Pin*>, tmpPinPair) {
      relatedPins.push_back(tmpPinPair);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpLogicalDepths, unsigned int, tmpLogicalDepth) {
      logicalDepths.push_back(tmpLogicalDepth);
    } END_FOR;
    
    /* Add the subject sequential cell into the graph and 
       the list of input cells  */
    if (addedCells.find(cellPtr) == addedCells.end()) {
      inputCells.push_back(cellPtr);
      addedCells[cellPtr] = true;
      seqCellGraph.HyperGraphAddNode(cellPtr);
    }
    for (int i = 0; i < relatedCells.size(); i++) { 
      Cell& endCell = *(Cell *)(relatedCells[i]);
      pair<Pin *, Pin *> pinPair = relatedPins[i];
      vector<void*> listOfCells;
      long int edgeIndex;
      listOfCells.push_back(cellPtr);
      listOfCells.push_back(&endCell);
      if (addedCells.find(&endCell) == addedCells.end()) {
	seqCellGraph.HyperGraphAddNode(&endCell);
	addedCells[&endCell] = true;
	inputCells.push_back(&endCell);
      }
      if (0 || debugPrint) cout << endl;
      if (0 || debugPrint) {cout << "DBG: Size of list of cells: " << listOfCells.size() << endl;}
      if (0 || debugPrint) {cout << "DBG CELL: " << (*(Cell *)listOfCells[0]).CellGetName() << " " << (*(Cell *)listOfCells[1]).CellGetName() << endl;}
      if (0 || debugPrint) {cout << "DBG PINS: " << (*(Pin *)(relatedPins[i].first)).PinGetName() << " " << (*(Pin *)(relatedPins[i].second)).PinGetName() << endl;}
      /* If the edge exists increment the weight of the edge */
      edgeIndex = seqCellGraph.HyperGraphNodesAreAdjacent(listOfCells[0], listOfCells[1]);
      if (edgeIndex == -1) {
	/* If the edge does not exist, add the edge */
	seqCellGraph.HyperGraphAddEdge(listOfCells, NIL(void *), 1);
	edgeIndex = seqCellGraph.HyperGraphNodesAreAdjacent(listOfCells[0], listOfCells[1]);
	if (0 || debugPrint) {cout << "DBG: edgeIndex for the newly added edge is " << edgeIndex << endl;}
      }
      pinPairsOfEdge[edgeIndex] = relatedPins[i];
    } 
  } DESIGN_END_FOR;
  _STEP_END("Building graph for sequential cells");
  if (0) {
    for (int i = 0; i < inputCells.size(); i++) {
      Cell *cellPtri = inputCells[i];
      for (int j = 0; j < inputCells.size(); j++) {
	if (i == j) {
	  continue;
	}
	Cell *cellPtrj = inputCells[j];
	int edgeIdx;
	edgeIdx = seqCellGraph.HyperGraphNodesAreAdjacent(cellPtri, cellPtrj);
	if (edgeIdx  >= 0) {
	  if (0 || debugPrint) cout << endl;
	  if (0 || debugPrint) cout << "Src cell: " << (*cellPtri).CellGetName() << "   ";
	  if (0 || debugPrint) cout << "Dest cell: " << (*cellPtrj).CellGetName() << endl;
	  if (0 || debugPrint) cout << "Edge index: " << edgeIdx << endl;
	  if (0 || debugPrint) cout << "First pin of pair: " << ((*(Pin *)(pinPairsOfEdge[edgeIdx].first)).PinGetName()) << endl;
	  if (0 || debugPrint) cout << "Second pin of pair: " << ((*(Pin *)(pinPairsOfEdge[edgeIdx].second)).PinGetName()) << endl;
	} else {
	  //	cout << "No Edge exists between these cells" << endl;
	}
      }
    }
  }
  mosekSolveQuadratic(*this, inputCells, seqCellGraph, pinPairsOfEdge);
}


void
Design::DesignSolveForSeqCellsOld(void)
{
  Design &myDesign = *this;
  HyperGraph seqCellGraph;
  string cellName;
  Cell *cellPtr;
  map<Cell*, bool> addedCells;
  map<unsigned int, pair<Pin*, Pin* > > pinPairsOfEdge;
  vector<Cell*> inputCells;
  
  _STEP_BEGIN("Building graph for sequential cells");
  /* Traverse the input cone of each sequential cell 
     to find a port or another cell to pair with */

  /* Force addition */
  if (0)  {
  DESIGN_FOR_ALL_SEQ_CELLS(myDesign, cellName, cellPtr) {
    vector<Cell*> tmpRelatedCells;
    vector<pair<Pin*, Pin* > > tmpRelatedPins;
    vector<unsigned int> tmpLogicalDepths;
    vector<Cell*> relatedCells;
    vector<pair<Pin*, Pin* > > relatedPins;
    vector<unsigned int> logicalDepths;

    Cell& seqCell = *cellPtr;
    Cell* tmpCellPtr;
    pair<Pin*,Pin*> tmpPinPair;
    unsigned int tmpLogicalDepth;

    if (!((*cellPtr).CellIsSequential())) {
      if (debugPrint) cout<< "Error in iterator" << endl;
      exit(0);
    }
    debugPrint = true;
    //    if (debugPrint) cout << "BEGIN INPUT CONE TRAVERSAL for cell " << seqCell.CellGetName() << " (" << seqCell.CellGetOrigName() << ") " << endl;
    debugPrint = false;
    /* Get the input logic cone */
    getLogicCone(seqCell, tmpRelatedCells, tmpLogicalDepths, tmpRelatedPins, 
		 PIN_DIR_INPUT, false);
    VECTOR_FOR_ALL_ELEMS(tmpRelatedCells, Cell*, tmpCellPtr) {
      relatedCells.push_back(tmpCellPtr);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpRelatedPins, pair<Pin* MCOMMA Pin*>, tmpPinPair) {
      relatedPins.push_back(tmpPinPair);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpLogicalDepths, unsigned int, tmpLogicalDepth) {
      logicalDepths.push_back(tmpLogicalDepth);
    } END_FOR;

    debugPrint = true;
    //    if (debugPrint) cout << "BEGIN INPUT CONE TRAVERSAL for cell " << seqCell.CellGetName() << " (" << seqCell.CellGetOrigName() << ") " << endl;
    debugPrint = false;
    /* Clear vectors */
    tmpRelatedCells.clear(); tmpLogicalDepths.clear(); tmpRelatedPins.clear();
    getLogicCone(seqCell, tmpRelatedCells, tmpLogicalDepths, tmpRelatedPins, 
		 PIN_DIR_OUTPUT, true);
    VECTOR_FOR_ALL_ELEMS(tmpRelatedCells, Cell*, tmpCellPtr) {
      relatedCells.push_back(tmpCellPtr);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpRelatedPins, pair<Pin* MCOMMA Pin*>, tmpPinPair) {
      relatedPins.push_back(tmpPinPair);
    } END_FOR;
    VECTOR_FOR_ALL_ELEMS(tmpLogicalDepths, unsigned int, tmpLogicalDepth) {
      logicalDepths.push_back(tmpLogicalDepth);
    } END_FOR;
    
    /* Add the subject sequential cell into the graph and 
       the list of input cells  */
    if (addedCells.find(cellPtr) == addedCells.end()) {
      inputCells.push_back(cellPtr);
      addedCells[cellPtr] = true;
      seqCellGraph.HyperGraphAddNode(cellPtr);
    }
    for (int i = 0; i < relatedCells.size(); i++) { 
      Cell& endCell = *(Cell *)(relatedCells[i]);
      pair<Pin *, Pin *> pinPair = relatedPins[i];
      vector<void*> listOfCells;
      long int edgeIndex;
      listOfCells.push_back(cellPtr);
      listOfCells.push_back(&endCell);
      if (addedCells.find(&endCell) == addedCells.end()) {
	seqCellGraph.HyperGraphAddNode(&endCell);
	addedCells[&endCell] = true;
	inputCells.push_back(&endCell);
      }
      if (0 || debugPrint) cout << endl;
      if (0 || debugPrint) {cout << "DBG: Size of list of cells: " << listOfCells.size() << endl;}
      if (0 || debugPrint) {cout << "DBG CELL: " << (*(Cell *)listOfCells[0]).CellGetName() << " " << (*(Cell *)listOfCells[1]).CellGetName() << endl;}
      if (0 || debugPrint) {cout << "DBG PINS: " << (*(Pin *)(relatedPins[i].first)).PinGetName() << " " << (*(Pin *)(relatedPins[i].second)).PinGetName() << endl;}
      /* If the edge exists increment the weight of the edge */
      edgeIndex = seqCellGraph.HyperGraphNodesAreAdjacent(listOfCells[0], listOfCells[1]);
      if (edgeIndex == -1) {
	/* If the edge does not exist, add the edge */
	seqCellGraph.HyperGraphAddEdge(listOfCells, NIL(void *), 1);
	edgeIndex = seqCellGraph.HyperGraphNodesAreAdjacent(listOfCells[0], listOfCells[1]);
	if (0 || debugPrint) {cout << "DBG: edgeIndex for the newly added edge is " << edgeIndex << endl;}
      }
      pinPairsOfEdge[edgeIndex] = relatedPins[i];
    } 
  } DESIGN_END_FOR;
  }
  void *seqCell1;
  void *seqCell2;
  void *portCell1;
  void *portCell2;
  seqCell1 = (void *)myDesign.DesignGetNode("o0");
  seqCell2 = (void *)myDesign.DesignGetNode("o1");
  portCell1 = (void *)myDesign.DesignGetNode("o2644");
  portCell2 = (void *)myDesign.DesignGetNode("o2661");

  seqCellGraph.HyperGraphAddNode(seqCell1); inputCells.push_back((Cell *)seqCell1);
  seqCellGraph.HyperGraphAddNode(seqCell2); inputCells.push_back((Cell *)seqCell2);
  seqCellGraph.HyperGraphAddNode(portCell1); inputCells.push_back((Cell *)portCell1);
  seqCellGraph.HyperGraphAddNode(portCell2); inputCells.push_back((Cell *)portCell2);
  
  vector<void*> listOfCells;
  unsigned int edgeIdx;
  listOfCells.push_back(seqCell1); listOfCells.push_back(seqCell2);
  seqCellGraph.HyperGraphAddEdge(listOfCells, NIL(void *), 1);
  edgeIdx = seqCellGraph.HyperGraphNodesAreAdjacent(seqCell1, seqCell2);
  Pin *pin1 = (*(Cell*)seqCell1).CellGetPinByName("o0_2");
  if (pin1 == NIL(Pin *)) {
    cout << "o0_2 not found " << endl;
  }
  Pin *pin2 = (*(Cell*)seqCell2).CellGetPinByName("o1_1");
  if (pin2 == NIL(Pin *)) {
    cout << "o1_1 not found " << endl;
  }
  pinPairsOfEdge[edgeIdx] = make_pair(pin1, pin2);
  listOfCells.clear();

  listOfCells.push_back(portCell1); listOfCells.push_back(seqCell1);
  seqCellGraph.HyperGraphAddEdge(listOfCells, NIL(void *), 1);
  edgeIdx = seqCellGraph.HyperGraphNodesAreAdjacent(portCell1, seqCell1);
  pin1 = (*(Cell *)portCell1).CellGetPinByName("o2644_1");
  if (pin1 == NIL(Pin *)) {
    cout << "o2644_1 not found " << endl;
  }
  pin2 = (*(Cell *)seqCell1).CellGetPinByName("o0_1");
  if (pin2 == NIL(Pin *)) {
    cout << "o0_1 not found " << endl;
  }

  pinPairsOfEdge[edgeIdx] = make_pair(pin1, pin2);
  listOfCells.clear();

  listOfCells.push_back(seqCell2); listOfCells.push_back(portCell2);
  seqCellGraph.HyperGraphAddEdge(listOfCells, NIL(void *), 1);
  edgeIdx = seqCellGraph.HyperGraphNodesAreAdjacent(seqCell2, portCell2);
  pin1 = (*(Cell *)seqCell2).CellGetPinByName("o1_2");
  if (pin1 == NIL(Pin *)) {
    cout << "o1_2 not found " << endl;
  }
  pin2 = (*(Cell *)portCell2).CellGetPinByName("o2661_1");
  if (pin2 == NIL(Pin *)) {
    cout << "o2661_1 not found " << endl;
  }

  pinPairsOfEdge[edgeIdx] = make_pair(pin1, pin2);
  listOfCells.clear();
  

  _STEP_END("Building graph for sequential cells");
  debugPrint = true;
  if (1) {
    for (int i = 0; i < inputCells.size(); i++) {
      Cell *cellPtri = inputCells[i];
      for (int j = 0; j < inputCells.size(); j++) {
	if (i == j) {
	  continue;
	}
	Cell *cellPtrj = inputCells[j];
	int edgeIdx;
	edgeIdx = seqCellGraph.HyperGraphNodesAreAdjacent(cellPtri, cellPtrj);
	if (edgeIdx  >= 0) {
	  if (debugPrint) cout << endl;
	  if (debugPrint) cout << "Src cell: " << (*cellPtri).CellGetName() << "   ";
	  if (debugPrint) cout << "Dest cell: " << (*cellPtrj).CellGetName() << endl;
	  if (debugPrint) cout << "Edge index: " << edgeIdx << endl;
	  if (debugPrint) cout << "First pin of pair: " << ((*(Pin *)(pinPairsOfEdge[edgeIdx].first)).PinGetName()) << endl;
	  if (debugPrint) cout << "Second pin of pair: " << ((*(Pin *)(pinPairsOfEdge[edgeIdx].second)).PinGetName()) << endl;
	} else {
	  //	cout << "No Edge exists between these cells" << endl;
	}
      }
    }
  }
  debugPrint = false;
  mosekSolveQuadratic(*this, inputCells, seqCellGraph, pinPairsOfEdge);
}

