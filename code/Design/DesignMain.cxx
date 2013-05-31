# include <Design.h>

using namespace std;

bool cmpCellLeftPos(Cell *cellPtri, Cell *cellPtrj)
{
  uint lefti = (*cellPtri).CellGetXpos();
  uint leftj = (*cellPtrj).CellGetXpos();
  return (lefti < leftj);
}

bool cmpCellBotPos(Cell *cellPtri, Cell *cellPtrj)
{
  uint boti = (*cellPtri).CellGetYpos();
  uint botj = (*cellPtrj).CellGetYpos();
  return (boti < botj);
}

map<string,Net*>& Design::DesignGetNets(void)
{
  map<string, Net*>& retVal = this->DesignNets;

  return (retVal);
}


map<string,Cell*>& Design::DesignGetCells(void)
{
  map<string, Cell*>& retVal = this->DesignCells;

  return (retVal);
}

vector<PhysRow*>& Design::DesignGetRows(void)
{
  vector<PhysRow*>& retVal = this->DesignPhysRows;

  return (retVal);
}

vector<Bin*>& Design::DesignGetBins(void)
{
  vector<Bin*>& retVal = this->DesignBins;

  return (retVal);
}


Cell *
Design::DesignGetNode(string nodeName)
{
  Cell *foundNode;
  
  foundNode = DesignCells[nodeName];
  
  return (foundNode);
}

string
Design::DesignGetName(void)
{
  return (this->Name);
}

map<unsigned int, unsigned int> 
Design::DesignGetRowHeights()
{
  unsigned int height;
  map<unsigned int, unsigned int> retVal;
  
  MAP_FOR_ALL_KEYS(RowHeights, unsigned int, unsigned int, height) {
    retVal[height] = 1;
  } END_FOR;
  
  return retVal;
}

void
Design::DesignAddOneCellToDesignDB(Cell *newCell)
{
  if (!DesignCellIsStdCell((*this),*newCell)) {
    (*newCell).CellSetIsMacro(true);
    NumMacroCells++;
  } else {
    this->averageStdCellWidth += (*newCell).CellGetWidth();
    this->averageStdCellHeight += (*newCell).CellGetHeight();
    NumStdCells++;
  }
  DesignCells[(*newCell).CellGetName()] = newCell;
  this->NumCells++;
}

void
Design::DesignAddOneNetToDesignDB(Net *newNet, double Weight)
{
  double actualWeight;
  
  actualWeight = Weight / ((*newNet).NetGetPinCount() - 1);
  (*newNet).NetSetWeight(actualWeight);
  DesignNets[(*newNet).NetGetName()] = newNet;
  this->NumNets++;
}

void
Design::DesignAddPseudoNet(Net *pseudoNet)
{
  PseudoNets.push_back(pseudoNet);
}

void
Design::DesignAddOnePhysRowToDesignDB(PhysRow *row)
{
  unsigned int RowHeight;
  
  RowHeight = (*row).PhysRowGetHeight();

  DesignPhysRows.push_back(row);
  DesignUpdateChipDim(row);

  if (RowHeights.find(RowHeight) == RowHeights.end()) {
    RowHeights[RowHeight] = 1;
    if (singleRowHeight == -1) {
      singleRowHeight = RowHeight;
    } else {
      singleRowHeight = -1;
    }
  } 
  
  this->NumPhysRows++;
}

void
Design::DesignSetPeakUtil(double maxUtil)
{
  this->peakUtilization = maxUtil;
}

void
Design::DesignSetPeakUtilBinIdx(uint peakUtilBinIdx)
{
  this->peakUtilizationBinIdx = peakUtilBinIdx;
}

void
Design::DesignSetNumBinRows(uint numRows)
{
  this->numBinRows = numRows;
}

void
Design::DesignSetNumBinCols(uint numCols)
{
  this->numBinCols = numCols;
}

