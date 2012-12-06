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

void
Design::DesignAddOneCellToDesignDB(Cell *newCell)
{
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
  DesignPhysRows.push_back(row);
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


Design::Design(string DesignPath, string DesignName)
{
  DesignReadDesign(DesignPath, DesignName);
}


