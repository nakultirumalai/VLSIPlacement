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
Design::DesignGetNumPhysRows(void)
{
  unsigned int rtv;
  
  rtv = this->NumPhysRows;
  
  return (rtv);
}

void
Design::DesignInit()
{
  /* Initialize private variables */
  NumCells = 0;
  NumNets = 0;
  NumPhysRows = 0;
  NumFixedCells = 0;

  singleRowHeight = -1;

  Name = "";
  DesignPath = "";
  DesignCellFileName = "";
  DesignNetFileName = "";
  DesignNetWtsFileName = "";
  DesignSclFileName = "";
  DesignPlFileName = "";
  RowBasedPlacement = false;
  
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