void 
Design::DesignCreateBins(void)
{
  uint binHeight, binWidth;
  
  binHeight = this->averageStdCellHeight;
  binWidth = this->averageStdCellWidth;
  
  /* Configure bin to accommodate four cells */
  /* SQUARE BIN */
  //  binHeight *= 2; binWidth *= 2;
  /* WIDE BIN */
  //  binWidth*= 4; 
  /* TALL BIN */
  // binHeight = 4 * binHeight; */
  /* LARGER SQUARE BIN */
  // binHeight *= 3; binWidth *= 3; */
  /* LARGER WIDE BIN */
  binHeight *= 2; binWidth *= 4; 
  /* LARGER TALL BIN */
  //  binHeight *=3; binWidth *= 2;
  DesignCreateBins(binHeight, binWidth);
}

void
Design::DesignCreateBins(uint binHeight, uint binWidth)
{
  Bin *binPtr;
  vector<Cell*> cellsOfBin;
  vector<Cell*> cellsSortedByLeft;
  vector<Cell*> cellsSortedByBot;
  double maxUtilization, overlapArea;
  double totalCellWidth, averageCellWidth;
  double utilization;
  uint maxx, maxy;
  uint binCount, numRows, numCols;
  uint left, right, bot, top;
  uint peakUtilBinIdx;
  uint i, j;
  uint numBins, numCells;
  bool createBins;

  _STEP_BEGIN("Bin construction");
  DesignGetBoundingBox(maxx, maxy);
  cellsSortedByLeft = DesignGetCellsSortedByLeft();
  cellsSortedByBot = DesignGetCellsSortedByBot();
  numCols = (uint)ceil(((double)maxx) / binWidth);
  numRows = (uint)ceil(((double)maxy) / binHeight);

  binCount = 0; 
  maxUtilization = 0; 
  peakUtilBinIdx = 0;
  numBins = DesignBins.size();
  createBins = false;
  if (numBins == 0) createBins = true;
  bot = 0; top = binHeight;
  //  cout << "Creating " << numCols * numRows << " bins" << endl;
  for (i = 0; i < numRows; i++) {
    left = 0; right = binWidth;
    for (j = 0; j < numCols; j++) {
      if (right > maxx) right = maxx;
      if (top > maxy) top = maxy;
      if (createBins) {
	binPtr = new Bin(binCount, left, right, bot, top);
	DesignBins.push_back(binPtr);
      } else {
	binPtr = DesignBins[binCount];
      }
      (*binPtr).BinSetNewRight(right);
      (*binPtr).BinSetNewTop(top);
      cellsOfBin =
        DesignGetCellsOfBin(binPtr, left, right, bot, top, cellsSortedByLeft,
			    cellsSortedByBot, overlapArea, totalCellWidth);
      utilization = overlapArea / (((double)binHeight) * binWidth);
      numCells = cellsOfBin.size();
      averageCellWidth = 0.0;
      if (numCells > 0) {
	averageCellWidth = totalCellWidth / numCells;
      }
      if (maxUtilization < utilization) {
        maxUtilization = utilization;
        peakUtilBinIdx = binCount;
      }
      (*binPtr).BinSetCells(cellsOfBin);
      (*binPtr).BinSetCellArea(overlapArea);
      (*binPtr).BinSetUtilization(utilization);
      (*binPtr).BinSetAverageCellWidth(averageCellWidth);
      left += binWidth;
      right += binWidth;
      binCount++;
    }
    bot += binHeight;
    top += binHeight;
  }
  DesignSetPeakUtil(maxUtilization);
  DesignSetPeakUtilBinIdx(peakUtilBinIdx);
  DesignSetNumBinRows(numRows);
  DesignSetNumBinCols(numCols);

  _STEP_END("Bin construction");
}

void
Design::DesignClearBins(void)
{
  Bin *binPtr;

  VECTOR_FOR_ALL_ELEMS(DesignBins, Bin*, binPtr) {
    (*binPtr).BinDeleteData();
  } END_FOR;
}

double
Design::DesignGetPeakUtil(void)
{
  return (this->peakUtilization);
}

uint
Design::DesignGetPeakUtilBinIdx(void)
{
  return (this->peakUtilizationBinIdx);
}

vector<Cell *>&
Design::DesignGetCellsToSolve(void)
{
  return cellsToSolve;
}

