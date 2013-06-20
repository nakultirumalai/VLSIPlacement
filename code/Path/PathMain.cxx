# include <Path.h>

Path::Path()
{
  pathLength = 0;
  slack = 0;
}

Path::~Path()
{

}

void 
Path::PathAddCellPinPair(Cell *thisCell, Pin *thisPin)
{
  Cell *cellOfPin;
  
  cellOfPin = &((*thisPin).PinGetParentCell());
  if (cellOfPin != thisCell) {
    cout << "Error: Pin does not belong to the cell passed!" << endl;
  } else {
    listOfCells.push_back(thisCell);
    listOfPins.push_back(thisPin);
    pathLength++;
  }
}

void 
Path::PathAddCellPinPair(Cell *thisCell, string libPinName)
{
  Pin *pinPtr;
  bool pinFound;
  
  pinFound = false;
  pinPtr = NIL(Pin *);
  CELL_FOR_ALL_PINS((*thisCell), PIN_DIR_ALL, pinPtr) {
    if ((*pinPtr).PinGetLibName() == libPinName) {
      pinFound = true;
      break;
    }
  } CELL_END_FOR;

  if (pinFound && pinPtr != NIL(Pin *)) {
    PathAddCellPinPair(thisCell, pinPtr);
  } else {
    cout << "Error: Specified pin name not found" << endl;
  }
}

void 
Path::PathSetSlack(double slack)
{
  this->slack = slack;
}

pair<Cell *, Pin *>
Path::PathGetFirstCellPin(void)
{
  return (make_pair(listOfCells[0], listOfPins[0]));
}

pair<Cell *, Pin *>
Path::PathGetLastCellPin(void)
{
  return (make_pair(listOfCells[pathLength], listOfPins[pathLength]));
}

vector<Cell *>&
Path::PathGetCells(void)
{
  return (this->listOfCells);
}

uint
Path::PathGetLength(void)
{
  return (pathLength + 1);
}

double
Path::PathGetSlack(void)
{
  return (slack);
}

