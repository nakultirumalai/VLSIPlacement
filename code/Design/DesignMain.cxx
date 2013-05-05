# include <Design.h>

using namespace std;

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
  }
  DesignCells[(*newCell).CellGetName()] = newCell;
  this->NumCells++;
}


void
Design::DesignAddOneNetToDesignDB(Net *newNet)
{
  DesignNets[(*newNet).NetGetName()] = newNet;
  this->NumNets++;
}

void
Design::DesignAddOnePhysRowToDesignDB(PhysRow *row)
{
  unsigned int RowHeight;
  
  RowHeight = (*row).PhysRowGetHeight();

  DesignPhysRows.push_back(row);

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
	foundPos = true;
	break;
      }
    }
  }END_FOR;
  if (!foundPos)
    cout<<"No suitable location for cell: "<<(cell->CellGetName())<<endl;
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
  VECTOR_FOR_ALL_ELEMS(allPhysRows, PhysRow*, Obj){
    vector<int> v;
    Obj->PhysRowGetBoundingBox(v);
    allRowBounds.push_back(v);
  }END_FOR;
  
  DESIGN_FOR_ALL_CELLS((*this), CellName, CellPtr){
    DesignAddCellToPhysRow(CellPtr, allRowBounds, allPhysRows);
  }DESIGN_END_FOR;

  
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
Design::DesignInit()
{
  /* Initialize private variables */
  NumCells = 0;
  NumNets = 0;
  NumPhysRows = 0;
  NumFixedCells = 0;
  NumTerminalCells = 0;

  singleRowHeight = -1;
  clockPeriod = 0.0;

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
  }END_FOR;
}

# endif
