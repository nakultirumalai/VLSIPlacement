# include <CellSpread.h>

# define FORCE_DIR_NO_FORCE 0
# define FORCE_DIR_FIRST_QUAD 1
# define FORCE_DIR_SECOND_QUAD 2
# define FORCE_DIR_THIRD_QUAD 3
# define FORCE_DIR_FOURTH_QUAD 4
# define FORCE_DIR_RIGHT 5
# define FORCE_DIR_LEFT 6
# define FORCE_DIR_TOP 7
# define FORCE_DIR_BOT 8
# define MAX_DBL 1000000000000000

void printForceDir(char forceDir) 
{
  switch(forceDir) {
  case FORCE_DIR_NO_FORCE: cout << "No force on cell"; break;
  case FORCE_DIR_RIGHT: cout << "Force of attraction towards right"; break;
  case FORCE_DIR_LEFT: cout << "Force of attraction towards left"; break;
  case FORCE_DIR_TOP: cout << "Force of attraction towards top"; break;
  case FORCE_DIR_BOT: cout << "Force of attraction towards bot"; break;
  case FORCE_DIR_FIRST_QUAD: cout << "Force of attraction in quadrant I"; break;
  case FORCE_DIR_SECOND_QUAD: cout << "Force of attraction in quadrant II"; break;
  case FORCE_DIR_THIRD_QUAD: cout << "Force of attraction in quadrant III"; break;
  case FORCE_DIR_FOURTH_QUAD: cout << "Force of attraction in quadrant IV"; break;
  default: cout << "Unknown direction of force";
  };
}

Cell*
createPseudoPort(Design &myDesign, uint xPos, uint yPos)
{
  Cell *newPort;

  newPort = new Cell(/* Port height */56, /*Port Width */56,
                     xPos, yPos);
  (*newPort).CellSetIsTerminal(true);
  (*newPort).CellSetIsPort(true);

  return (newPort);
}

