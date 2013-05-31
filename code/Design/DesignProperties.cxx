# include <Design.h>

void
printCellDiffDetails(Cell &thisCell)
{
  Cell *thisCellPtr;
  Pin *pini, *pinj;
  double celliXPos, celliYPos;
  double celljXPos, celljYPos;
  double piniXOffset, piniYOffset;
  double pinjXOffset, pinjYOffset;
  double diffCellXPos, diffCellYPos;
  double diffCellXPosCurPair, diffCellYPosCurPair;
  double diffPinXPosCurPair, diffPinYPosCurPair;
  double diffPinXOffPos, diffPinYOffPos;
  double totalXDiffDist, totalYDiffDist;
  double netWeight;

  CELL_FOR_ALL_PINS_NOFILT(thisCell, PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    netWeight = connectedNet.NetGetWeight();
    piniXOffset = (*pini).PinGetXOffset();
    piniYOffset = (*pini).PinGetYOffset();
    celliXPos = CellGetDblX(&thisCell);
    celliYPos = CellGetDblY(&thisCell);
    cout << "ciX\tpix\tcjX\tpjX\tciY\tpiY\tcjY\tpjY\tNWeig.\tdiffX\tdiffY" << endl;
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      if (pini == pinj) continue;
      Cell &connectedCell = (*pinj).PinGetParentCell();
      if (&connectedCell == &thisCell) continue;
      pinjXOffset = (*pinj).PinGetXOffset();
      pinjYOffset = (*pinj).PinGetYOffset();
      diffCellXPos -= netWeight * celliXPos;
      diffCellXPos -= netWeight * piniXOffset;
      diffCellYPos -= netWeight * celliYPos;
      diffCellYPos -= netWeight * piniYOffset;
      if (!connectedCell.CellIsTerminal()) {
	celljXPos = CellGetDblX(&connectedCell);
	celljYPos = CellGetDblY(&connectedCell);
      } else {
	cout << "PORT" << endl;
	celljXPos = connectedCell.CellGetXpos();
	celljYPos = connectedCell.CellGetYpos();
      }
      diffCellXPos += netWeight * (celljXPos + pinjXOffset);
      diffCellYPos += netWeight * (celljYPos + pinjYOffset);
      cout << celliXPos << "\t" << piniXOffset << "\t" 
	   << celljXPos << "\t" << pinjXOffset << "\t"
	   << celliYPos << "\t" << piniYOffset << "\t" 
	   << celljYPos << "\t" << pinjYOffset << "\t"
	   << netWeight << "\t"
	   << (netWeight * (celljXPos + pinjXOffset - (celliXPos + piniXOffset))) << "\t"
	   << (netWeight * (celljYPos + pinjYOffset - (celliYPos + piniYOffset))) << "\t"
	   << endl;
    } NET_END_FOR;
  } CELL_END_FOR;
}

bool
Design::DesignCheckSolvedCellsProperty(vector<Cell*> solvedCells)
{
  Cell *thisCellPtr;
  Pin *pini, *pinj;
  double celliXPos, celliYPos;
  double celljXPos, celljYPos;
  double piniXOffset, piniYOffset;
  double pinjXOffset, pinjYOffset;
  double diffCellXPos, diffCellYPos;
  double diffCellXPosCurPair, diffCellYPosCurPair;
  double diffPinXPosCurPair, diffPinYPosCurPair;
  double diffPinXOffPos, diffPinYOffPos;
  double totalXDiffDist, totalYDiffDist;
  double netWeight;
  bool rtv = true;

  VECTOR_FOR_ALL_ELEMS(solvedCells, Cell*, thisCellPtr) {
    Cell &thisCell = *thisCellPtr;
    diffCellXPos = 0.0; diffCellYPos = 0.0;
    diffPinXOffPos = 0.0; diffPinYOffPos = 0.0;
    if (debugPrint)
    cout << "Checking cell \"" << thisCell.CellGetName() 
	 << "(" << thisCell.CellGetOrigName() << ", " 
	 << thisCell.CellGetLibCellName() << ")\"..." << endl;
    CELL_FOR_ALL_PINS_NOFILT(thisCell, PIN_DIR_ALL, pini) {
      Net &connectedNet = (*pini).PinGetNet();
      netWeight = connectedNet.NetGetWeight();
      piniXOffset = (*pini).PinGetXOffset();
      piniYOffset = (*pini).PinGetYOffset();
      celliXPos = CellGetDblX(&thisCell);
      celliYPos = CellGetDblY(&thisCell);
      if (0 && debugPrint) {
	cout << "ciX\tpix\tcjX\tpjX\tciY\tpiY\tcjY\tpjY\tNWeig.\tdiffX\tdiffY" << endl;
      }
      NET_FOR_ALL_PINS(connectedNet, pinj) {
	if (pini == pinj) continue;
	Cell &connectedCell = (*pinj).PinGetParentCell();
	if (&connectedCell == &thisCell) continue;
	pinjXOffset = (*pinj).PinGetXOffset();
	pinjYOffset = (*pinj).PinGetYOffset();
	diffCellXPos -= netWeight * celliXPos;
	diffCellXPos -= netWeight * piniXOffset;
	diffCellYPos -= netWeight * celliYPos;
	diffCellYPos -= netWeight * piniYOffset;
	if (!connectedCell.CellIsTerminal()) {
	  celljXPos = CellGetDblX(&connectedCell);
	  celljYPos = CellGetDblY(&connectedCell);
	} else {
	  if (debugPrint && 0) cout << "PORT" << endl;
	  celljXPos = connectedCell.CellGetXpos();
	  celljYPos = connectedCell.CellGetYpos();
	}
	diffCellXPos += netWeight * (celljXPos + pinjXOffset);
	diffCellYPos += netWeight * (celljYPos + pinjYOffset);
	if (debugPrint && 0) {
	  cout << celliXPos << "\t" << piniXOffset << "\t" 
	       << celljXPos << "\t" << pinjXOffset << "\t"
	       << celliYPos << "\t" << piniYOffset << "\t" 
	       << celljYPos << "\t" << pinjYOffset << "\t"
	       << netWeight << "\t"
	       << (netWeight * (celljXPos + pinjXOffset - (celliXPos + piniXOffset))) << "\t"
	       << (netWeight * (celljYPos + pinjYOffset - (celliYPos + piniYOffset))) << "\t"
	       << endl;
	}
      } NET_END_FOR;
    } CELL_END_FOR;
    if (dtoi(diffCellXPos) == 0) {
      if (debugPrint) cout << " MATCHES in X..." << endl;
    } else {
      cout << " For CELL: " << thisCell.CellGetName() << "(" 
	   << thisCell.CellGetOrigName() << ", " << thisCell.CellGetLibCellName() << ")\"  "
	   << "Failed to match in X, Difference: Integer: " << dtoi(diffCellXPos) 
	   << "  Double:" << diffCellXPos;
      cout << endl;
      printCellDiffDetails(thisCell);
      rtv = false;
    }
    if (dtoi(diffCellYPos) == 0) {
      if (debugPrint) cout << " MATCHES in Y..." << endl;
    } else {
      cout << " For CELL: " << thisCell.CellGetName() << "(" 
	   << thisCell.CellGetOrigName() << ", " << thisCell.CellGetLibCellName() << ")\"  "
	   << " Failed to match in Y, Difference: Integer: " << dtoi(diffCellYPos)
	   << "  Double:" << diffCellYPos;
      cout << endl;
      rtv = false;
    }
  } END_FOR;

  return rtv;
}