void
Design::DesignSetCellsToSolve(vector<Cell *> cellsToSolve)
{
  this->cellsToSolve = cellsToSolve;
}

int
Design::DesignGetNextRowBinIdx(uint binIdx)
{
  int rtv;

  rtv = -1;
  if (!((binIdx + 1) % numBinCols == 0)) 
    rtv = binIdx + 1;
  
  return (rtv);
}

int
Design::DesignGetNextColBinIdx(uint binIdx)
{
  int rtv;
  uint nextColBinIdx;
  uint rowOfBin;
  
  rtv = -1;
  rowOfBin = binIdx / numBinCols;
  if (rowOfBin < (numBinRows - 1)) {
    rtv = binIdx + numBinCols;
  }
  
  return (rtv);
}

int
Design::DesignGetPrevRowBinIdx(uint binIdx)
{
  int rtv;

  rtv = -1;
  if (!(binIdx % numBinCols == 0)) 
    rtv = binIdx - 1;
  
  return (rtv);
}

int
Design::DesignGetPrevColBinIdx(uint binIdx)
{
  int rtv;

  rtv = -1;
  if (!(binIdx / numBinCols == 0))
    rtv = binIdx - numBinCols;
  
  return (rtv);
}

vector<Cell *>
Design::DesignGetCellsSortedByLeft(void)
{
  vector<Cell *> rtv;
  
  rtv = DesignGetCellsToSolve();
  sort(rtv.begin(), rtv.end(), cmpCellLeftPos);

  return (rtv);
}

vector<Cell *>
Design::DesignGetCellsSortedByBot(void)
{
  vector<Cell *> rtv;
  
  rtv = DesignGetCellsToSolve();
  sort(rtv.begin(), rtv.end(), cmpCellBotPos);

  return (rtv);
}

void 
Design::DesignAddDelayArc(string libCell, string outputPin, string inputPin, 
			  double arcDelay)
{
  _KEY_EXISTS(libCellDelayDB, libCell) {
    map<string, map<string, double> >& cellDelays = libCellDelayDB[libCell];
    _KEY_EXISTS(cellDelays, outputPin) {
      map<string, double> &arcDelays = cellDelays[outputPin];
      _KEY_EXISTS(arcDelays, inputPin) {
	cout << "Warning: Delay for library cell (" << libCell 
	     << ") arc " << outputPin << " - " << inputPin << " being replaced" << endl;
      }
      arcDelays[inputPin] = arcDelay;
    } else {
      map<string, double> arcDelays;
      arcDelays[inputPin] = arcDelay;
      cellDelays[outputPin] = arcDelays;
    }
  } else {
    map<string, map<string, double > > cellDelays;
    map<string, double> arcDelays;
    arcDelays[inputPin] = arcDelay;
    cellDelays[outputPin] = arcDelays;
    libCellDelayDB[libCell] = cellDelays;
  }
}

double
Design::DesignGetDelayArc(string libCell, string outputPin, string inputPin)
{
  double rtv;

  rtv = -1;
  _KEY_EXISTS(libCellDelayDB, libCell) {
    map<string, map<string, double> >& cellDelays = libCellDelayDB[libCell];
    _KEY_EXISTS(cellDelays, outputPin) {
      map<string, double> &arcDelays = cellDelays[outputPin];
      _KEY_EXISTS(arcDelays, inputPin) {
	rtv = arcDelays[inputPin];
      }
    }
  }
  
  return (rtv);
}

