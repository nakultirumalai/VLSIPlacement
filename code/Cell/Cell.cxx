# include <Cell.h>

void 
Cell::CellSetXpos(int Xpos) 
{
  x = Xpos;
}

void
Cell::CellSetYpos(int Ypos)
{
  y = Ypos;
}

void
Cell::CellSetPos(int Xpos, int Ypos) 
{
  x = Xpos;
  y = Ypos;
}

void
Cell::CellSetHeight(int Height) 
{
  height = Height;
}

void
Cell::CellSetWidth(int Width)
{
  width = Width;
}

void
Cell::CellSetOrientation(int Orientation)
{
  orientation = Orientation;
}

void
Cell::CellSetName(const string& cellName)
{
  name = cellName;
}

void
Cell::CellSetTerminal(const bool& terminalCell)
{
  this->terminalCell = terminalCell;
}

void
Cell::CellSetNumPins(int numPins)
{
  this->numPins = numPins;
}

void 
Cell::CellSetNumInPins(int numInPins)
{
  this->numInPins = numInPins;
}

void 
Cell::CellSetNumOutPins(int numOutPins)
{
  this->numOutPins = numOutPins;
}

void
Cell::CellSetIsCluster(const bool& isCluster)
{
  this->isCluster = isCluster;
}

void
Cell::CellAddPin(Pin *pinPtr)
{
  int pinDir;

  Pins.push_back(pinPtr);
  
  pinDir = (*pinPtr).PinGetDirection();
  if (pinDir == PIN_DIR_INPUT) {
    numInPins++;
  } else {
    numOutPins++;
  }

  numPins++;
}

void
Cell::CellMoveRight(int offset)
{
  x += offset;
}

void
Cell::CellMoveLeft(int offset)
{
  x -= offset;
}

void
Cell::CellMoveUp(int offset)
{
  y += offset;
}

void
Cell::CellMoveDown(int offset)
{
  y -= offset;
}

void
Cell::CellMoveCell(int XOffset, int YOffset)
{
  x = x + XOffset;
  y = y + YOffset;
}

int 
Cell::CellGetXpos(void)
{
  return (x);
}

int 
Cell::CellGetYpos(void)
{
  return (y);
}

int 
Cell::CellGetHeight(void)
{
  return (height);
}

int 
Cell::CellGetWidth(void)
{
  return (width);
}

int 
Cell::CellGetNumPins(int pinDir)
{
  int pinCount = 0;

  if (pinDir == PIN_DIR_INPUT) {
    pinCount = numInPins;
  } else {
    pinCount = numOutPins;
  }
  
  return (pinCount);
}

int 
Cell::CellGetNumPins(void)
{
  return (CellGetNumPins(PIN_DIR_ALL));
}

int 
Cell::CellGetOrientation(void)
{
  return (orientation);
}

unsigned int
Cell::CellGetArea(void)
{
  return (height * width);
}

bool 
Cell::CellGetTerminal(void)
{
  return (terminalCell);
}

bool
Cell::CellGetIsCluster(void)
{
  return (isCluster);
}

string
Cell::CellGetName(void)
{
  return name;
}

vector<Pin *>
Cell::CellGetPins(int pinDir)
{
  vector<Pin *> cellPins = CellGetPins();
  vector<Pin *> returnPins;
  Pin *newPin;

  VECTOR_FOR_ALL_ELEMS(cellPins, Pin*, newPin) {
    if ((*newPin).PinGetDirection() != pinDir) {
      continue;
    }
    returnPins.push_back(newPin);
  } END_FOR;
  
  return (returnPins);
}

vector<Pin *>
Cell::CellGetPins(void)
{
  vector <Pin *> Pins = this->Pins;
  
  return (Pins);
}

Cell::Cell(int Height, int Width) 
{
  CellSetPos(0,0);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(false);
  CellSetIsCluster(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, string Name)
{
  CellSetPos(0,0);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(false);
  CellSetIsCluster(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}


Cell::Cell(int Height, int Width, string Name, bool terminalCell)
{
  CellSetPos(0,0);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(terminalCell);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, string Name)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, string Name, 
	   bool terminalCell)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(terminalCell);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, char Orientation)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetTerminal(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, char Orientation, string Name)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetTerminal(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, char Orientation, string Name, bool terminalCell)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetTerminal(terminalCell);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
}