/*******************************************************************************
  GIVEN A CELL, RETURNS THE MAGNITUDE OF FORCE ACTING ON IT. ALSO SPECIFIES 
  WHICH QUADRANT THE FORCE IS POINTED TOWARDS: FIRST, SECOND, THIRD OR FOURTH.
  RETURNS THE INDIVIDUAL COMPONENTS. 
  THIS FUNCTION CAN BE USED TO COMPUTE THE FORCE RIGHT AFTER QUADRATIC 
  OPTIMIZATION.
*******************************************************************************/
void
CellSpreadGetForceOnCell(Design &myDesign, Cell &thisCell, 
			 double newXPos, double newYPos,
			 double &magnitude,
			 double &totalXForce, double &totalYForce,
			 char &forceDir, double &chipBoundLeft, 
			 double &chipBoundRight, double &chipBoundTop,
			 double &chipBoundBot)
{
  Pin *pini, *pinj;
  double celliXPos, celliYPos, celljXPos, celljYPos;
  double piniXOffset, piniYOffset, pinjXOffset, pinjYOffset;
  double diffCellXPos, diffCellYPos, diffPinXOffPos, diffPinYOffPos;
  double totalXDiffDist, totalYDiffDist;
  double netWeight;
  uint maxx, maxy;
  
  if (newXPos == -1) celliXPos = CellGetDblX(&thisCell);
  else celliXPos = newXPos;
  if (newYPos == -1) celliYPos = CellGetDblY(&thisCell);
  else celliYPos = newYPos;

  totalXForce = 0.0;
  totalYForce = 0.0;
  magnitude = 0.0;
  forceDir = FORCE_DIR_NO_FORCE;
  chipBoundLeft = 0;
  chipBoundRight = 0;
  chipBoundTop = 0;
  chipBoundBot = 0;
  diffCellXPos = 0.0;
  diffCellYPos = 0.0;

  CELL_FOR_ALL_PINS_NOFILT(thisCell, PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    netWeight = connectedNet.NetGetWeight();
    piniXOffset = (*pini).PinGetXOffset();
    piniYOffset = (*pini).PinGetYOffset();
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      Cell &connectedCell = ((*pinj).PinGetParentCell());
      if (pinj == pini) continue;
      pinjXOffset = (*pinj).PinGetXOffset();
      pinjYOffset = (*pinj).PinGetYOffset();
      diffCellXPos -= netWeight * (celliXPos + piniXOffset);
      diffCellYPos -= netWeight * (celliYPos + piniYOffset);
      if (!connectedCell.CellIsTerminal()) {
	celljXPos = CellGetDblX(&connectedCell);
	celljYPos = CellGetDblY(&connectedCell);
      } else {
	celljXPos = connectedCell.CellGetXpos();
	celljYPos = connectedCell.CellGetYpos();
      }
      diffCellXPos += netWeight * (celljXPos + pinjXOffset);
      diffCellYPos += netWeight * (celljYPos + pinjYOffset);
    } NET_END_FOR;
  } CELL_END_FOR;

  totalXForce = diffCellXPos; 
  totalYForce = diffCellYPos;
  int intXForce, intYForce;
  intXForce = dtoi(totalXForce); intYForce = dtoi(totalYForce);
  if (intXForce == 0 && intYForce == 0) {
    totalXForce = intXForce;
    totalYForce = intYForce;
    return;
  }
  magnitude = sqrt((totalXForce * totalXForce + totalYForce * totalYForce));
  /* Find the equation of the line y = mx + c and its intersection 
     with the chip boundaries */
  double m, c;
  bool parallel, perpendicular;
  parallel = false; perpendicular = false;
  if (intXForce == 0) {
    m = 0;
    chipBoundLeft = -MAX_DBL;
    chipBoundRight = -MAX_DBL;
    chipBoundTop = maxx;
    chipBoundBot = 0.0;
    perpendicular = true;
    if (intYForce > 0) {
      forceDir = FORCE_DIR_TOP;
    } else if (intYForce < 0) {
      forceDir = FORCE_DIR_BOT;
    }
  } else if (intYForce == 0)  {
    chipBoundTop = -MAX_DBL;
    chipBoundBot = -MAX_DBL;
    chipBoundLeft = 0.0;
    chipBoundRight = maxy;
    parallel = true;
    if (intXForce > 0) {
      forceDir = FORCE_DIR_RIGHT;
    } else if (intXForce < 0) {
      forceDir = FORCE_DIR_LEFT;      
    }
  } else {
    m = ((double)intYForce) / intXForce;
    c = (celliYPos - m * celliXPos);
    myDesign.DesignGetBoundingBox(maxx, maxy);
    /* x-Coordinate of the top boundary */
    chipBoundTop = (maxy - c)/m;
    chipBoundLeft = c;
    chipBoundRight = (maxx * m) + c;
    chipBoundBot = -c / m;
    if (intXForce > 0 && intYForce > 0) {
      forceDir = FORCE_DIR_FIRST_QUAD;
    } else if (intXForce < 0 && intYForce > 0) {
      forceDir = FORCE_DIR_SECOND_QUAD;
    } else if (intXForce < 0 && intYForce < 0) {
      forceDir = FORCE_DIR_THIRD_QUAD;
    } else if (intXForce > 0 && intYForce < 0) {
      forceDir = FORCE_DIR_FOURTH_QUAD;
    }
  }
}

