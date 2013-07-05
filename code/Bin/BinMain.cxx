# include <CellSpread.h>

Bin::~Bin()
{
}

Bin::Bin()
{
  binIdx = 0;
  left = 0;
  right = 0;
  bot = 0;
  top = 0;
  cellArea = 0.0;
  binArea = 0.0;
  utilization = 0.0;
  newRight = 0;
  newTop = 0;
  totalCellWidth = 0;
  averageCellWidth = 0;
  stretchedInX = false;
  stretchedInY = false;
}

Bin::Bin(uint binIdx, uint left, uint right, uint bot, uint top)
{
  this->binIdx = binIdx;
  this->left = left;
  this->right = right;
  this->bot = bot;
  this->top = top;
  cellArea = 0.0;
  binArea = (right - left) * (top - bot);
  utilization = 0.0;
  newRight = 0;
  newTop = 0;
  averageCellWidth = 0;
  stretchedInX = false;
  stretchedInY = false;
}

Bin::Bin(uint binIdx, uint left, uint right, uint bot, uint top, 
	 vector<Cell *> &cellsOfBin)
{
  this->binIdx = binIdx;
  this->left = left;
  this->right = right;
  this->bot = bot;
  this->top = top;
  this->cellsOfBin = cellsOfBin;
  cellArea = 0.0;
  utilization = 0.0;
  newRight = 0;
  newTop = 0;
  averageCellWidth = 0;
  stretchedInX = false;
  stretchedInY = false;
}

void 
Bin::BinGetBoundingBox(uint& left, uint& right, uint& bot, uint& top)
{
  left = this->left;
  right = this->right;
  bot = this->bot;
  top = this->top;
}

uint 
Bin::BinGetIdx(void)
{
  return (this->binIdx);
}

uint 
Bin::BinGetLeft(void)
{
  return (this->left);
}

uint 
Bin::BinGetRight(void)
{
  return (this->right);
}

uint 
Bin::BinGetBot(void)
{
  return (bot = this->bot);
}

uint 
Bin::BinGetTop(void)
{
  return (this->top);
}

double
Bin::BinGetNewRight(void)
{
  return (this->newRight);
}

double
Bin::BinGetNewTop(void)
{
  return (this->newTop);
}

double 
Bin::BinGetUtilization(void)
{
  return utilization;
} 

double 
Bin::BinGetCellArea(void)
{
  return cellArea;
}

double 
Bin::BinGetAverageCellWidth(void)
{
  return averageCellWidth;
}

bool
Bin::BinStretchInX(void)
{
  return stretchedInX;
}

bool
Bin::BinStretchInY(void)
{
  return stretchedInY;
}

vector<Cell *>& 
Bin::BinGetCells(void)
{
  return cellsOfBin;
}
  
/* Set functions */
void 
Bin::BinSetBoundingBox(uint left, uint right, uint bot, uint top)
{
  this->left = left;
  this->right = right;
  this->bot = bot;
  this->top = top;
}

void 
Bin::BinSetCellArea(double binCellArea)
{
  this->cellArea = binCellArea;
}

void 
Bin::BinAddCellArea(double cellArea)
{
  this->cellArea += cellArea;
  this->utilization = cellArea / binArea;
}

void
Bin::BinSetTotalCellWidth(double totalCellWidth)
{
  this->totalCellWidth = totalCellWidth;
}

void
Bin::BinAddCellWidth(double cellWidth)
{
  this->totalCellWidth += cellWidth;
}

void 
Bin::BinComputeAverageCellWidth(void)
{
  this->averageCellWidth = totalCellWidth / (cellsOfBin.size());
}

void 
Bin::BinSetAverageCellWidth(double averageCellWidth)
{
  this->averageCellWidth = averageCellWidth;
}

void 
Bin::BinSetUtilization(double utilization)
{
  this->utilization = utilization;
}

void 
Bin::BinComputeUtilization(void)
{
  double height;
  double width;
  
  height = (double)(top - bot);
  width = (double)(right - left);
  
  this->utilization = cellArea / (height * width);
}

void
Bin::BinAddCells(vector<Cell *>& cellsToAdd) 
{
  cellsOfBin.insert(cellsOfBin.end(), cellsToAdd.begin(),
		    cellsToAdd.end());
}

void
Bin::BinAddCell(Cell *cellToAdd) 
{
  double height, width;

  cellsOfBin.push_back(cellToAdd);
  totalCellWidth += (*cellToAdd).CellGetWidth();
  cellArea += (double)(*cellToAdd).CellGetArea();
  height = (double)(top - bot);
  width = (double)(right - left);

  utilization = cellArea / (height * width);
}

void
Bin::BinAddCellVirtual(Cell *cellToAdd) 
{
  double height, width;

  totalCellWidth += (*cellToAdd).CellGetWidth();
  cellArea += (double)(*cellToAdd).CellGetArea();
  height = (double)(top - bot);
  width = (double)(right - left);

  utilization = cellArea / (height * width);
}

void
Bin::BinRemoveCell(uint cellIdx) 
{
  Cell *cellPtr;

  cellPtr = cellsOfBin[cellIdx];
  //  cellsOfBin.erase(cellsOfBin.begin() + cellIdx);
  totalCellWidth -= (*cellPtr).CellGetWidth();
  cellArea -= (double)(*cellPtr).CellGetArea();
  BinComputeUtilization();
}

void 
Bin::BinSetCells(vector<Cell *>& cellsOfBin)
{
  this->cellsOfBin = cellsOfBin;
}

void
Bin::BinSetRight(uint right)
{
  this->right = right;
}

void
Bin::BinSetTop(uint top)
{
  this->top = top;
}

void
Bin::BinSetNewRight(double newRight)
{
  this->newRight = newRight;
  if (newRight != right) {
    stretchedInX = true;
  }
}

void
Bin::BinSetNewTop(double newTop)
{
  this->newTop = newTop;
  if (newTop != top) {
    stretchedInY = true;
  }
}

void
Bin::BinDeleteData(void)
{
  this->cellsOfBin.clear();
  this->newRight = 0;
  this->newTop = 0;
  this->utilization = 0;
  this->cellArea = 0;
  this->averageCellWidth = 0;
  this->totalCellWidth = 0;
  this->stretchedInX = false;
  this->stretchedInY = false;
}  


void
Bin::BinPrintBin(void)
{
  cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
  cout << "<<   BIN: " << binIdx << endl;
  cout << "<<   Address: " << this << endl;
  cout << "<<   left: " << left << endl;
  cout << "<<   bot: " << bot << endl;
  cout << "<<   top: " << top << endl;
  cout << "<<   right: " << right << endl;
  cout << "<<    " << endl;
  cout << "<<   newRight: " << newRight << endl;
  cout << "<<   newTop: " <<  newTop << endl;
  cout << "<<            " << endl;
  cout << "<<   Number of cells: " << cellsOfBin.size() << endl;
  cout << "<<   TotalCellArea: " << cellArea << endl;
  cout << "<<   Utilization: " << utilization << endl;
  cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

