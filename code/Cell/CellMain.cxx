# include <Cell.h>
# include <Pin.h>

void 
Cell::CellSetXpos(uint Xpos) 
{
  x = Xpos;
}

void 
Cell::CellSetOldXpos(double oldXpos) 
{
  oldx = oldXpos;
}

void 
Cell::CellSetXposDbl(double Xpos) 
{
  x = Xpos;
}

void
Cell::CellSetYpos(uint Ypos)
{
  y = Ypos;
}

void
Cell::CellSetOldYpos(double Ypos)
{
  oldy = Ypos;
}

void
Cell::CellSetYposDbl(double Ypos)
{
  y = Ypos;
}


void
Cell::CellSetPos(uint Xpos, uint Ypos) 
{
  x = Xpos;
  y = Ypos;
}

void
Cell::CellSetPosDbl(double Xpos, double Ypos) 
{
  Pin *pinPtr;
  Net *netPtr;
  uint idx, absx, absy;
  int xOffset, yOffset;
  bool updatedx, updatedy;

  x = Xpos; y = Ypos;
  for (idx = 0; idx < numPins; idx++) {
    pinPtr = Pins[idx];
    netPtr = (*pinPtr).ConnectedNet;
    xOffset = (*pinPtr).xOffset;
    yOffset = (*pinPtr).yOffset;
    updatedx = false; updatedy = false;
    absx = x + xOffset;
    absy = y + yOffset;
    if ((*netPtr).maxx <= absx) {
      (*netPtr).maxx = absx;
      (*netPtr).pinMaxx = pinPtr;
      updatedx = true;
    } 
    if ((*netPtr).minx >= absx) {
      (*netPtr).minx = absx;
      (*netPtr).pinMinx = pinPtr;
      updatedx = true; 
    }
    if ((*netPtr).maxy <= absy) {
      (*netPtr).maxy = absy;
      (*netPtr).pinMaxy = pinPtr;
      updatedy = true;
    } 
    if ((*netPtr).miny >= absy) {
      (*netPtr).miny = absy;
      (*netPtr).pinMiny = pinPtr;
      updatedy = true;
    }
    if (updatedx) (*netPtr).xhpwl = (*netPtr).maxx - (*netPtr).minx;
    if (updatedy) (*netPtr).yhpwl = (*netPtr).maxy - (*netPtr).miny;
  }
}