void
CellSpreadCreatePseudoPort(Design &myDesign, HyperGraph &myGraph, 
			   Cell &thisCell, double newXpos, double newYpos,
			   double chipBoundLeft, double chipBoundRight, 
			   double chipBoundTop, double chipBoundBot, 
			   double magnitude, char forceDir)
{
  double spreadForce, springConstant;
  double pseudoPinX, pseudoPinY;
  double cellXpos, cellYpos;
  double portXForce, portYForce;
  uint minx, miny, maxx, maxy;

  minx = miny = 0;
  myDesign.DesignGetBoundingBox(maxx, maxy);
  cellXpos = newXpos;
  cellYpos = newYpos;

  switch (forceDir) {
  case FORCE_DIR_NO_FORCE: return;
  case FORCE_DIR_LEFT: pseudoPinX = maxx;
    pseudoPinY = chipBoundRight; break;
  case FORCE_DIR_RIGHT: pseudoPinX = minx;
    pseudoPinY = chipBoundLeft; break;
  case FORCE_DIR_TOP: pseudoPinY = miny;
    pseudoPinX = chipBoundBot; break;
  case FORCE_DIR_BOT: pseudoPinY = maxy;
    pseudoPinX = chipBoundTop; break;
  case FORCE_DIR_FIRST_QUAD: 
    if (chipBoundLeft <= miny) {
      pseudoPinX = chipBoundBot; pseudoPinY = 0.0;
    } else if (chipBoundBot <= minx) {
      pseudoPinX = 0; pseudoPinY = chipBoundLeft;
    }
    break;
  case FORCE_DIR_SECOND_QUAD:
    if (chipBoundRight <= miny) {
      pseudoPinX = chipBoundBot; pseudoPinY = miny;
    } else if (chipBoundBot >= maxx) {
      pseudoPinX = maxx; pseudoPinY = chipBoundRight;
    }
    break;
  case FORCE_DIR_THIRD_QUAD:
    if (chipBoundTop >= maxx) {
      pseudoPinX = maxx; pseudoPinY = chipBoundRight;
    } else if (chipBoundRight >= maxy) {
      pseudoPinX = chipBoundTop; pseudoPinY = maxy;
    }
    break;
  case FORCE_DIR_FOURTH_QUAD:
    if (chipBoundLeft >= maxy) {
      pseudoPinX = chipBoundTop; pseudoPinY = maxy;
    } else if (chipBoundTop <= minx) {
      pseudoPinX = minx; pseudoPinY = chipBoundLeft;
    }
    break;
  default: cout << "DEFAULT CASE NOT EXPECTED" << endl;
  };

  /* Since we are minimizing quadratic wirelength. Model of force for 
     other objective types is going to change */
  portXForce = pseudoPinX - cellXpos;
  portYForce = pseudoPinY - cellYpos;
  spreadForce = sqrt(portXForce * portXForce + portYForce * portYForce);
  springConstant = magnitude / spreadForce;

  cout << "DBG: S.constant :" << springConstant << " PSPX: " << pseudoPinX << " PSPY: " << pseudoPinY
       << " pXFrc: " << portXForce << " pYFrc: " <<portYForce << endl;

  if (debugPrint) {
    cout << "Created pseudopin: X: " << pseudoPinX
	 << "  Y: " << pseudoPinY << endl;
    cout << "Diff of cell from pseudo pin in X : " << (pseudoPinX - cellXpos) << endl;
    cout << "Diff of cell from pseudo pin in Y : " << (pseudoPinY - cellYpos) << endl;
    cout << "Total force acting on cell from pseudo pin in X : " << springConstant * (pseudoPinX - cellXpos) << endl;
    cout << "Total force acting on cell from pseudo pin in Y : " << springConstant * (pseudoPinY - cellYpos) << endl;
    cout << "Spring constant: " << springConstant << endl;
  }
  
  Cell *pseudoPort;
  pseudoPort = (Cell *)CellGetPseudoPort(&thisCell);
  if (pseudoPort == NIL(Cell *)) {
    /* Create a pseudo pin on the chip boundary */
    pseudoPort = createPseudoPort(myDesign, dtoi(pseudoPinX), dtoi(pseudoPinY));
    CellSetPseudoPort((&thisCell), pseudoPort);
    Pin *portPin = new Pin(0, 0, 0, PIN_DIR_INPUT, "**ppp**");
    (*pseudoPort).CellAddPin(portPin);
    CellSetIsPseudo(pseudoPort);

    /* Create a pseudo net between the cell pseudo pin
       and the pseudo pin on the chip boundary */
    Pin *pseudoPin = new Pin(0, 0, 0, PIN_DIR_INPUT, "**ppc**");
    thisCell.CellAddPin(pseudoPin);
    PinSetIsPseudo(pseudoPin);

    /* Create the pseudo net */
    Net *netPtr = new Net(0, "**pn**");
    myDesign.DesignAddPseudoNet(netPtr);
    NetSetIsPseudo(netPtr);

    /* Make the connection */
    (*netPtr).NetAddPin(*portPin);
    (*portPin).Connect((*netPtr));

    (*netPtr).NetAddPin(*pseudoPin);
    (*pseudoPin).Connect((*netPtr));

    (*netPtr).NetSetWeight(springConstant);
    /* Add the net to the hypergraph */
    myGraph.HyperGraphAddNode(pseudoPort);
    /* Add an edge to the hypergraph with weight equal to spring constant */
    vector<void *> nodeList;
    nodeList.push_back((void *)(pseudoPort));
    nodeList.push_back((void *)(&thisCell));
    myGraph.HyperGraphAddEdge(nodeList, netPtr, springConstant);
  } else {
    Net *netPtr;
    (*pseudoPort).CellSetXpos(pseudoPinX);
    (*pseudoPort).CellSetYpos(pseudoPinY);
    CELL_FOR_ALL_PINS_NOFILT((*pseudoPort), PIN_DIR_ALL, pini) {
      (*pini).PinSetXOffset(0);
      (*pini).PinSetYOffset(0);
      netPtr = &((*pini).PinGetNet());
    } CELL_END_FOR;
    int edgeIdx;
    edgeIdx = myGraph.HyperGraphNodesAreAdjacent(pseudoPort, &thisCell);
    (*netPtr).NetSetWeight(springConstant);
    if (edgeIdx >= 0) {
      myGraph.SetEdgeWeight(edgeIdx, springConstant);
    } else {
      cout << "Error: Created edge not found! Exiting... " << endl;
      exit(0);
    } 
  }
}

