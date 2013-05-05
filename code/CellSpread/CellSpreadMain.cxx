# include <CellSpread.h>

void
CellSpreadSetCellPosition(Cell &thisCell, uint newPos, char dir)
{
  if (dir == XDIRECTION) {
    thisCell.CellSetXpos(newPos);
  } else if (dir == YDIRECTION) {
    thisCell.CellSetYpos(newPos);
  }
} 

uint
CellSpreadGetCellPosition(Cell &thisCell, char dir)
{
  uint rtv;

  rtv = 0;
  if (dir == XDIRECTION) {
    rtv = thisCell.CellGetXpos();
  } else if (dir == YDIRECTION) {
    rtv = thisCell.CellGetYpos();
  }
  
  return rtv;
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

void
CellSpreadCreatePseudoPort(Design &myDesign, HyperGraph &myGraph, 
			   Cell &thisCell, uint maxx, uint maxy,
			   char dir)
{
  double resultantForce;
  double totalXDiffDist = 0.0;
  double totalYDiffDist = 0.0;
  double springConstant;
  double pseudoPinX;
  double pseudoPinY;
  double cellXpos, cellYpos;
  double slope;
  uint piniXAbsPos, piniYAbsPos;
  uint pinjXAbsPos, pinjYAbsPos;
  double lefty, righty, topx, botx;
  bool noPseudoPin;

  cellXpos = thisCell.CellGetXpos();
  cellYpos = thisCell.CellGetYpos();

  /* Compute the total force on the cell because of the connected pins */
  Pin *pini, *pinj;
  CELL_FOR_ALL_PINS(thisCell, PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    piniXAbsPos = cellXpos + (*pini).PinGetXOffset();
    piniYAbsPos = cellYpos + (*pini).PinGetYOffset();
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      Cell& connectedCell = (*pinj).PinGetParentCell();
      pinjXAbsPos = connectedCell.CellGetXpos() + (*pinj).PinGetXOffset();
      pinjYAbsPos = connectedCell.CellGetYpos() + (*pinj).PinGetYOffset();

      totalXDiffDist += (piniXAbsPos - pinjXAbsPos);
      totalYDiffDist += (piniYAbsPos - pinjYAbsPos);
      //      totalXDiffDist += (cellXpos - pinjXAbsPos);
      //      totalYDiffDist += (cellYpos - pinjYAbsPos);
    } NET_END_FOR;
  } CELL_END_FOR;


  /* Compute the slope of the force vector */
  if (totalXDiffDist != 0) {
    slope = totalYDiffDist/totalXDiffDist;
  } else {
    slope = INFINITY;
  }

  /* Check intersection with all boundaries */
  noPseudoPin = false;
  if (slope == 0) {
    if (totalXDiffDist > 0) {
      pseudoPinX = maxx;
      pseudoPinY = cellYpos;
    } else if (totalXDiffDist < 0) {
      pseudoPinX = 0;
      pseudoPinY = cellYpos;      
    } else if (totalXDiffDist == 0) {
      noPseudoPin = true;
    }
  } else if (slope == INFINITY) {
    if (totalYDiffDist > 0) {
      pseudoPinY = maxy;
      pseudoPinX = cellXpos;
    } else if (totalYDiffDist < 0) {
      pseudoPinY = 0;
      pseudoPinX = cellXpos;      
    } else if (totalYDiffDist == 0) {
      noPseudoPin = true;
    }
  } else {
    /* Find intersection with all boundaries */
    lefty = cellYpos - (slope * cellXpos);
    righty = slope * (maxx - cellXpos) + cellYpos;
    topx = (maxy - cellYpos + (slope * cellXpos)) / slope;
    botx = ((slope * cellXpos) - cellYpos) / slope;
    if (totalXDiffDist > 0 && totalYDiffDist > 0) {
      /* Choose top or right */
      if (topx > maxx) { pseudoPinX = maxx; pseudoPinY = righty; }
      else { pseudoPinX = topx; pseudoPinY = maxy; }
    } else if (totalXDiffDist > 0 && totalYDiffDist < 0) {
      /* Choose bot or right */
      if (botx > maxx) { pseudoPinX = maxx; pseudoPinY = righty; }
      else { pseudoPinX = botx; pseudoPinY = 0; }
    } else if (totalXDiffDist < 0 && totalYDiffDist > 0) {
      /* Choose top or left */
      if (topx < 0) { pseudoPinX = 0; pseudoPinY = lefty; }
      else { pseudoPinX = topx; pseudoPinY = maxy; }
    } else if (totalXDiffDist < 0 && totalYDiffDist < 0) {
      /* Choose bot or left */ 
      if (botx < 0) { pseudoPinX = 0; pseudoPinY = lefty; }
      else { pseudoPinX = botx; pseudoPinY = 0; }
    }
  }

  /* Since we are minimizing quadratic wirelength. Model of force for 
     other objective types is going to change */
  springConstant = sqrt((totalXDiffDist * totalXDiffDist) + (totalYDiffDist * totalYDiffDist));
  totalXDiffDist = pseudoPinX - cellXpos;
  totalYDiffDist = pseudoPinY - cellYpos;
  springConstant /= sqrt((totalXDiffDist * totalXDiffDist) + (totalYDiffDist * totalYDiffDist));

  Cell *pseudoPort;
  pseudoPort = (Cell *)CellGetPseudoPort(&thisCell);
  if (pseudoPort == NIL(Cell *)) {
    /* Create a pseudo pin on the chip boundary */
    pseudoPort = createPseudoPort(myDesign, dtoi(pseudoPinX), dtoi(pseudoPinY));
    CellSetPseudoPort((&thisCell), pseudoPort);
    Pin *portPin = new Pin(0, 0, 0, PIN_DIR_INPUT, "**ppp**");
    (*pseudoPort).CellAddPin(portPin);
    (*pseudoPort).CellSetIsTerminal(true);
    (*pseudoPort).CellSetIsPort(true);
    CellSetIsPseudo(pseudoPort);
    PinSetIsPseudo(portPin);

    /* Create a pseudo net between the cell pseudo pin
       and the pseudo pin on the chip boundary */
    Pin *pseudoPin = new Pin(0, 0, 0, PIN_DIR_INPUT, "**ppc**");
    thisCell.CellAddPin(pseudoPin);
    PinSetIsPseudo(pseudoPin);

    /* Create the pseudo net */
    Net *netPtr = new Net(0, "**pn**");
    NetSetIsPseudo(netPtr);

    /* Make the connection */
    (*netPtr).NetAddPin(*portPin);
    (*portPin).Connect((*netPtr));
    (*netPtr).NetAddPin(*pseudoPin);
    (*pseudoPin).Connect((*netPtr));

    /* Add the net to the hypergraph */
    myGraph.HyperGraphAddNode(pseudoPort);

    /* Add an edge to the hypergraph with weight equal to spring constant */
    vector<void *> nodeList;
    nodeList.push_back((void *)(pseudoPort));
    nodeList.push_back((void *)(&thisCell));
    myGraph.HyperGraphAddEdge(nodeList, netPtr, (springConstant/GRID_COMPACTION_RATIO));
  } else {
    (*pseudoPort).CellSetXpos(pseudoPinX);
    (*pseudoPort).CellSetYpos(pseudoPinY);
    CELL_FOR_ALL_PINS((*pseudoPort), PIN_DIR_ALL, pini) {
      (*pini).PinSetXOffset(0);
      (*pini).PinSetYOffset(0);
    } CELL_END_FOR;
  }
}


