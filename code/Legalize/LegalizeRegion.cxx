# include <Legalize.h>

using namespace std;

bool
ascendingXpos(Cell* cell1, Cell* cell2)
{
  return((cell1->CellGetXpos()) < (cell2->CellGetXpos()));
}

/* Set functions for Region */

void
Region::RegionSetBegin(int regionBegin)
{
  this->regionBegin = regionBegin;
}

void
Region::RegionSetEnd(int regionEnd)
{
  this->regionEnd = regionEnd;
}

void
Region::RegionSetEmpty(bool isEmpty)
{
  this->isEmpty = isEmpty;
}

void
Region::RegionSetSupply(int supply)
{
  this->supply = supply;
}

void
Region::RegionSetRow(PhysRow* row)
{
  (this->presentInRow) = row;
}

void
Region::RegionSetTotalCellWidth(int totalCellWidth)
{
  (this->totalCellWidth) = totalCellWidth;
}

void
Region::RegionSetLBound(bool LBound)
{
  (this->regionLBound) = LBound;
}

void
Region::RegionSetRBound(bool RBound)
{
  (this->regionRBound) = RBound;
}

/* Get functions for region */
int
Region::RegionGetBegin(void)
{
  return regionBegin;
}

int
Region::RegionGetEnd(void)
{
  return regionEnd;
}

bool
Region::RegionGetEmpty(void)
{
  return isEmpty;
}

int
Region::RegionGetSupply(void)
{
  return supply;
}

PhysRow* 
Region::RegionGetRow(void)
{
  return presentInRow;
}

vector<Cell*>& 
Region::RegionGetCellsInRegion(void)
{
  return cellsInRegion;
}

int
Region::RegionGetFCellWidth(void)
{
  vector<Cell*> cellsInRegion = RegionGetCellsInRegion();
  if (cellsInRegion.size())
    return (cellsInRegion[0]->CellGetWidth());
  else
    return 0;
}

int
Region::RegionGetLCellWidth(void)
{
  vector<Cell*> cellsInRegion = RegionGetCellsInRegion();
  if (cellsInRegion.size()){
    int end = cellsInRegion.size() - 1;
    return (cellsInRegion[end]->CellGetWidth());
  } else
    return 0;
}
  
/* Other Functions */
void
Region::RegionAddCellToRegion(Cell* myCell)
{
  (this->cellsInRegion).push_back(myCell);
  (this->totalCellWidth) += (myCell->CellGetWidth());
}

void
Region::RegionCalculateWidth(void)
{
  (this->regionWidth) = (this->regionEnd) - (this->regionBegin);
}

int
Region::RegionGetTotalCellWidth(void)
{
  return totalCellWidth;
}

int
Region::RegionGetRegionWidth(void)
{
  return regionWidth;
}

bool
Region::RegionGetLBound(void)
{
  return regionLBound;
}

bool
Region::RegionGetRBound(void)
{
  return regionRBound;
}
		     
void
Region::RegionFindCellsInRegion(vector<Cell*> & cellsInRow)
{
  Cell* CObj;
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, CObj){
    int cellBegin = CObj->CellGetXpos();
    int cellEnd = cellBegin + (CObj->CellGetWidth());
    int mid = (cellBegin + cellEnd) / 2 ;
    if((mid >= (this->regionBegin)) && (mid < (this->regionEnd))){
      RegionAddCellToRegion(CObj);
    }
  }END_FOR;
  sort((this->cellsInRegion).begin(),(this->cellsInRegion).end(), ascendingXpos);
}

  
/* Constructors and Destructors */
Region::Region()
{
  RegionSetBegin(DEFAULT_REGION_BEGIN);
  RegionSetRow(DEFAULT_ROW);
  RegionSetEnd(DEFAULT_REGION_END);
  RegionSetEmpty(DEFAULT_REMPTY);
  RegionSetSupply(DEFAULT_SUPPLY);
  RegionSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  RegionSetLBound(DEFAULT_LBOUND);
  RegionSetRBound(DEFAULT_RBOUND);
}

Region::Region(int regionBegin, PhysRow* row)
{
  RegionSetBegin(regionBegin);
  RegionSetRow(row);
  RegionSetEnd(DEFAULT_REGION_END);
  RegionSetEmpty(DEFAULT_REMPTY);
  RegionSetSupply(DEFAULT_SUPPLY);
  RegionSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  RegionSetLBound(DEFAULT_LBOUND);
  RegionSetRBound(DEFAULT_RBOUND);
}

Region::Region(int regionBegin, PhysRow* row, bool isEmpty)
{
  RegionSetBegin(regionBegin);
  RegionSetRow(row);
  RegionSetEnd(DEFAULT_REGION_END);
  RegionSetEmpty(isEmpty);
  RegionSetSupply(DEFAULT_SUPPLY);
  RegionSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  RegionSetLBound(DEFAULT_LBOUND);
  RegionSetRBound(DEFAULT_RBOUND);
}

Region::Region(int regionBegin, PhysRow* row, int rowEnd, bool isEmpty)
{
  RegionSetBegin(regionBegin);
  RegionSetRow(row);
  RegionSetEnd(rowEnd);
  RegionSetEmpty(isEmpty);
  RegionSetSupply(DEFAULT_SUPPLY);
  RegionCalculateWidth();
  RegionSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  RegionSetLBound(DEFAULT_LBOUND);
  RegionSetRBound(DEFAULT_RBOUND);
}

Region::Region(int regionBegin, PhysRow* row, int rowEnd, int supply, bool isEmpty)
{
  RegionSetBegin(regionBegin);
  RegionSetRow(row);
  RegionSetEnd(rowEnd);
  RegionSetEmpty(isEmpty);
  RegionSetSupply(supply);
  RegionCalculateWidth();
  RegionSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  RegionSetLBound(DEFAULT_LBOUND);
  RegionSetRBound(DEFAULT_RBOUND);
}
