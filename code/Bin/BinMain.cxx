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
  utilization = 0.0;
  newRight = 0;
  newTop = 0;
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
Bin::BinSetAverageCellWidth(double avgCellWidth)
{
  this->averageCellWidth = avgCellWidth;
}

void 
Bin::BinSetUtilization(double utilization)
{
  this->utilization = utilization;
}

void 
Bin::BinSetCells(vector<Cell *>& cellsOfBin)
{
  this->cellsOfBin = cellsOfBin;
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
  cout << "<<   Utilization: " << utilization << endl;
  cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

