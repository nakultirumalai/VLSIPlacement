# include <Legalize.h>

using namespace std;

bool
ascendingXpos(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetXpos()) < (cell2->CellGetXpos()));
}

/* Set functions for Bin */

void
LegalizeBin::BinSetBegin(int binBegin)
{
  this->binBegin = binBegin;
}

void
LegalizeBin::BinSetEnd(int binEnd)
{
  this->binEnd = binEnd;
}

void
LegalizeBin::BinSetEmpty(bool isEmpty)
{
  this->isEmpty = isEmpty;
}

void
LegalizeBin::BinSetSupply(double supply)
{
  this->supply = supply;
}

void
LegalizeBin::BinSetRow(PhysRow* row)
{
  (this->presentInRow) = row;
}

void
LegalizeBin::BinSetTotalCellWidth(int totalCellWidth)
{
  (this->totalCellWidth) = totalCellWidth;
}

void
LegalizeBin::BinSetLBound(bool LBound)
{
  (this->binLBound) = LBound;
}

void
LegalizeBin::BinSetRBound(bool RBound)
{
  (this->binRBound) = RBound;
}

void
LegalizeBin::BinSetIndex(int binIndex)
{
  (this->binIndex) = binIndex;
}

void
LegalizeBin::BinSetCellFract(Cell* thisCell, double fract)
{
  cellFracts[thisCell] = fract;
}

void
LegalizeBin::BinSetHeight(int height)
{
  this->binHeight = height;
}


void
LegalizeBin::BinSetBot(int bot)
{
  this->binBot = bot;
}

void 
LegalizeBin::BinSetZone(Zone *thisZone)
{
  (this->inZone) = thisZone;
}

/* Get functions for bin */
int
LegalizeBin::BinGetBegin(void)
{
  return binBegin;
}

int
LegalizeBin::BinGetEnd(void)
{
  return binEnd;
}

bool
LegalizeBin::BinGetEmpty(void)
{
  return isEmpty;
}

double
LegalizeBin::BinGetSupply(void)
{
  return supply;
}

PhysRow* 
LegalizeBin::BinGetRow(void)
{
  return presentInRow;
}

vector<Cell*>& 
LegalizeBin::BinGetCellsInBin(void)
{
  return cellsInBin;
}

int
LegalizeBin::BinGetFCellWidth(void)
{
  vector<Cell*> cellsInBin = BinGetCellsInBin();
  if (cellsInBin.size())
    return (cellsInBin[0]->CellGetWidth());
  else
    return 0;
}

int
LegalizeBin::BinGetLCellWidth(void)
{
  vector<Cell*> cellsInBin = BinGetCellsInBin();
  if (cellsInBin.size()){
    int end = cellsInBin.size() - 1;
    return (cellsInBin[end]->CellGetWidth());
  } else
    return 0;
}

int 
LegalizeBin::BinGetIndex(void)
{
  return (this->binIndex);
}

double
LegalizeBin::BinGetCellFract(Cell* thisCell)
{
  return (cellFracts[thisCell]);
}
 
Zone*
LegalizeBin::BinGetZone(void)
{
  return (this->inZone);
}

/* Other Functions */
void
LegalizeBin::BinAddCellToBin(Cell* thisCell)
{
  (this->cellsInBin).push_back(thisCell);
  int cellWidth = (*thisCell).CellGetWidth();
  this->totalCellWidth += cellWidth;
}

void
LegalizeBin::BinRemoveCellFromBin(Cell* thisCell)
{
  int cellWidth, i; 
  uint numCells;
  vector<Cell *>::iterator it;
  Cell *cellPtr;
  bool found;

  cellWidth = (*thisCell).CellGetWidth();
  this->totalCellWidth -= cellWidth;
  found = false;
  numCells = (this->cellsInBin).size();
  for (it = (this->cellsInBin).begin(); it != (this->cellsInBin).end(); ++it) {
    cellPtr = *it;
    if (cellPtr == thisCell) {
      found = true;
      break;
    }
  }
  if (found) {
    (this->cellsInBin).erase(it);
  }

    //  (this->cellsInBin).erase(remove((this->cellsInBin).begin(), (this->cellsInBin).end(), thisCell), (this->cellsInBin).end());
}

