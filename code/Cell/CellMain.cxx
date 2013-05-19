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
Cell::CellSetOrientation(objOrient orient)
{
  this->orient = orient;
}

void
Cell::CellSetName(const string& cellName)
{
  name = cellName;
}

void
Cell::CellSetOrigName(const string& origName)
{
  this->origName = origName;
}

void
Cell::CellSetLibCellName(const string& libCellName)
{
  this->libCellName = libCellName;
}

void
Cell::CellSetLibName(const string& libName)
{
  this->libName = libName;
}

void
Cell::CellSetIsTerminal(const bool& terminalCell)
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
Cell::CellSetIsClusterChild(const bool& isClusterChild)
{
  this->isClusterChild = isClusterChild;
}

void
Cell::CellSetIsClustered(const bool& isClustered)
{
  this->isClustered = isClustered;
}

void
Cell::CellSetIsMacro(const bool & isMacro) 
{
  this->isMacro = isMacro;
}

void
Cell::CellSetIsFixed(const bool & isFixed) 
{
  this->isFixed = isFixed;
}

void
Cell::CellSetClusterLevel(unsigned int clusterLevel)
{
  this->clusterLevel = clusterLevel;
}

void 
Cell::CellSetIsSequential(const bool & isSequential)
{
  this->isSequential = isSequential;
}

void 
Cell::CellSetIsPort(const bool & isPort)
{
  this->isPort = isPort;
}

void
Cell::CellSetXIsLegal(const bool & isXLegal)
{
  this->isXLegal = isXLegal;
  
}

void
Cell::CellSetYIsLegal(const bool & isYLegal)
{
  this->isYLegal = isYLegal;
}

void
Cell::CellAddChildCell(Cell &thisCell)
{
  childCells.push_back(&thisCell);
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
  (*pinPtr).PinSetParentCell((*this));
  numPins++;
}

void
Cell::CellAddArcDelay(Pin *fromPin, Pin *toPin, double delay) 
{
  unsigned int pin1Idx = (*fromPin).PinGetId();
  unsigned int pin2Idx = (*toPin).PinGetId();
  
  (arcDelays[pin1Idx])[pin2Idx] = delay;
}

void
Cell::CellIncrementClusterLevel(void)
{
  clusterLevel++;
}

void
Cell::CellDecrementClusterLevel(void)
{
  if (clusterLevel == 0) {
    _ASSERT_TRUE("Cluster level of cell already at 0.");
  }
  clusterLevel--;
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
Cell::CellGetRight(void)
{
  return (x + width);
}

int 
Cell::CellGetYpos(void)
{
  return (y);
}

int 
Cell::CellGetTop(void)
{
  return (y + height);
}

void
Cell::CellGetPos(uint &xpos, uint &ypos)
{
  xpos = this->x;
  ypos = this->y;
}

void
Cell::CellGetBoundingBox(uint &left, uint &bot, uint &right, uint &top)
{
  left = this->x;
  bot = this->y;
  
  right = this->x + this->width;
  top = this->y + this->height;
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
  } else if (pinDir == PIN_DIR_OUTPUT) {
    pinCount = numOutPins;
  } else {
    pinCount = numPins;
  }

  return (pinCount);
}

int 
Cell::CellGetNumPins(void)
{
  return (CellGetNumPins(PIN_DIR_ALL));
}

unsigned int
Cell::CellGetClusterLevel(void)
{
  return (this->clusterLevel);
}

objOrient 
Cell::CellGetOrientation(void)
{
  return (orient);
}

unsigned int
Cell::CellGetArea(void)
{
  return (height * width);
}

bool 
Cell::CellIsTerminal(void)
{
  return (this->terminalCell);
}

bool
Cell::CellIsCluster(void)
{
  return (this->isCluster);
}

bool
Cell::CellIsClusterChild(void)
{
  return (isClusterChild);
}

bool
Cell::CellIsClustered(void)
{
  return (isClustered);
}

bool
Cell::CellIsMacro(void)
{
  return (isMacro);
}

bool
Cell::CellIsFixed(void)
{
  return (isFixed);
}

bool
Cell::CellIsSequential(void)
{
  return (isSequential);
}

bool
Cell::CellIsPort(void)
{
  return (isPort);
}

bool
Cell::CellXIsLegal(void)
{
  return isXLegal;
}

bool
Cell::CellYIsLegal(void)
{
  return isYLegal;
}

string
Cell::CellGetName(void)
{
  return name;
}

string
Cell::CellGetOrigName(void)
{
  return origName;
}

string
Cell::CellGetLibCellName(void)
{
  return libCellName;
}

string
Cell::CellGetLibName(void)
{
  return libName;
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

vector<Cell *>
Cell::CellGetChildCells(void)
{
  return (this->childCells);
}

double
Cell::CellGetArcDelay(Pin *fromPin, Pin *toPin)
{
  unsigned int pin1Idx, pin2Idx;
  double rtv;

  pin1Idx = (*fromPin).PinGetId();
  pin2Idx = (*toPin).PinGetId();

  rtv = ((arcDelays[pin1Idx])[pin2Idx]);
}

Cell::Cell()
{
  CellSetPos(0, 0);
  CellSetHeight(0);
  CellSetWidth(0);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width) 
{
  CellSetPos(0,0);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, string Name)
{
  CellSetPos(0,0);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}


Cell::Cell(int Height, int Width, string Name, bool terminalCell)
{
  CellSetPos(0,0);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(terminalCell);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, string Name)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, string Name, 
	   bool terminalCell)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(N);
  CellSetIsTerminal(terminalCell);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, objOrient Orientation)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, objOrient Orientation, string Name)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetIsTerminal(false);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, objOrient Orientation, string Name, bool terminalCell)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetName(Name);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetIsTerminal(terminalCell);
  CellSetIsCluster(false);
  CellSetIsClusterChild(false);
  CellSetIsMacro(false);
  CellSetIsFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
}

Cell::~Cell()
{}

Pin *
Cell::CellGetPinByName(const string &PinName) 
{
  Pin* pinPtr;
  Pin* rtv;
  
  rtv = NIL(Pin *);
  CELL_FOR_ALL_PINS((*this), PIN_DIR_ALL, pinPtr) {
    if ((*pinPtr).PinGetName() == PinName) {
      rtv = pinPtr;
      break;
    }
  } CELL_END_FOR;
  
  return (rtv);
}

