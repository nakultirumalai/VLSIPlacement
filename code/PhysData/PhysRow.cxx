# include <PhysRow.h>

void
PhysRow::PhysRowSetCoordinate(int coordinate)
{
  this->coordinate = coordinate;
}

void
PhysRow::PhysRowSetType(rowOrientation rowType)
{
  this->rowType = rowType;
}

void
PhysRow::PhysRowSetHeight(unsigned int height)
{
  this->height = height;
}

void
PhysRow::PhysRowSetSiteWidth(unsigned int siteWidth)
{
  this->siteWidth = siteWidth;
}

void
PhysRow::PhysRowSetSiteSpacing(unsigned int siteSpacing)
{
  this->siteSpacing = siteSpacing;
}

void
PhysRow::PhysRowSetSiteOrientation(objOrient orient)
{
  this->siteOrient = orient;
}

void
PhysRow::PhysRowSetSiteSymmetry(siteSymmetry symmetry)
{
  this->siteSym = symmetry;
}

void
PhysRow::PhysRowSetSubRows(map<unsigned int, unsigned int> subRows)
{
  this->subRows = subRows;
}

void
PhysRow::PhysRowSetNumSubRows(unsigned int numSubRows)
{
  this->numSubRows = numSubRows;
}

void
PhysRow::PhysRowSetNumSites(unsigned int numSites) 
{
  this->numSites = numSites;
}

void
PhysRow::PhysRowSetTotalCellWidth(int totalCellWidth)
{
  this->totalCellWidth = totalCellWidth;
}

void
PhysRow::PhysRowSetBoundingBoxWidth(int totalBoundingBoxWidth)
{
  this->totalBoundingBoxWidth = totalBoundingBoxWidth;
}

void
PhysRow::PhysRowSetBlockedWidth(int blockedWidth)
{
  this->blockedWidth = blockedWidth;
}

void
PhysRow::PhysRowSetRowBegin(int rowBegin)
{
  this->rowBegin = rowBegin;
}


void
PhysRow::PhysRowSetIndex(int index)
{
  this->rowIndex = index;
}


int
PhysRow::PhysRowCalculateRowEnd(void)
{
  return ((this->numSites) * (this->siteSpacing));
}

void
PhysRow::PhysRowCalculateWMax(void)
{
  this->wMax = ((this->totalBoundingBoxWidth) - (this->blockedWidth));
}

void
PhysRow::PhysRowIncrementSubRows(void)
{
  this->numSubRows++;
}

int
PhysRow::PhysRowGetCoordinate(void)
{
  return coordinate;
}

objOrient
PhysRow::PhysRowGetSiteOrientation(void)
{
  return (siteOrient);
}

string
PhysRow::PhysRowGetSiteOrientationStr(void)
{
  string rtv;

  switch (this->siteOrient) {
  case N: rtv = "N"; break;
  case E: rtv = "E"; break;
  case S: rtv = "S"; break;
  case W: rtv = "W"; break;
  case FN: rtv = "FN"; break;
  case FE: rtv = "FE"; break;
  case FS: rtv = "FS"; break;
  case FW: rtv = "FW"; break;
  default: rtv = "Unknown"; break;
  };

  return (rtv);
}

siteSymmetry
PhysRow::PhysRowGetSiteSymmetry(void)
{
  return siteSym;
}

string
PhysRow::PhysRowGetSiteSymmetryStr(void)
{
  string rtv;

  switch (siteSym) {
  case NO_SYMMETRY: rtv = "1"; break;
  case YES_SYMMETRY: rtv = "Y"; break;
  default: rtv = "Unknown"; break;
  };
  
  return (rtv);
}

rowOrientation
PhysRow::PhysRowGetType(void)
{
  return rowType;
}

string
PhysRow::PhysRowGetTypeStr(void)
{
  string rtv;
  switch (rowType) {
  case VERTICAL: rtv = "Vertical"; break;
  case HORIZONTAL: rtv = "Horizontal"; break;
  };

  return (rtv);
}

unsigned int
PhysRow::PhysRowGetHeight(void)
{
  return height;
}

unsigned int
PhysRow::PhysRowGetSiteWidth(void)
{
  return siteWidth;
}

unsigned int 
PhysRow::PhysRowGetSiteSpacing(void)
{
  return siteSpacing;
}

unsigned int
PhysRow::PhysRowGetNumSites(void) 
{
  return numSites;
}

unsigned int
PhysRow::PhysRowGetNumSubRows(void)
{
  return numSubRows;
}

map<unsigned int, unsigned int>
PhysRow::PhysRowGetSubRows(void)
{
  map<unsigned int, unsigned int> subRows;
  
  subRows = this->subRows;
  
  return subRows;
}

int
PhysRow::PhysRowGetTotalCellWidth(void)
{
  return totalCellWidth;
}
 
int 
PhysRow::PhysRowGetBoundingBoxWidth(void)
{
  return totalBoundingBoxWidth;
}