/*******************************************************************************
  FUNCTION TO DO CELL SPREADING, GIVEN A VECTOR OF CELLS, THE CURRENT ALPHA 
  VALUE AND THE WINDOW WIDTH ALONG WITH THE DIRECTION, GETS NEW POSITION OF A 
  CELL IN THE GIVEN DIRECTION, SPREADS THE CELL AND ATTACHES PSEUDO PORTS AND 
  PINS AND NETS TO THE CURRENT CELL TO MAKE SURE THE NEXT ITERATION HAS THE 
  CELL COME INTO THE SAME POSITION AFTER QUADRATIC MINIMIZATION
*******************************************************************************/
void
CellSpreadInDesign(Design &myDesign, HyperGraph &myGraph,
		   vector<Cell *> cellsSortedByDir, double alpha, 
		   uint windowWidth, char dir, uint &lastLinePos) 
{
  uint newLinePos;
  uint oldPos, newPos;
  uint maxx, maxy;
  uint idx;
  double weight = 1;

  /* Dir == 1 for x spreading and 0 for y spreading */
  if (lastLinePos == 0) {
    Cell &lastCell = *(cellsSortedByDir[cellsSortedByDir.size() - 1]);
    lastLinePos = CellSpreadGetCellPosition(lastCell, dir);
  }

  /* Return if the last line position has already hit the 
     upper limit */
  maxx = 119427;
  maxy = 119427;

  newLinePos = lastLinePos + alpha * windowWidth;
  if (dir == XDIRECTION && (newLinePos > maxx)) newLinePos = maxx;
  if (dir == YDIRECTION && (newLinePos > maxy)) newLinePos = maxy;
  
  /* Iterate over all the cells in the sorted vector */
  for (idx = cellsSortedByDir.size(); idx > 0; idx--) {
    Cell &thisCell = *(cellsSortedByDir[idx - 1]);
    oldPos = CellSpreadGetCellPosition(thisCell, dir);

    /* Weight can be added to selectively scale down the 
       movement of the new cell */
    newPos = dtoi(weight * (double)newLinePos/lastLinePos * oldPos);
    CellSpreadSetCellPosition(thisCell, newPos, dir);
  }
  for (idx = cellsSortedByDir.size(); idx > 0; idx--) {
    Cell &thisCell = *(cellsSortedByDir[idx - 1]);
    CellSpreadCreatePseudoPort(myDesign, myGraph, thisCell, 
			       maxx, maxy, dir);
  }    
  
  lastLinePos = newLinePos;
}