void
LegalizeBin::BinRemoveCellFromFract(Cell* thisCell)
{
  (this->cellFracts).erase(thisCell); 
}

void
LegalizeBin::BinRemoveAllFractCells(void)
{
  (this->cellFracts).clear(); 
}

void
LegalizeBin::BinCalculateWidth(void)
{
  (this->binWidth) = (this->binEnd) - (this->binBegin);
}

int
LegalizeBin::BinGetTotalCellWidth(void)
{
  /*
  Cell *thisCell;
  int totalCellWidth;
  int cellWidth;
  totalCellWidth = 0;
  
  VECTOR_FOR_ALL_ELEMS (this->cellsInBin, Cell*, thisCell){
    cellWidth = (*thisCell).CellGetWidth();
    totalCellWidth += cellWidth;
    } END_FOR;
  */
  return totalCellWidth;
}

int
LegalizeBin::BinGetWidth(void)
{
  return binWidth;
}

bool
LegalizeBin::BinGetLBound(void)
{
  return binLBound;
}

bool
LegalizeBin::BinGetRBound(void)
{
  return binRBound;
}

int
LegalizeBin::BinGetHeight(void)
{
  return binHeight;
}

int
LegalizeBin::BinGetBot(void)
{
  return binBot;
}

map<Cell*, double>&
LegalizeBin::BinGetAllFractCells(void)
{
  return (this->cellFracts);
}


/* Constructors and Destructors */
LegalizeBin::LegalizeBin()
{
  BinSetBegin(DEFAULT_BIN_BEGIN);
  BinSetRow(DEFAULT_ROW);
  BinSetEnd(DEFAULT_BIN_END);
  BinSetEmpty(DEFAULT_EMPTY);
  BinSetSupply(DEFAULT_SUPPLY);
  BinSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  BinSetLBound(DEFAULT_LBOUND);
  BinSetRBound(DEFAULT_RBOUND);
}

LegalizeBin::LegalizeBin(int binBegin, PhysRow* row)
{
  BinSetBegin(binBegin);
  BinSetRow(row);
  BinSetEnd(DEFAULT_BIN_END);
  BinSetEmpty(DEFAULT_EMPTY);
  BinSetSupply(DEFAULT_SUPPLY);
  BinSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  BinSetLBound(DEFAULT_LBOUND);
  BinSetRBound(DEFAULT_RBOUND);
}

LegalizeBin::LegalizeBin(int binBegin, PhysRow* row, bool isEmpty)
{
  BinSetBegin(binBegin);
  BinSetRow(row);
  BinSetEnd(DEFAULT_BIN_END);
  BinSetEmpty(isEmpty);
  BinSetSupply(DEFAULT_SUPPLY);
  BinSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  BinSetLBound(DEFAULT_LBOUND);
  BinSetRBound(DEFAULT_RBOUND);
}

LegalizeBin::LegalizeBin(int binBegin, PhysRow* row, int rowEnd, bool isEmpty)
{
  BinSetBegin(binBegin);
  BinSetRow(row);
  BinSetEnd(rowEnd);
  BinSetEmpty(isEmpty);
  BinSetSupply(DEFAULT_SUPPLY);
  BinCalculateWidth();
  BinSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  BinSetLBound(DEFAULT_LBOUND);
  BinSetRBound(DEFAULT_RBOUND);
}

LegalizeBin::LegalizeBin(int binBegin, PhysRow* row, int rowEnd, int supply, bool isEmpty)
{
  BinSetBegin(binBegin);
  BinSetRow(row);
  BinSetEnd(rowEnd);
  BinSetEmpty(isEmpty);
  BinSetSupply(supply);
  BinCalculateWidth();
  BinSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  BinSetLBound(DEFAULT_LBOUND);
  BinSetRBound(DEFAULT_RBOUND);
}
