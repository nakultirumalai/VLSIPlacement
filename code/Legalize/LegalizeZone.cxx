# include <Legalize.h>

using namespace std;

/* Set functions for Zone */

void
Zone::ZoneSetBegin(int zoneBegin)
{
  this->zoneBegin = zoneBegin;
}

void
Zone::ZoneSetEnd(int zoneEnd)
{
  this->zoneEnd = zoneEnd;
}

void
Zone::ZoneSetEmpty(bool isEmpty)
{
  this->isEmpty = isEmpty;
}

void
Zone::ZoneSetRow(PhysRow* row)
{
  (this->presentInRow) = row;
}

void
Zone::ZoneSetTotalCellWidth(int totalCellWidth)
{
  (this->totalCellWidth) = totalCellWidth;
}

void
Zone::ZoneSetLRBound(unsigned int LRBound)
{
  (this->zoneLRBound) = LRBound;
}

/* Get functions for zone */
int
Zone::ZoneGetBegin(void)
{
  return zoneBegin;
}

int
Zone::ZoneGetEnd(void)
{
  return zoneEnd;
}

int
Zone::ZoneGetBot(void)
{
  int bot = (*(this->presentInRow)).PhysRowGetCoordinate();
  return bot;
}

bool
Zone::ZoneGetEmpty(void)
{
  return isEmpty;
}

PhysRow* 
Zone::ZoneGetRow(void)
{
  return presentInRow;
}

vector<Cell*>& 
Zone::ZoneGetCellsInZone(void)
{
  return cellsInZone;
}

/* Other Functions */
void
Zone::ZoneAddCellToZone(Cell* myCell)
{
  (this->cellsInZone).push_back(myCell);
  (this->totalCellWidth) += (myCell->CellGetWidth());
}

void
Zone::ZoneRemoveCellFromZone(Cell *thisCell)
{
  int cellWidth, i; 
  uint numCells;
  vector<Cell *>::iterator it;
  Cell *cellPtr;
  bool found;
  cellWidth = (*thisCell).CellGetWidth();
  this->totalCellWidth -= cellWidth;
  found = false;
  numCells = (this->cellsInZone).size();
  for (it = (this->cellsInZone).begin(); it != (this->cellsInZone).end(); ++it) {
    cellPtr = *it;
    if (cellPtr == thisCell) {
      found = true;
      break;
    }
  }
  if (found) {
    (this->cellsInZone).erase(it);
  }

}

int
Zone::ZoneGetWidth(void)
{
  (this->zoneWidth) = (this->zoneEnd) - (this->zoneBegin);
  return zoneWidth;
}

int
Zone::ZoneGetTotalCellWidth(void)
{
  return totalCellWidth;
}

unsigned int
Zone::ZoneGetLRBound(void)
{
  return zoneLRBound;
}

/*		     
void
Zone::ZoneFindCellsInZone(vector<Cell*> & cellsInRow)
{
  Cell* CObj;
  VECTOR_FOR_ALL_ELEMS(cellsInRow, Cell*, CObj){
    int cellBegin = CObj->CellGetXpos();
    int cellEnd = cellBegin + (CObj->CellGetWidth());
    int mid = (cellBegin + cellEnd) / 2 ;
    if((mid >= (this->zoneBegin)) && (mid < (this->zoneEnd)) && ((this->isEmpty) == 0)){
      ZoneAddCellToZone(CObj);
      
    }
  }END_FOR;
  sort((this->cellsInZone).begin(),(this->cellsInZone).end(), ascendingXpos);
}
*/
  
/* Constructors and Destructors */
Zone::Zone()
{
  ZoneSetBegin(DEFAULT_ZONE_BEGIN);
  ZoneSetRow(DEFAULT_ZROW);
  ZoneSetEnd(DEFAULT_ZONE_END);
  ZoneSetEmpty(DEFAULT_EMPTY);
  ZoneSetTotalCellWidth(DEFAULT_ZTOTAL_CELL_WIDTH);
  ZoneSetLRBound(DEFAULT_ZLRBOUND);
}

Zone::Zone(int zoneBegin, bool isEmpty)
{
  ZoneSetBegin(zoneBegin);
  ZoneSetRow(DEFAULT_ZROW);
  ZoneSetEnd(DEFAULT_ZONE_END);
  ZoneSetEmpty(isEmpty);
  ZoneSetTotalCellWidth(DEFAULT_ZTOTAL_CELL_WIDTH);
  ZoneSetLRBound(DEFAULT_ZLRBOUND);
}

Zone::Zone(int zoneBegin, PhysRow *thisRow, bool isEmpty)
{
  ZoneSetBegin(zoneBegin);
  ZoneSetRow(thisRow);
  ZoneSetEnd(DEFAULT_ZONE_END);
  ZoneSetEmpty(isEmpty);
  ZoneSetTotalCellWidth(DEFAULT_ZTOTAL_CELL_WIDTH);
  ZoneSetLRBound(DEFAULT_ZLRBOUND);
}