int 
PhysRow::PhysRowGetBlockedWidth(void)
{
  return blockedWidth;
}

int 
PhysRow::PhysRowGetRowBegin(void)
{
  return rowBegin;
}

int 
PhysRow::PhysRowGetWMax(void)
{
  return wMax;
}

int
PhysRow::PhysRowGetIndex(void)
{
  return rowIndex;
}

void
PhysRow::PhysRowAddSubRow(unsigned int rowOrigin, unsigned int numSites) 
{
  //vector<Cell *> emptyVec;
  this->subRows[rowOrigin] = numSites;
  this->numSites += numSites;
  PhysRowIncrementSubRows();
  this->totalBoundingBoxWidth += (numSites * siteSpacing);
  /* Initializing the vector containing all cells */
  //(this->allCellsInRow).push_back(emptyVec);  
}




void
PhysRow::PhysRowAddCellToRow(Cell* &myCell)
{
  int cellWidth;
  (this->cellsInRow).push_back(myCell);
  cellWidth =  (myCell->CellGetWidth());
  (this->totalCellWidth) += cellWidth;
  double supply = totalCellWidth - totalBoundingBoxWidth;
  PhysRowSetSupply(this, supply);
}

void
PhysRow::PhysRowRemoveCellFromRow(Cell* &myCell)
{
  int cellWidth;
  cellWidth =  (myCell->CellGetWidth());
  (this->totalCellWidth) -= cellWidth;
  (this->cellsInRow).erase(remove(this->cellsInRow.begin(),this->cellsInRow.end(), myCell), this->cellsInRow.end());
  double supply = totalCellWidth - totalBoundingBoxWidth;
  PhysRowSetSupply(this, supply);
}

void 
PhysRow::PhysRowGetCellsInRow(vector<Cell *> &allCells)
{
  allCells = (this->cellsInRow);
}

void 
PhysRow::PhysRowMarkFixedCellsInRow(int columnWidth)
{
  Cell* Obj;
  VECTOR_FOR_ALL_ELEMS((this->cellsInRow), Cell*, Obj){
    int cellWidth = Obj->CellGetWidth();
    if(cellWidth > (2 * columnWidth)){
      CellSetIsFixed(Obj);
    }
  }END_FOR;
}

void
PhysRow::PhysRowGetBoundingBox(int &minx, int &miny, int &maxx, int &maxy)
{
  if ((this->rowType) == HORIZONTAL){
    minx = this->rowBegin;
    miny = this->coordinate;
    maxx = minx + (this->numSites) * (this->siteSpacing);
    maxy = miny + (this->height);
  } else if ((this->rowType) == VERTICAL) {
    minx = this->coordinate;
    miny = this->rowBegin;
    maxx = minx + this->height;
    maxy = miny + (this->numSites) * (this->siteSpacing);
  }
}

void
PhysRow::PhysRowGetBoundingBox(vector<int> &v)
{
  if ((this->rowType) == HORIZONTAL) {
    int leftX = (int)(this->rowBegin);
    int leftY = (int)(this->coordinate);
    int rightX = (int)((this->numSites) * (this->siteSpacing));
    int rightY = (int)((this->coordinate) + (this->height));
    
    /* Left Bottom */
    v.push_back(leftX);
    v.push_back(leftY);
    
    /* Right Top */
    v.push_back(rightX);
    v.push_back(rightY);
  } else if ((this->rowType) == VERTICAL){

    int leftX = (int)(this->coordinate);
    int leftY = (int)(this->rowBegin);
    int rightX = (int)((this->coordinate) + (this->height));
    int rightY = (int)((this->numSites) * (this->siteSpacing));
    /* Left Bottom */
    v.push_back(leftX);
    v.push_back(leftY);      
    /* Right Top */
    v.push_back(rightX);
    v.push_back(rightY);
  }
}

void
PhysRow::PhysRowRemoveAllCells(void)
{
  (this->cellsInRow).clear();
  (this->totalCellWidth) = 0;
}

/* Constructors begin */
PhysRow::PhysRow(rowOrientation orient)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);
  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);
  
}

PhysRow::PhysRow(rowOrientation orient, unsigned int height)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);
  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);  
  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 unsigned int siteWidth, unsigned int siteSpacing) 
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);  
  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 unsigned int siteWidth, unsigned int siteSpacing, 
		 objOrient siteOrient, siteSymmetry symmetry)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(siteOrient);
  PhysRowSetSiteSymmetry(symmetry);
  PhysRowSetHeight(height);
  PhysRowSetSiteSpacing(siteSpacing);
  PhysRowSetSiteWidth(siteWidth);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);  
  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 unsigned int siteWidth, unsigned int siteSpacing, 
		 unsigned int numSubRows, map<unsigned int, unsigned int> subRows)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(height);
  PhysRowSetSiteSpacing(siteSpacing);
  PhysRowSetSiteWidth(siteWidth);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(numSubRows);
  PhysRowSetSubRows(subRows);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);  
  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 map<unsigned int, unsigned int> subRows)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetHeight(height);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetNumSites(DEFAULT_NUM_SITES);
  PhysRowSetNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(subRows);
  PhysRowSetTotalCellWidth(DEFAULT_TOTAL_CELL_WIDTH);
  PhysRowSetBoundingBoxWidth(DEFAULT_TOTAL_BOUNDINGBOX_WIDTH);
  PhysRowSetBlockedWidth(DEFAULT_BLOCKED_WIDTH);
  PhysRowSetRowBegin(DEFAULT_ROW_BEGIN);  
  
}

