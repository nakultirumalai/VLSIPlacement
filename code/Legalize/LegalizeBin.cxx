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
LegalizeBin::BinSetSupply(int supply)
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

int
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
  
/* Other Functions */
void
LegalizeBin::BinAddCellToBin(Cell* myCell)
{
  int cellBegin = myCell->CellGetBegin();
  int cellEnd = myCell->CellGetEnd();
  int cellWidth = myCell->CellGetWidth();
  if ((cellBegin >= (this->binBegin)) && (cellEnd < (this->binEnd))){
    (this->cellsInBin).push_back(myCell);
    (this->totalCellWidth) += (myCell->CellGetWidth());
    cellFracts[myCell] = 1.0;
  }
  else if ((cellBegin >= (this->binBegin)) && (cellEnd >= (this->binEnd))){
    int diff = cellEnd - (this->binEnd) - 1;
    double fract = diff/cellWidth;
    cellFracts[myCell] = fract;
  }
  else if ((cellBegin < (this->binBegin)) && (cellEnd < (this->binEnd))){
    int diff = (this->binBegin) - cellBegin;
    double fract = diff/cellWidth;
    cellFracts[myCell] = fract;
  }    
  sort((this->cellsInBin).begin(),(this->cellsInBin).end(), ascendingXpos);
}

void
LegalizeBin::BinCalculateWidth(void)
{
  (this->binWidth) = (this->binEnd) - (this->binBegin);
}

int
LegalizeBin::BinGetTotalCellWidth(void)
{
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
		     
void
LegalizeBin::BinFindCellsInBin(vector<Cell*> & cellsInRow)
{
  Cell* CObj;
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, CObj){
    int cellBegin = CObj->CellGetXpos();
    int cellEnd = cellBegin + (CObj->CellGetWidth());
    int mid = (cellBegin + cellEnd) / 2 ;
    if((mid >= (this->binBegin)) && (mid < (this->binEnd))){
      BinAddCellToBin(CObj);
    }
  }END_FOR;
  sort((this->cellsInBin).begin(),(this->cellsInBin).end(), ascendingXpos);
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
