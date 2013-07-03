# include <Legalize.h>
# include <Pin.h>

bool debugM = false;

bool
compareCostFunc(const pair<Cell *, double> &cell1, const pair<Cell *, double> &cell2) 
{
  return (cell1.second < cell2.second);
}

double
LegalizeComputeMoveCostOld(Cell &thisCell, double oldXpos, double oldYpos, 
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

  celliXPos = newXpos;
  celliYPos = newYpos;
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
LegalizeComputeMoveCost(Cell &thisCell, double oldXpos, double oldYpos, 
			double newXpos, double newYpos)
{
  double alphax, alphay;
  double totalCost;

  alphax = 1.0;
  alphay = 1.0;
  
  totalCost = alphax * fabs(newXpos - oldXpos) + alphay * fabs(newYpos - oldYpos) * 4.0;
  //totalCost = sqrt((alphax * (fabs(newXpos - oldXpos)) * (fabs(newXpos - oldXpos))) + (alphay * (fabs(newYpos - oldYpos)) * (fabs(newYpos - oldYpos))));

  return (totalCost);
}

double 
LegalizeGetCellsToMove(LegalizeBin &srcBin, LegalizeBin &destBin,
		       map<Cell *, double> &fractionalCells, bool sameZone,
		       map<Cell *, double> &cellsToMove, double &totalFracWidth)
{
  vector<pair<Cell*, double> > cellCostPair;
  pair<Cell *, double> thisPair;
  double srcSupply, cellFrac;
  double cellCost, totalCost;
  double destX, destY;
  uint cellWidth;
  Cell *binCell;
  
  PhysRow *thisRow;
  PhysRow *adjRow;
  int binIdx, rowIdx, destIdx, destRowIdx;
  binIdx = srcBin.BinGetIndex();
  thisRow = srcBin.BinGetRow();
  rowIdx = (*thisRow).PhysRowGetIndex();
  
  adjRow = destBin.BinGetRow();
  destIdx = destBin.BinGetIndex();
  destRowIdx = (*adjRow).PhysRowGetIndex();
  //  srcSupply = (double)srcBin.BinGetSupply();
  srcSupply = LegalizeBinGetBValue(&srcBin);
  destX = (double)destBin.BinGetBegin();
  destY = (double)destBin.BinGetBot();
  
  //  cout << " SRC B-Val " << srcSupply << endl;
  /* Compute the cost for all cells */
  MAP_FOR_ALL_ELEMS(fractionalCells, Cell *, double, binCell, cellFrac) {
    cellCost = 
      LegalizeComputeMoveCost(*binCell, binCell->CellGetXpos(), 
			      binCell->CellGetYpos(),
			      destX, destY);
    //    cout << "CELL " << (binCell->CellGetName()) << " COST TO MOVE FROM (" << (binCell->CellGetXpos()) << "," 
    //	 << (binCell->CellGetYpos()) << ") TO (" << destX << "," << destY << ") = "
    //	 << cellCost << endl; 
    
    cellCostPair.push_back(make_pair(binCell, cellCost));
  } END_FOR;
  

  
  /* Sort the cells on the basis of cost */
  sort(cellCostPair.begin(), cellCostPair.end(), compareCostFunc);
  
    
  /* Pick the top k cells including their fractional width */
  totalCost = 0.0;
  totalFracWidth = 0.0;
  cellsToMove.clear();
  VECTOR_FOR_ALL_ELEMS(cellCostPair, pair<Cell * MCOMMA double>, thisPair) {
    Cell &thisCell = *(thisPair.first);
    cellFrac = fractionalCells[&thisCell];
    if (!sameZone && cellFrac < 1.0) {
      continue;
    }
    cellWidth = thisCell.CellGetWidth();
    cellsToMove.insert(make_pair(&thisCell, cellFrac));
    totalFracWidth += cellFrac * cellWidth;
    srcSupply -= (cellFrac * cellWidth);
    totalCost += thisPair.second;
    //cout << " TOTAL FRACTWIDTH = " << totalFractWidth << " TOTAL COST " << totalCost << endl;
    if (srcSupply <= 0) {
      break;
    }
    /*
    if ((srcSupply - (cellFrac * cellWidth)) >= 0) {
      cellsToMove.insert(make_pair(&thisCell, cellFrac));
      totalFracWidth += cellFrac * cellWidth;
      srcSupply -= cellFrac * cellWidth;
      totalCost += thisPair.second;
    } else {
      break;
      }*/
  } END_FOR;
  
  return (totalCost);
}