/*******************************************************************************
  FUNCTION THAT DOES THE ACTUAL CELL SPREADING
*******************************************************************************/
void
CellSpreadInBin(Design &myDesign, HyperGraph &myGraph,
		Bin *binPtr, double newBinRight, double newBinTop, 
		double newBinRightPrev, double newBinTopPrev, double maxUtil)
{
  Cell *cellPtr;
  vector<Cell *>& binCells = (*binPtr).BinGetCells();
  uint binRight, binTop, binLeft, binBot;
  uint xj, yj;
  uint cellWidth, cellHeight;
  uint maxx, maxy;
  double xjPrime, yjPrime;
  double newXPos, newYPos;
  bool noXSpread, noYSpread;
  double alphaX, alphaY;
  double magnitude, totalXForce, totalYForce;
  double chipBoundLeft, chipBoundRight;
  double chipBoundTop, chipBoundBot;
  double averageCellWidth;
  char forceDir;

  (*binPtr).BinGetBoundingBox(binLeft, binRight, binBot, binTop);
  myDesign.DesignGetBoundingBox(maxx, maxy);
  noXSpread = false; noYSpread = false;
  if (newBinRight == binRight) noXSpread = true;
  if (newBinTop == binTop) noYSpread = true;
  if (noXSpread && noYSpread) return;

  averageCellWidth = (*binPtr).BinGetAverageCellWidth();
  VECTOR_FOR_ALL_ELEMS(binCells, Cell*, cellPtr) {
    Cell &thisCell = (*cellPtr);
    xj = thisCell.CellGetXpos();
    yj = thisCell.CellGetYpos();
    cellWidth = thisCell.CellGetWidth();
    cellHeight = thisCell.CellGetHeight();
    newXPos = xj; newYPos = yj;
    if (!noXSpread) {
      xjPrime = newBinRight * (xj - binLeft);
      xjPrime += newBinRightPrev * (binRight - xj);
      xjPrime /= (binRight - binLeft);
      alphaX = 0.5 + ((0.5/maxUtil) * (averageCellWidth/cellHeight));
      newXPos = xj + alphaX * (xjPrime - xj);
    }
    if (!noYSpread) {
      yjPrime = newBinTop * (yj - binBot);
      yjPrime += newBinTopPrev * (binTop - yj);
      yjPrime /= (binTop - binBot);
      alphaY = 0.8 + (0.5/maxUtil);
      newYPos = yj + alphaY * (yjPrime - yj);
    }
    cout <<"DBG: CELL: " << thisCell.CellGetName() << " OLD: X: " << xj <<" Y: " << yj <<" NEW: X: " << newXPos << " Y: "<< newYPos << " B: OLD R : " << binRight << " OLD T:" << binTop << " NEW R: " << newBinRight << " NEW T: " << newBinTop << endl;

    double oldXForce, oldYForce, oldMagnitude;
    char oldForceDir;
    if (debugPrint) {
      CellSpreadGetForceOnCell(myDesign, thisCell, -1, -1, oldMagnitude,
			       oldXForce, oldYForce, oldForceDir, 
			       chipBoundLeft, chipBoundRight, chipBoundTop, 
			       chipBoundBot);
    }
    CellSpreadGetForceOnCell(myDesign, thisCell, newXPos, newYPos, magnitude,
			     totalXForce, totalYForce, forceDir, chipBoundLeft, 
			     chipBoundRight, chipBoundTop, chipBoundBot);


    if (debugPrint) {
      cout << "Cell: " << thisCell.CellGetName() << endl;
      cout << "xj\txj'\tyj\tyj'\ttotXF\ttotYF\ttot" << endl;
      cout << xj << "\t" << newXPos << "\t" << yj << "\t" << newYPos << "\t" 
	   << totalXForce << "\t" << totalYForce << "\t" << magnitude << endl;
      cout << "Force direction: ";
      printForceDir(forceDir);
      cout << endl;
    }
    CellSpreadCreatePseudoPort(myDesign, myGraph, thisCell, newXPos, newYPos,
			       chipBoundLeft, chipBoundRight, chipBoundTop,
			       chipBoundBot, magnitude, forceDir);
  } END_FOR;
}

