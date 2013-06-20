# include <Design.h>

void 
Design::DesignPrintPorts(uint numPorts)
{
  Cell *portPtr;
  string portName;
  uint count;
  
  count = 0;
  DESIGN_FOR_ALL_CELLS((*this), portName, portPtr) {
    if (count == numPorts) {
      break;
    }
    Cell &thisPort = *portPtr;
    if (!thisPort.CellIsPort()) {
      continue;
    }
    cout << "Port: " << thisPort.CellGetName() 
	 << " X: " << thisPort.CellGetXpos() 
	 << " Y: " << thisPort.CellGetYpos() 
	 << endl;
    count++;
  } DESIGN_END_FOR;
}

void 
Design::DesignPrintTerminals(uint numTerminals)
{
  Cell *portPtr;
  string portName;
  uint count;
  
  count = 0;
  DESIGN_FOR_ALL_CELLS((*this), portName, portPtr) {
    if (count == numTerminals) {
      break;
    }
    Cell &thisPort = *portPtr;
    if (!thisPort.CellIsTerminal()) {
      continue;
    }
    cout << "Port: " << thisPort.CellGetName() 
	 << " X: " << thisPort.CellGetXpos() 
	 << " Y: " << thisPort.CellGetYpos() 
	 << endl;
    count++;
  } DESIGN_END_FOR;
}

void
printCell(Cell *cellPtr)
{
  cout << (*cellPtr).CellGetName() << endl;
}

void
Design::printNumTopCells(void)
{
  uint count;
  string cellName;
  Cell *cellPtr;
  
  count = 0;
  DESIGN_FOR_ALL_CELLS((*this), cellName, cellPtr) {
    count++;
  } DESIGN_END_FOR;
  cout << count << endl;
}

void
Design::printNumHiddenCells(void)
{
  cout << DesignHiddenCells.size() << endl;
}

void
Design::printNumClusterCells(void)
{
  cout << DesignClusters.size() << endl;
}
