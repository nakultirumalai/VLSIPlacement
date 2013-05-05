# include <Design.h>

/* Define the sorting function */
bool cmpCellXpos(Cell *cellPtri, Cell *cellPtrj) 
{ 
  return ((*cellPtri).CellGetXpos() < (*cellPtrj).CellGetXpos());
}
/* Define the sorting function */
bool cmpCellYpos(Cell *cellPtri, Cell *cellPtrj) 
{ 
  return ((*cellPtri).CellGetYpos() < (*cellPtrj).CellGetYpos());
}

void 
Design::DesignSolveForAllCells(allSolverType solverType)
{
  vector<Cell *> inputCells;
  vector <Cell *> cellsSortedByX;
  vector <Cell *> cellsSortedByY;
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
    //    cout << "Cell name: " << (*(Cell *)cellObj).CellGetName() << endl;    
    inputCells.push_back((Cell *)cellObj);
    cellsSortedByX.push_back((Cell *)cellObj);
    cellsSortedByY.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;
  
  double alphaX, alphaY;
  alphaX = 0.01; alphaY = 0.01;
  /* Execute the solver to obtain a result */
  genSolve((*this), myGraph, inputCells);
  
  newLineXpos = 0;
  while (1) {
    if (newLineXpos > 118000) {
      break;
    }
    sort(cellsSortedByX.begin(), cellsSortedByX.end(), cmpCellXpos);
    CellSpreadInDesign((*this), myGraph, cellsSortedByX, alphaX,
		       windowWidth, XDIRECTION, newLineXpos);
    cout << "Alpha X: " << alphaX << "  Stretch line position: " << newLineXpos << endl;
    if (alphaX < 1.0) alphaX += 0.05;
    if (alphaX > 1.0) {
      alphaX = 1.0;
    } 
    /* Execute the solver to obtain a result */
    genSolve((*this), myGraph, inputCells);
  }
  /* After all iterations, remove pseudo pins, pseudo ports and
     pseudo nets */
}