/*******************************************************************************
  FUNCTION TO DO CELL SPREADING, DIVIDE THE ENTIRE CHIP AREA INTO EQUAL SIZED
  BINS AND COMPUTE THEIR UTILIZATION. AFTER SPREADING, RETURNS THE PEAK 
  UTILIZATION OF ALL THE BINS.
*******************************************************************************/
double
CellSpreadInDesignFastPlace(Design &myDesign, HyperGraph &myGraph)
{
  Bin *binPtr;
  vector<Bin *> DesignBins;
  vector<Cell *> cellsOfBin;
  double xiMinus1, xi, xiPlus1;
  double newXiMinus1, newYiMinus1;
  double yiMinus1, yi, yiPlus1;
  double delta, maxUtil;
  double Ui, UiPlus1, xiPrime, yiPrime;
  uint binIdx, numBins;
  int startBinIdx, nextBinIdx, prevBinIdx;
  Bin *prevBinPtr, *nextBinPtr, *curBinPtr;

  DesignBins = myDesign.DesignGetBins();
  numBins = DesignBins.size();
  /* Set the value of delta = 1.5 */
  delta = 1.5; maxUtil = myDesign.DesignGetPeakUtil();
  /* Assign bin idx to the start first row */
  startBinIdx = 0;
  do {
    /* Traverse each row */
    binIdx = startBinIdx;
    do {
      xiMinus1 = 0; 
      prevBinIdx = myDesign.DesignGetPrevRowBinIdx(binIdx);
      nextBinIdx = myDesign.DesignGetNextRowBinIdx(binIdx);
      if (prevBinIdx >= 0) {
	xiMinus1 = (*(DesignBins[prevBinIdx])).BinGetRight();
      }
      if (nextBinIdx <= 0) {
	break;
      }
      curBinPtr = DesignBins[binIdx];
      nextBinPtr = DesignBins[nextBinIdx];
      Ui = (*curBinPtr).BinGetUtilization();
      UiPlus1 = (*nextBinPtr).BinGetUtilization();
      xi = (*curBinPtr).BinGetRight();
      xiPlus1 = (*nextBinPtr).BinGetRight();
      xiPrime = (xiMinus1) * (UiPlus1 + delta) + (xiPlus1) * (Ui + delta);
      xiPrime /= (Ui + UiPlus1 + 2 * delta);
      (*curBinPtr).BinSetNewRight(xiPrime);
      binIdx = nextBinIdx;
    } while (1);
    startBinIdx = myDesign.DesignGetNextColBinIdx(startBinIdx);
  } while (startBinIdx >= 0);
  startBinIdx = 0;
  do {
    /* Traverse each column */
    binIdx = startBinIdx;
    do {
      yiMinus1 = 0; 
      prevBinIdx = myDesign.DesignGetPrevColBinIdx(binIdx);
      nextBinIdx = myDesign.DesignGetNextColBinIdx(binIdx);
      if (prevBinIdx >= 0) {
	yiMinus1 = (*(DesignBins[prevBinIdx])).BinGetTop();
      }
      if (nextBinIdx <= 0) {
	break;
      }
      curBinPtr = DesignBins[binIdx];
      nextBinPtr = DesignBins[nextBinIdx];
      Ui = (*curBinPtr).BinGetUtilization();
      UiPlus1 = (*nextBinPtr).BinGetUtilization();
      yi = (*curBinPtr).BinGetTop();
      yiPlus1 = (*nextBinPtr).BinGetTop();
      yiPrime = (yiMinus1) * (UiPlus1 + delta) + (yiPlus1) * (Ui + delta);
      yiPrime /= (Ui + UiPlus1 + 2 * delta);
      (*curBinPtr).BinSetNewTop(yiPrime);
      binIdx = nextBinIdx;
    } while (1);
    startBinIdx = myDesign.DesignGetNextRowBinIdx(startBinIdx);
  } while (startBinIdx >= 0);
  
  
  /* For each bin, iterate over all the cells of the bin
     and compute their new X Y position resulting from 
     spreading and create the pseudo nets and ports to 
     bring the cell to the same position */
  double prevBinNewTop, prevBinNewRight;
  double thisBinNewTop, thisBinNewRight;
  for (binIdx = 0; binIdx < numBins; binIdx++) {
    prevBinNewRight = 0; prevBinNewTop = 0;
    prevBinIdx = myDesign.DesignGetPrevRowBinIdx(binIdx);
    if (prevBinIdx != -1) {
      prevBinNewRight = (*(DesignBins[prevBinIdx])).BinGetNewRight();
    } 
    prevBinIdx = myDesign.DesignGetPrevColBinIdx(binIdx);
    if (prevBinIdx != -1) {
      prevBinNewTop = (*(DesignBins[prevBinIdx])).BinGetNewTop();
    } 
    curBinPtr = DesignBins[binIdx];
    if (debugPrint) {
      if (!((*curBinPtr).BinGetUtilization() == 0)) {
	(*curBinPtr).BinPrintBin();
	cout << "Previous bin new Right" << prevBinNewRight << endl;
	cout << "Previous bin new Top" << prevBinNewTop << endl;
      }
    }
    thisBinNewTop = (*curBinPtr).BinGetNewTop();
    thisBinNewRight = (*curBinPtr).BinGetNewRight();
    CellSpreadInBin(myDesign, myGraph, curBinPtr, thisBinNewRight, thisBinNewTop,
		    prevBinNewRight, prevBinNewTop, maxUtil);
  }
}