void
Design::DesignAddCellToPhysRow(Cell* cell, vector<vector<int> > &allRowBounds, 
			       vector<PhysRow*> &allPhysRows)
{
  /* Getting cell bounds */

  int cellX = cell->CellGetXpos();
  int cellY = cell->CellGetYpos();
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();
  
  vector<int> Obj;
  int rowIndex;
  int cellCount;
  rowOrientation rowType = (allPhysRows[0]->PhysRowGetType());
  bool foundPos = false;

  VECTOR_FOR_ALL_ELEMS(allRowBounds, vector<int>, Obj){
    rowIndex = i;
    if (rowType == HORIZONTAL){
      int botX = Obj[0];
      int botY = Obj[1];
      int topX = Obj[2];
      int topY = Obj[3];
      int cellXend = cellX + cellWidth;
      int cellYend = cellY + cellHeight;
      
      if ((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	  (cellXend <= topX) && (cellY == botY) && (cellYend > botY) && 
	  ((cellHeight % (topY - botY)) == 0)){
	
	  (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	  foundPos = true;
	  break;
      }
    } else if (rowType == VERTICAL){
      int botX = Obj[0];
      int botY = Obj[1];
      int topX = Obj[2];
      int topY = Obj[3];
      int cellXend = cellX + cellHeight;
      int cellYend = cellY + cellWidth;
      if ((cellX == botX) && (cellXend > botX) && 
	  ((cellHeight % (topX-botX)) == 0) && (cellY >= botY) &&
	  (cellY < topY) && (cellYend > cellY) && (cellYend <= topY)){
	allPhysRows[rowIndex]->PhysRowAddCellToRow(cell);
	foundPos = true;
	break;
      }
    }
  } END_FOR;
}

void
Design::DesignAddAllCellsToPhysRows(void)
{
  string CellName;
  Cell* CellPtr;
  vector<PhysRow*> allPhysRows = DesignGetRows();
  vector<vector<int> > allRowBounds;
  rowOrientation rowType = (allPhysRows[0]->PhysRowGetType());
  /* Get Bounding boxes for all rows */
  PhysRow* Obj;  
  VECTOR_FOR_ALL_ELEMS (allPhysRows, PhysRow*, Obj){
    vector<int> v;
    Obj->PhysRowGetBoundingBox(v);
    Obj->PhysRowSetIndex(i);
    allRowBounds.push_back(v);
  } END_FOR;
  
  DESIGN_FOR_ALL_CELLS((*this), CellName, CellPtr){
    DesignAddCellToPhysRow(CellPtr, allRowBounds, allPhysRows);
  } DESIGN_END_FOR;
}

void
Design::DesignOpenFile(string FileName)
{
  if (DesignFile.is_open()) {
    DesignFile.close();
  }
  DesignFile.open(FileName.data(), ifstream::in);
}


void
Design::DesignCloseFile(void)
{
  if (DesignFile.is_open()) {
    DesignFile.close();
  }
}

int 
Design::DesignGetSingleRowHeight(void)
{
  return singleRowHeight;
}

unsigned int
Design::DesignGetNumCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumStdCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumStdCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumTopCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumTopCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumNets(void)
{
  unsigned int rtv;
  
  rtv = this->NumNets;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumFixedCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumFixedCells;
  
  return (rtv);
}

unsigned int
Design::DesignGetNumTerminalCells(void)
{
  unsigned int rtv;
  
  rtv = this->NumTerminalCells;
}

unsigned int
Design::DesignGetNumPhysRows(void)
{
  unsigned int rtv;
  
  rtv = this->NumPhysRows;
  
  return (rtv);
}

void
Design::DesignGetBoundingBox(uint &maxx, uint &maxy)
{
  maxx = (*this).maxx;
  maxy = (*this).maxy;
}

void
Design::DesignSetGraph(HyperGraph& thisGraph) 
{
  this->DesignGraphPtr = &thisGraph;
}

HyperGraph&
Design::DesignGetGraph(void)
{
  HyperGraph& rtv = (*(this->DesignGraphPtr));

  return (rtv);
}

double
Design::DesignGetClockPeriod(void)
{
  double rtv;
  
  rtv = clockPeriod;
  
  return (rtv);
}

void
Design::DesignSetClockPeriod(double clkPeriod)
{
  this->clockPeriod = clkPeriod;
}

void
Design::DesignUpdateChipDim(PhysRow *row)
{
  vector<int> boundingBox;
  int &left = this->preShiftLeft;
  int &right = this->preShiftRight;
  int &bot = this->preShiftBot;
  int &top = this->preShiftTop;
  int rowLeft, rowBot, rowRight, rowTop;
  (*row).PhysRowGetBoundingBox(rowLeft, rowBot, rowRight, rowTop);

  if (rowLeft < left) {
    left = rowLeft;
  }
  if (rowRight > right) {
    right = rowRight;
    maxx = right;
  } 
  if (rowBot < bot) {
    bot = rowBot;
  }
  if (rowTop > top) {
    top = rowTop;
    maxy = top;
  } 
}

void
Design::DesignShiftChipToZeroZero(void)
{
  PhysRow *PhysRowPtr;
  Cell *cellPtr;
  string cellName;
  uint RowIdx;
  int rowLeft, rowRight, rowBot, rowTop;
  int cellLeft, cellBot;
  int rowCoordinate, rowBegin;
  uint numSites, siteSpacing;
  rowOrientation orient;

  DESIGN_FOR_ALL_ROWS((*this), RowIdx, PhysRowPtr) {
    orient = (*PhysRowPtr).PhysRowGetType();
    rowCoordinate = (*PhysRowPtr).PhysRowGetCoordinate();
    rowBegin = (*PhysRowPtr).PhysRowGetRowBegin();
    siteSpacing = (*PhysRowPtr).PhysRowGetSiteSpacing();
    numSites = (*PhysRowPtr).PhysRowGetNumSites();
    if (orient == HORIZONTAL) {
      rowCoordinate -= (*this).preShiftBot;
      rowBegin -= (*this).preShiftLeft;
    } else if (orient == VERTICAL) {
      rowCoordinate -= (*this).preShiftLeft;
      rowBegin -= (*this).preShiftBot;
    } else {
      cout << "Error: Unknown row type caught. Exiting!!!" << endl;
      exit(0);
    }
    (*PhysRowPtr).PhysRowSetRowBegin(rowBegin);
    (*PhysRowPtr).PhysRowSetCoordinate(rowCoordinate);
  } DESIGN_END_FOR;
  
  DESIGN_FOR_ALL_CELLS((*this), cellName, cellPtr) {
    cellLeft = (*cellPtr).CellGetXpos();
    cellBot = (*cellPtr).CellGetYpos();
    cellLeft -= preShiftLeft;
    cellBot -= preShiftBot;
    (*cellPtr).CellSetXpos(cellLeft);
    (*cellPtr).CellSetYpos(cellBot);
  } DESIGN_END_FOR;
  
  (*this).maxx -= preShiftLeft;
  (*this).maxy -= preShiftBot;
}

void 
Design::DesignSetVarsPostRead()
{
  double &avgStdCellWidth = this->averageStdCellWidth;
  double &avgStdCellHeight = this->averageStdCellHeight;
  
  avgStdCellWidth /= this->NumStdCells;
  avgStdCellHeight /= this->NumStdCells;
}

double
Design::DesignGetAverageStdCellWidth()
{
  return (this->averageStdCellWidth);
}

double
Design::DesignGetAverageStdCellHeight()
{
  return (this->averageStdCellHeight);
}

void 
Design::DesignSetEnv(Env &thisEnv)
{
  this->DesignEnv = thisEnv;
}

Env& 
Design::DesignGetEnv(void)
{
  return DesignEnv;
}

void
Design::DesignInit()
{
  /* Initialize private variables */
  NumCells = 0;
  NumNets = 0;
  NumPhysRows = 0;
  NumFixedCells = 0;
  NumTerminalCells = 0;
  NumStdCells = 0;
  NumMacroCells = 0;
  maxx = -INT_MAX;
  maxy = -INT_MAX;

  singleRowHeight = -1;
  clockPeriod = 0.0;
  peakUtilization = 0.0;
  peakUtilizationBinIdx = 0;
  
  preShiftLeft = 0;
  preShiftBot = 0;
  preShiftRight = -INT_MAX;
  preShiftTop = -INT_MAX;
  
  Name = "";
  DesignPath = "";
  DesignCellFileName = "";
  DesignNetFileName = "";
  DesignNetWtsFileName = "";
  DesignSclFileName = "";
  DesignPlFileName = "";
  RowBasedPlacement = false;
  
  DesignGraphPtr = NIL(HyperGraph *);
  /* Initialize public variables */
  /* empty for now as there are only maps and vectors */
}

Design::Design() 
{
  DesignInit();
}

Design::Design(string DesignPath, string DesignName)
{
  DesignInit();

  DesignReadDesign(DesignPath, DesignName);
}

/* Calculate WMax for each PhysRow */
/*
  vector<PhysRow*> allPhysRows = DesignGetRows();
  PhysRow* Obj;
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    Obj->PhysRowCalculateWMax();
  }END_FOR;
*/
# if 0
void
Design::DesignAddCellToPhysRow(Cell *cell)
{
  /* Getting cell bounds */

  int cellX = cell->CellGetXpos();
  int cellY = cell->CellGetYpos();
  int cellHeight = cell->CellGetHeight();
  int cellWidth = cell->CellGetWidth();
  
  vector< unsigned int> Obj;
  int rowIndex;
  unsigned int subRowIndex;
  int cellCount;
  rowOrientation rowType = allPhysRows[0]->PhysRowGetType();
  
  VECTOR_FOR_ALL_ELEMS(allRowBounds, vector<unsigned int>, Obj){
    rowIndex = i;
    if (Obj.size() > 4){
      if (rowType == HORIZONTAL){
	int cellXend = cellX + cellWidth;
	int cellYend = cellY + cellHeight;
	
	for (int i = 0; i < Obj.size(); i+=4){
	  
	  int botX = Obj[i];
	  int botY = Obj[i+1];
	  int topX = Obj[i+2];
	  int topY = Obj[i+3];
 
	  /* Cell found in subrow */
	  if((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	     (cellXend <= topX) && (cellY == botY) && (cellYend > botY) && 
	     ((cellHeight % (topY - botY)) == 0)){
	    subRowIndex = (i % 4);
	    //cout<<"subRowIndex="<<subRowIndex<<endl;
	    (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	    //cout<<"Added "<<cellCount++<<"cell"<<endl;
	    break; /* No more checks required */
	  }
	}
      }
      else if(rowType == VERTICAL){
	int cellXend = cellX + cellHeight;
	int cellYend = cellY + cellWidth;
	
	for(int i = 0; i < Obj.size(); i += 4){
	  int botX = Obj[i];
	  int botY = Obj[i+1];
	  int topX = Obj[i+2];
	  int topY = Obj[i+3];
	  
	  if((cellX == botX) && (cellXend > botX) && 
	     ((cellHeight % (topX - botX)) == 0) && (cellY >= botY) && 
	     (cellY < topY) && (cellYend > cellY) && (cellYend <= topY)){
	    subRowIndex = (i % 4);
	    (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	    //cout<<"Added "<<cellCount++<<"cell"<<endl;
	    break;
	  }
	}
      }
    }
    /* Indicates one subRow present in the entire row */
    
    else if ((Obj.size() == 4)){
      if (rowType == HORIZONTAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellWidth;
	int cellYend = cellY + cellHeight;
	
	if ((cellX >= botX) && (cellX < topX) && (cellXend > cellX) && 
	   (cellXend <= topX) && (cellY == botY) && (cellYend > botY) && 
	   ((cellHeight % (topY - botY)) == 0)){
	  
	  //cout<<"Will add cell: "<<cell->CellGetName()<<endl;
	  //cout<<"rowIndex :"<<rowIndex<<endl;
	  //cout<<allPhysRows[rowIndex]->PhysRowGetCoordinate()<<endl;
	  (allPhysRows[rowIndex])->PhysRowAddCellToRow(cell);
	  
	  //cout<<"Added "<<cellCount++<<"cell"<<endl;
	}
      }
      else if (rowType == VERTICAL){
	int botX = Obj[0];
	int botY = Obj[1];
	int topX = Obj[2];
	int topY = Obj[3];
	int cellXend = cellX + cellHeight;
	int cellYend = cellY + cellWidth;
	if ((cellX == botX) && (cellXend > botX) && 
	   ((cellHeight % (topX-botX)) == 0) && (cellY >= botY) &&
	    (cellY < topY) && (cellYend > cellY) && (cellYend <= topY)){
	  allPhysRows[rowIndex]->PhysRowAddCellToRow(cell);
	  //cout<<"Added "<<cellCount++<<"cell"<<endl;
	}
      }
    }
  } END_FOR;
}

# endif
