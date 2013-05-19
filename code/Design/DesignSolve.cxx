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
  uint lastLineXpos, lastLineYpos;
  uint newLineXpos, newLineYpos;
  uint itrNum, nodeIdx, idx;

  /* window width is the width of the window that moves
     right in nanometers */
  uint windowHeight, windowWidth;
  uint maxx, maxy;
  (*this).DesignGetBoundingBox(maxx, maxy);
  windowHeight = 4000;
  windowWidth = 8000;

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
  string plotFileName;

  (*this).DesignSetCellsToSolve(cellsToSolve);
  /* Invoke solver */
  genSolveX((*this), myGraph, inputCells);
  genSolveY((*this), myGraph, inputCells);
  /* Check force on cells */
  if (!(*this).DesignCheckSolvedCellsProperty(cellsToSolve)) {
    cout << "Cells are not matched" << endl;
  }
  double prevPeakUtil;
  double threshold;
  prevPeakUtil = 0.0;
  threshold = 1.0;
  while (1) {
    (*this).DesignCreateBins(windowWidth,windowHeight);
    peakUtilization = (*this).DesignGetPeakUtil();
    if (prevPeakUtil == 0.0) {
      prevPeakUtil = peakUtilization;
    }
    binIdx = (*this).DesignGetPeakUtilBinIdx();
    cout << "Iteration: " << count << " Peak Utilization: " << peakUtilization 
	 << " Bin index: " << binIdx << endl;
    if (count == 2) break;
    /* Add spreading forces */
    CellSpreadInDesignFastPlace((*this), myGraph);

    /* Plot the stuff in the design */
    plotFileName = "Itr" + getStrFromInt(count) + ".plt";
    (*this).DesignPlotData("Title", plotFileName);

    /* Clear bins */
    (*this).DesignClearBins();

    /* Invoke solver */
    genSolveX((*this), myGraph, inputCells);
    genSolveY((*this), myGraph, inputCells);
    (*this).DesignClearPseudoNetWeights();

    count++;
    if ((prevPeakUtil > peakUtilization) &&
	((prevPeakUtil - peakUtilization) < threshold)) {
      break;
    }
    prevPeakUtil = peakUtilization;
  }
  /*
  /* Check force on cells 
  if (!(*this).DesignCheckSolvedCellsProperty(cellsToSolve)) {
    cout << "Cells are not matched" << endl;
  }
  */
}
