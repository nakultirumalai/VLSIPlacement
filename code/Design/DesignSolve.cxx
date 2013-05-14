# include <Design.h>

void 
Design::DesignSolveForAllCells(allSolverType solverType)
{
  vector<Cell *> inputCells;
  vector<Cell *> cellsToSolve;
  vector<Cell *> cellsSortedByLeft;
  Cell *cellPtr;
  void *cellObj;
  string cellName;
  double alphax, alphay;
  uint windowWidth;
  uint lastLineXpos, lastLineYpos;
  uint newLineXpos, newLineYpos;

  uint itrNum, nodeIdx, idx;

  /* window width is the width of the window that moves
     right in nanometers */
  windowWidth = 30000;
  /* Create two lists of cells, one is a list sorted by x 
     and the other is a list sorted by y values */

  HyperGraph &myGraph = DesignGetGraph();
  /* Insert cells into a vector in the order of their indices in the 
     hypergraph */ 
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    cellsToSolve.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;
  
  uint count = 0; uint binIdx;
  double peakUtilization;

  (*this).DesignSetCellsToSolve(cellsToSolve);
  /* Invoke solver */
  genSolveX((*this), myGraph, inputCells);
  genSolveY((*this), myGraph, inputCells);
  /* Check force on cells */
  if (!(*this).DesignCheckSolvedCellsProperty(cellsToSolve)) {
    cout << "Cells are not matched" << endl;
  }

  while (1) {
    (*this).DesignCreateBins(6000,6000);
    peakUtilization = (*this).DesignGetPeakUtil();
    binIdx = (*this).DesignGetPeakUtilBinIdx();
    cout << "Iteration: " << count << " Peak Utilization: " << peakUtilization 
	 << " Bin index: " << binIdx << endl;

    if (peakUtilization < 10) break;

    /* Add spreading forces */
    CellSpreadInDesignFastPlace((*this), myGraph);
    
    /* Clear bins */
    (*this).DesignClearBins();

    /* Invoke solver */
    genSolveX((*this), myGraph, inputCells);
    genSolveY((*this), myGraph, inputCells);
    count++;
  }
  
  /* Check force on cells */
  if (!(*this).DesignCheckSolvedCellsProperty(cellsToSolve)) {
    cout << "Cells are not matched" << endl;
  }
}