PhysRow::~PhysRow()
{}


rowOrientation 
PhysRowGetRowTypeFromStr(string rowType)
{
  rowOrientation retVal;

  if (rowType == "Horizontal") {
    retVal = HORIZONTAL;
  } else if (rowType == "Vertical") {
    retVal = VERTICAL;
  }
  
  return (retVal);
}	    

siteSymmetry 
PhysRowGetSiteSymmetryFromStr(string symmetry)
{
  siteSymmetry retVal;
  
  if (symmetry == "1" || "Y") retVal = YES_SYMMETRY;
  else retVal = NO_SYMMETRY;
  
  return retVal;
}

# if 0
void
PhysRow::PhysRowSetColumnWidth(int columnWidth)
{
  this->columnWidth = columnWidth;
}
int 
PhysRow::PhysRowGetColumnWidth(void)
{
  return columnWidth;
}

void
PhysRow::PhysRowAddCellToSubRow(Cell* &myCell, unsigned int subRowIndex)
{
  ((this->allCellsInRow)[subRowIndex]).push_back(myCell);
  if(!(myCell->CellIsTerminal())){
    (this->totalCellWidth) += (myCell->CellGetWidth());
  }
  else{
    (this->blockedWidth) += (myCell->CellGetWidth());
  }
  
}

void
PhysRow::PhysRowAddZoneToRow(int begin, int end)
{
  (this->zones).push_back(begin);
  (this->zones).push_back(end);
}

void
PhysRow::PhysRowGetFixedCellCoords(vector<int> &cellCoords)
{
  vector<Cell*> allCells;
  PhysRowGetCellsInRow(allCells);
  Cell* Obj;
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj){
    if(CellIsFixed){
      int cellX = Obj->CellGetXpos();
      cellCoords.push_back(cellX);
    }
  }END_FOR;
  sort(cellCoords.begin(),cellCoords.end());
}

void
PhysRow::PhysRowFindZonesInRow(void)
{
  vector<Cell*> allCells;
  PhysRowGetCellsInRow(allCells);
  vector<Cell*> fixedCells;
  /* Get all the fixed cells in the row */
  Cell* Obj;
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj){
    if(CellIsFixed){
      fixedCells.push_back(Obj);
    }
  }END_FOR;
  
  VECTOR_FOR_ALL_ELEMS(allCells, Cell*, Obj)
    if(CellIsFixed){
      fixedCells.push_back(Obj);
    }
  }END_FOR;
  
  int zoneBegin = 0;
  int zoneEnd = 0;
  int rowEnd = (this->numSites) * (this->siteSpacing);
  vector<int> cellCoords;
  PhysRowGetFixedCellCoords(cellCoords);
  bool firstBlockedZone = true;
  if(firstBlockedZone){
    PhysRowAddZone
      }
}
void
PhysRow::PhysRowGetBoundingBox(vector<int> &v)
{
   if((PhysRowGetType())==HORIZONTAL){
     /* Left Bottom */
     v.push_back(this->rowBegin);
     v.push_back(this->coordinate);
      
      /* Right Top */
     v.push_back((this->numSites)*(this->siteSpacing));
     v.push_back((this->coordinate)+(this->height));
    }
    else{
      map<unsigned int, unsigned int> subRows = PhysRowGetSubRows();
      for(map<unsigned int, unsigned int>::iterator iter = subRows.begin(); iter!= subRows.end(); ++iter){
	v.push_back(iter->first);
	v.push_back(coordinate);
	v.push_back((iter->first)+(iter->second)*siteSpacing);
	v.push_back(coordinate+height);
      }
    }
  }
  else if((PhysRowGetType())==VERTICAL){
    if(number==1){
      /* Left Bottom */
      v.push_back(coordinate);
      v.push_back(0);
      
      /* Right Top */
      v.push_back(coordinate+height);
      v.push_back(numSites*siteSpacing);
    }
    else{
      map<unsigned int, unsigned int> subRows = PhysRowGetSubRows();
      for(map<unsigned int, unsigned int>::iterator iter= subRows.begin(); iter!= subRows.end(); ++iter){
	v.push_back(coordinate);
	v.push_back(iter->first);
	v.push_back(coordinate+height);
	v.push_back((iter->first)+(iter->second)*numSites);
      }
    }
  }
}

# endif