/*******************************************************************************
  FUNCTION TO DO CELL SPREADING, DIVIDE THE ENTIRE CHIP AREA INTO EQUAL SIZED
  BINS AND COMPUTE THEIR UTILIZATION 
*******************************************************************************/
void
CellSpreadInDesignFastPlace(Design &myDesign, HyperGraph &myGraph,
			    vector<Cell *> cellsSortedByDir, double alpha, 
			    uint windowWidth, char dir, uint &lastLinePos) 
{
  uint newLinePos;
  uint oldPos, newPos;
  uint maxx, maxy;
  uint idx;
  double weight = 1;

  /* Dir == 1 for x spreading and 0 for y spreading */
  if (lastLinePos == 0) {
    Cell &lastCell = *(cellsSortedByDir[cellsSortedByDir.size() - 1]);
    lastLinePos = CellSpreadGetCellPosition(lastCell, dir);
  }

  /* Return if the last line position has already hit the 
     upper limit */
  maxx = 119427;
  maxy = 119427;

  newLinePos = lastLinePos + alpha * windowWidth;
  if (dir == XDIRECTION && (newLinePos > maxx)) newLinePos = maxx;
  if (dir == YDIRECTION && (newLinePos > maxy)) newLinePos = maxy;
  
  /* Iterate over all the cells in the sorted vector */
  for (idx = cellsSortedByDir.size(); idx > 0; idx--) {
    Cell &thisCell = *(cellsSortedByDir[idx - 1]);
    oldPos = CellSpreadGetCellPosition(thisCell, dir);

    /* Weight can be added to selectively scale down the 
       movement of the new cell */
    newPos = dtoi(weight * (double)newLinePos/lastLinePos * oldPos);
    CellSpreadSetCellPosition(thisCell, newPos, dir);
  }
  for (idx = cellsSortedByDir.size(); idx > 0; idx--) {
    Cell &thisCell = *(cellsSortedByDir[idx - 1]);
    CellSpreadCreatePseudoPort(myDesign, myGraph, thisCell, 
			       maxx, maxy, dir);
  }    
  
  lastLinePos = newLinePos;
}
