# include <Legalize.h>
# include <Pin.h>

bool
compareCostFunc(pair<Cell *, double> cell1, pair<Cell *, double> cell2) 
{
  return (cell1.second < cell2.second);
}

double
LegalizeComputeMoveCost(Cell &thisCell, double oldXpos, double oldYpos, 
			double newXpos, double newYpos)
{
  Pin *pini, *pinj;
  double celliXPos, celliYPos;
  double celljXPos, celljYPos;
  double piniXOffset, piniYOffset;
  double pinjXOffset, pinjYOffset;
  double piniAbsX, piniAbsY;
  double pinjAbsX, pinjAbsY;
  double oldForceX, oldForceY, totalOld;
  double newForceX, newForceY, totalNew;
  double netWeight;

  /* Compute the cost added to the quadratic function 
     if the cell were to be moved to a new location */
  celliXPos = oldXpos;
  celliYPos = oldYpos;
  oldForceX = 0.0; oldForceY = 0.0;
  CELL_FOR_ALL_PINS(thisCell, PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    netWeight = connectedNet.NetGetWeight();
    piniXOffset = (*pini).PinGetXOffset();
    piniYOffset = (*pini).PinGetYOffset();
    piniAbsX = piniXOffset + celliXPos;
    piniAbsY = piniYOffset + celliYPos;
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      if (pini == pinj) continue;
      Cell &connectedCell = (*pinj).PinGetParentCell();
      if (&connectedCell == &thisCell) continue;
      celljXPos = connectedCell.CellGetXpos();
      celljYPos = connectedCell.CellGetYpos();
      pinjXOffset = (*pinj).PinGetXOffset();
      pinjYOffset = (*pinj).PinGetYOffset();
      pinjAbsX = celljXPos + pinjXOffset;
      pinjAbsY = celljYPos + pinjYOffset;
      oldForceX += pinjAbsX - piniAbsX;
      oldForceY += pinjAbsY - piniAbsY;
    } NET_END_FOR;
  } CELL_END_FOR;

  newForceX = 0.0; newForceY = 0.0;
  CELL_FOR_ALL_PINS(thisCell, PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    netWeight = connectedNet.NetGetWeight();
    piniXOffset = (*pini).PinGetXOffset();
    piniYOffset = (*pini).PinGetYOffset();
    piniAbsX = piniXOffset + celliXPos;
    piniAbsY = piniYOffset + celliYPos;
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      if (pini == pinj) continue;
      Cell &connectedCell = (*pinj).PinGetParentCell();
      if (&connectedCell == &thisCell) continue;
      celljXPos = connectedCell.CellGetXpos();
      celljYPos = connectedCell.CellGetYpos();
      pinjXOffset = (*pinj).PinGetXOffset();
      pinjYOffset = (*pinj).PinGetYOffset();
      pinjAbsX = celljXPos + pinjXOffset;
      pinjAbsY = celljYPos + pinjYOffset;
      newForceX += pinjAbsX - piniAbsX;
      newForceY += pinjAbsY - piniAbsY;
    } NET_END_FOR;
  } CELL_END_FOR;
  
  totalOld = sqrt((oldForceX * oldForceX) + (oldForceY * oldForceY));
  totalNew = sqrt((newForceX * newForceX) + (newForceY * newForceY));
  
  return (totalNew - totalOld);
}

double 
LegalizeGetCellsToMove(LegalizeBin &srcBin, LegalizeBin &destBin,
		       vector<Cell *> &binCells, bool sameZone,
		       vector<Cell *> &cellsToMove)
{
  vector<pair<Cell*, double> > cellCostPair;
  pair<Cell *, double> thisPair;
  double srcSupply, cellFrac;
  double cellCost, totalCost;
  double destX, destY;
  uint cellWidth;
  Cell *binCell;
  
  srcSupply = (double)srcBin.BinGetSupply();
  destX = (double)destBin.BinGetBegin();
  destY = (double)destBin.BinGetBot();
  
  /* Compute the cost for all cells */
  VECTOR_FOR_ALL_ELEMS(binCells, Cell *, binCell) {
    cellCost = 
      LegalizeComputeMoveCost(*binCell, binCell->CellGetXpos(), 
			      binCell->CellGetYpos(),
			      destX, destY);
    cellCostPair.push_back(make_pair(binCell, cellCost));
  } END_FOR;
  
  /* Sort the cells on the basis of cost */
  sort(cellCostPair.begin(), cellCostPair.end(), compareCostFunc);
  
  /* Pick the top k cells including their fractional width */
  totalCost = 0.0;
  VECTOR_FOR_ALL_ELEMS(cellCostPair, pair<Cell * MCOMMA double>, thisPair) {
    Cell &thisCell = *(thisPair.first);
    cellFrac = srcBin.BinGetCellFract(&thisCell);
    if (!sameZone && cellFrac < 1.0) {
      continue;
    }
    cellWidth = thisCell.CellGetWidth();
    if ((srcSupply - (cellFrac * cellWidth)) >= 0) {
      cellsToMove.push_back(&thisCell);
      srcSupply -= cellFrac * cellWidth;
      totalCost += thisPair.second;
    } else {
      break;
    }
  } END_FOR;
  
  return (totalCost);
}