void
Cell::CellSetOldPos(double oldXpos, double oldYpos) 
{
  oldx = oldXpos;
  oldy = oldYpos;
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
Cell::CellSetIsClusterFixed(const bool & isClusterFixed) 
{
  //  if (this->name == "o1419" || this->name == "o2033" 
  //      || this->name == "o1612") {
    //    cout << "break here" << endl;
  //  }
  this->isClusterFixed = isClusterFixed;
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
Cell::CellSetIsPort(const bool &isPort)
{
  this->isPort = isPort;
}

void 
Cell::CellSetIsHidden(const bool &isHidden)
{
  this->isHidden = isHidden;
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
Cell::CellSetAdjacentToPort(const bool &adjToPort) 
{
  this->adjacentToPort = adjToPort;
}

void
Cell::CellMarkNetsDirty(void)
{
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
Cell::CellSetBin(Bin *binOfCell) 
{
  cellBin = binOfCell;
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
Cell::CellMoveCell(double xPos, double yPos)
{
  Net *netPtr;
  x = xPos;
  y = yPos;
  CELL_FOR_ALL_NETS((*this), PIN_DIR_ALL, netPtr) {
    (*netPtr).NetSetDirtyHPWL(true);
    //    cout << "Affected net: " << (*netPtr).NetGetName() << endl;
  } CELL_END_FOR;
}

void
Cell::CellFindModifiedHPWL(ulong &oldXHPWL, ulong &oldYHPWL,
			   ulong &newXHPWL, ulong &newYHPWL)
{
  Net *netPtr;
  Pin *pinPtr, *connPinPtr;
  Pin *pinMaxx, *pinMaxy, *pinMinx, *pinMiny;
  Cell *cellPtr;
  map<Net*, bool> visitedNets;
  double pinAbsX, pinAbsY;
  double maxx, maxy, minx, miny;
  
  oldXHPWL = 0; oldYHPWL = 0;
  newXHPWL = 0; newYHPWL = 0;
  maxx = 0; maxy = 0; minx = DBL_MAX; miny = DBL_MAX;
  VECTOR_FOR_ALL_ELEMS(Pins, Pin*, pinPtr) {
    netPtr = (*pinPtr).ConnectedNet;
    _KEY_EXISTS(visitedNets, netPtr) {
      continue;
    }
    visitedNets[netPtr] = true;
    oldXHPWL += (netPtr->maxx - netPtr->minx);
    oldYHPWL += (netPtr->maxy - netPtr->miny);
    //    (*netPtr).NetComputeHPWL();
    //    continue;
    NET_FOR_ALL_PINS((*netPtr), connPinPtr) {
      cellPtr = (*connPinPtr).ParentCell;
      pinAbsX = cellPtr->x + connPinPtr->xOffset;
      pinAbsY = cellPtr->y + connPinPtr->yOffset;
      if (pinAbsX > maxx) {
	maxx = pinAbsX; 
	netPtr->maxx = pinAbsX;
	pinMaxx = connPinPtr;
      } 
      if (pinAbsX < minx) {
	minx = pinAbsX; 
	netPtr->minx = pinAbsX;
	pinMinx = connPinPtr;
      }
      if (pinAbsY > maxy) {
	maxy = pinAbsY; 
	netPtr->maxy = pinAbsY;
	pinMaxy = connPinPtr;
      }
      if (pinAbsY < miny) {
	miny = pinAbsY; 
	netPtr->miny = pinAbsY;
	pinMiny = connPinPtr;
      }
    } NET_END_FOR;
    newXHPWL += (netPtr->maxx - netPtr->minx);
    newYHPWL += (netPtr->maxy - netPtr->miny);
  } END_FOR;
}

void
Cell::CellMoveCellComputeHPWL(double newXpos, double newYpos,
			      ulong &oldXHPWL, ulong &oldYHPWL,
			      ulong &newXHPWL, ulong &newYHPWL)
{
  Pin *pinPtr;
  Net *netPtr;
  double oldXpos, oldYpos;
  double oldPinAbsX, oldPinAbsY;
  double newPinAbsX, newPinAbsY;
  double maxx, maxy, minx, miny;
  bool incx, decx, incy, decy;
  bool recompute;
  
  incx = false; decx = false;
  if (newXpos > x) { 
    incx = true; 
  } else if (newXpos < x) {
    decx = true;
  }
  incy = false; decy = false;
  if (newYpos > y) { 
    incy = true; 
  } else if (newYpos < y) {
    decy = true;
  }

  oldXHPWL = 0; oldYHPWL = 0;
  newXHPWL = 0; newYHPWL = 0;
  oldXpos = x; oldYpos = y;
  x = newXpos; y = newYpos;
  VECTOR_FOR_ALL_ELEMS(Pins, Pin*, pinPtr) {
    recompute = false;
    Pin &thisPin = *pinPtr;
    Net &thisNet = *((*pinPtr).ConnectedNet);
    oldXHPWL += thisNet.maxx - thisNet.minx;
    oldYHPWL += thisNet.maxy - thisNet.miny;
    if (incx == true) {
      oldPinAbsX = oldXpos + thisPin.xOffset;
      newPinAbsX = newXpos + thisPin.xOffset;
      if (thisNet.pinMaxx == pinPtr) {
	thisNet.maxx = newPinAbsX;
      } else if (thisNet.maxx < newPinAbsX) {
	thisNet.maxx = newPinAbsX;
	thisNet.pinMaxx = pinPtr;
      } else if (thisNet.pinMinx == pinPtr) {
	recompute = true;
      }
    } else if (decx == true && (!recompute)) {
      oldPinAbsX = oldXpos + thisPin.xOffset;
      newPinAbsX = newXpos + thisPin.xOffset;
      if (thisNet.pinMinx == pinPtr) {
	thisNet.minx = newPinAbsX;
      } else if (thisNet.minx > newPinAbsX) {
	thisNet.minx = newPinAbsX;
	thisNet.pinMinx = pinPtr;
      } else if (thisNet.pinMaxx == pinPtr) {
	recompute = true;
      }
    }
    if (incy == true && (!recompute)) {
      oldPinAbsY = oldYpos + thisPin.yOffset;
      newPinAbsY = newYpos + thisPin.yOffset;
      if (thisNet.pinMaxy == pinPtr) {
	thisNet.maxy = newPinAbsY;
      } else if (thisNet.maxy < newPinAbsY) {
	thisNet.maxy = newPinAbsY;
	thisNet.pinMaxy = pinPtr;
      } else if (thisNet.pinMiny == pinPtr) {
	recompute = true;
      }
    } else if (decy == true && (!recompute)) {
      oldPinAbsY = oldYpos + thisPin.yOffset;
      newPinAbsY = newYpos + thisPin.yOffset;
      if (thisNet.pinMiny == pinPtr) {
	thisNet.miny = newPinAbsY;
      } else if (thisNet.miny > newPinAbsY) {
	thisNet.miny = newPinAbsY;
	thisNet.pinMiny = pinPtr;
      } else if (thisNet.pinMaxy == pinPtr) {
	recompute = true;
      }
    }
    if (recompute) {
      thisNet.maxx = 0; thisNet.maxy = 0; 
      thisNet.minx = INT_MAX; thisNet.miny = INT_MAX;
      double cellx, celly;
      NET_FOR_ALL_PINS(thisNet, pinPtr) {
	cellx = (*pinPtr).ParentCell->x;
	celly = (*pinPtr).ParentCell->y;
	newPinAbsX = (*pinPtr).xOffset + cellx;
	newPinAbsY = (*pinPtr).yOffset + celly;
	if (newPinAbsX > thisNet.maxx) {
	  thisNet.pinMaxx = pinPtr;
	  thisNet.maxx = newPinAbsX;
	}
	if (newPinAbsX < thisNet.minx) {
	  thisNet.minx = newPinAbsX;
	  thisNet.pinMinx = pinPtr;
	}
	if (newPinAbsY > thisNet.maxy) {
	  thisNet.maxy = newPinAbsY;
	  thisNet.pinMaxy = pinPtr;
	}
	if (newPinAbsY < thisNet.miny) {
	  thisNet.miny = newPinAbsY;
	  thisNet.pinMiny = pinPtr;
	}
      } NET_END_FOR;
    }
  } END_FOR;
}


uint 
Cell::CellGetXpos(void)
{
  return (dtoi(x));
}

double
Cell::CellGetOldXpos(void)
{
  return (oldx);
}

double
Cell::CellGetXposDbl(void)
{
  return (x);
}

uint 
Cell::CellGetRight(void)
{
  return (x + width);
}

uint 
Cell::CellGetYpos(void)
{
  return (dtoi(y));
}

double
Cell::CellGetOldYpos(void)
{
  return (oldy);
}

double 
Cell::CellGetYposDbl(void)
{
  return (y);
}

uint 
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
Cell::CellIsClusterFixed(void)
{
  return (isClusterFixed);
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
Cell::CellIsHidden(void)
{
  return (isHidden);
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

bool
Cell::CellIsAdjacentToPort(void)
{
  return adjacentToPort;
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

vector<Pin *>&
Cell::CellGetPins(void)
{
  return (this->Pins);
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

Bin*
Cell::CellGetBin(void)
{
  return (cellBin);
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, objOrient Orientation, string Name, bool terminalCell)
{
  CellSetPos((uint)Xpos, (uint)Ypos);
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
  CellSetIsClusterFixed(false);
  CellSetIsSequential(false);
  CellSetIsPort(false);
  CellSetIsHidden(false);
  CellSetNumInPins(0);
  CellSetNumOutPins(0);
  CellSetClusterLevel(0);
  CellSetXIsLegal(false);
  CellSetYIsLegal(false);
  CellSetAdjacentToPort(false);
  CellSetBin(NIL(Bin*));
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

