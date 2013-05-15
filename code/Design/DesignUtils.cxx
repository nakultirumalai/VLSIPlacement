# include <Design.h>

using namespace std;

double thetaFunc(uint L1, uint R1, uint L2, uint R2)
{
  uint left, right;
  double overlap;

  right = R2;
  if (R1 < R2) right = R1;
  left = L2;
  if (L1 > L2) left = L1;

  overlap = (int)(right - left);
  if (overlap < 0) overlap = 0.0;

  return (overlap);
}

void
Design::DesignProcessProperty(ifstream& file, string& Property, 
			      string& Value)
{
  string line;
  unsigned int numNodes, numTerms;
  int idx1, idx2, idx3;
  bool result;

  if (!file.eof()) getline(file, line);

  while (line == "" && !file.eof()) 
    getline(file, line);

  idx1 = 0;
  while (line[idx1] == ' ' || line[idx1] == '\t') 
    idx1++;

  idx2 = idx1;
  while (line[idx2] != ' ' && line[idx2] != '\t') 
    idx2++;

  idx3 = idx2;
  while (!(line[idx3] >= ZERO && line[idx3] <= NINE)) 
    idx3++;
  
  Property = line.substr(idx1, (idx2 - idx1));
  Value = line.substr(idx3);
}


void
Design::DesignFileReadHeader(ifstream& file)
{
  string buffer;
  int i;

  i = 0;
  while (!file.eof() && i < 4) {
    getline(file, buffer);
    i++;
  }
}

bool
DesignCellIsStdCell(Design &myDesign, Cell &thisCell)
{
  map<unsigned int, unsigned int>rowHeights;
  int rowHeight, cellHeight;
  bool stdCell;

  rowHeights = myDesign.DesignGetRowHeights();
  rowHeight = myDesign.DesignGetSingleRowHeight();
  cellHeight = thisCell.CellGetHeight();
  stdCell = false;

  if (rowHeight == -1) {
    if (rowHeights.find(cellHeight) != rowHeights.end()) {
      stdCell = true;
    }
  } else if (cellHeight == rowHeight) {
    stdCell = true;
  }

  return (stdCell);
}

vector<Cell *>
Design::DesignGetCellsOfRegion(uint left, uint right, uint bot, uint top,
			       vector<Cell *> &cellsSortedByLeft,
			       vector<Cell *> &cellsSortedByBot,
			       double &overLapArea, 
			       double &totalCellWidth)
{
  uint last, sortedListsSize, lastPos;
  uint cellLeftPos, cellRightPos, cellTopPos, cellBotPos;
  uint idxLeft, idxRight, idxBot, idxTop;
  uint cellLeftXpos, cellRightXpos;
  uint cellTopYpos, cellBotYpos;
  double thisCellOverlapArea;
  bool noCellFound;
  vector<Cell *> listOfCells;

  overLapArea = 0.0;
  totalCellWidth = 0.0;
  /* Compute the overlap here after checking the y-positions of the cells in a 
     detailed manner */
  for (int i = 0; i < cellsSortedByLeft.size(); i++) {
    Cell &thisCell = *(cellsSortedByLeft[i]);
    cellLeftPos = thisCell.CellGetXpos();
    cellRightPos = thisCell.CellGetRight();
    cellBotPos = thisCell.CellGetYpos();
    cellTopPos = thisCell.CellGetTop();

    thisCellOverlapArea = thetaFunc(left, right, cellLeftPos, cellRightPos) *
      thetaFunc(bot, top, cellBotPos, cellTopPos);

    if (thisCellOverlapArea != 0.0) {
      if ((cellLeftPos >= left && cellLeftPos < right) &&
          (cellBotPos >= bot && cellBotPos < top)) {
        listOfCells.push_back(&thisCell);
	totalCellWidth += thisCell.CellGetWidth();
      }
      overLapArea += thisCellOverlapArea;
    }
  }

  return (listOfCells);
}

void
Design::DesignPlotData(string plotTitle, string plotFileName)
{
  vector<Cell *> cellsToSolve;
  Bin *binPtr;
  uint binIdx;

  Plot newPlot(plotTitle, plotFileName);
  newPlot.PlotSetBoundary(*this);
  cellsToSolve = (*this).DesignGetCellsToSolve();
  
  newPlot.PlotAddCells(cellsToSolve);

  DESIGN_FOR_ALL_BINS((*this), binIdx, binPtr) {
    newPlot.PlotAddBin(*binPtr);
  } DESIGN_END_FOR;

  newPlot.PlotWriteOutput();
}

void
Design::DesignClearPseudoNetWeights(void)
{
  Net *pseudoNetPtr;
  uint idx;
  uint numPseudoNets;
  
  numPseudoNets = PseudoNets.size();
  for (idx = 0; idx < numPseudoNets; idx++) {
    pseudoNetPtr = PseudoNets[idx];
    (*pseudoNetPtr).NetSetWeight(0.0);
  }
}
