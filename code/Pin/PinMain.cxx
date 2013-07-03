# include <Pin.h>
# include <Cell.h>
# include <common.h>

/* Set functions */
void
Pin::PinSetId(int id)
{
  Id = id;
}

void 
Pin::PinSetName(string pinName)
{
  Name = pinName;
}

void 
Pin::PinSetLibName(string libPinName)
{
  this->libPinName = libPinName;
}
  
void 
Pin::PinSetParentCell(const Cell& parentCell)
{
  ParentCell = (Cell *) &parentCell;
}

void
Pin::PinSetXOffset(int xoffset)
{
  xOffset = xoffset;
}

void 
Pin::PinSetYOffset(int yoffset)
{
  yOffset = yoffset;
}

void
Pin::PinSetDirection(char direction)
{
  dir = direction;
}

void
Pin::PinSetIsClock(const bool &isClock)
{
  this->isClock = isClock;
}

void
Pin::PinSetIsHidden(const bool &isHidden)
{
  this->isHidden = isHidden;
}

/* Get functions */
int 
Pin::PinGetId(void)
{
  return (Id);
}

string 
Pin::PinGetName(void) const
{
  return (Name);
}

string 
Pin::PinGetLibName(void) const
{
  return (libPinName);
}

int
Pin::PinGetXOffset(void)
{
  return (xOffset);
}

int 
Pin::PinGetYOffset(void)
{
  return (yOffset);
}

char
Pin::PinGetDirection(void) const
{
  return (dir);
}

int 
Pin::PinGetAbsXPos(void) const
{
  return ((*ParentCell).CellGetXposDbl() + xOffset);
}

int 
Pin::PinGetAbsYPos(void) const
{
  return ((*ParentCell).CellGetYposDbl() + yOffset);
}

Cell& 
Pin::PinGetParentCell(void)
{
  Cell &parentCell = *ParentCell;
  
  return (parentCell);
}

Cell*
Pin::PinGetParentCellPtr(void)
{
  Cell *parentCell = ParentCell;
  
  return (parentCell);
}

Net& 
Pin::PinGetNet(void)
{
  Net &connectedNet = *ConnectedNet;
  
  return (connectedNet);
}

bool
Pin::PinIsClock(void)
{
  return (isClock);
}

bool
Pin::PinIsHidden(void)
{
  return (isHidden);
}

/* Other functions */
void 
Pin::Connect(const Net& netToConnect)
{
  ConnectedNet = (Net *) &netToConnect;
}

Net& 
Pin::Disconnect(void)
{
  ConnectedNet = NULL;
}

void 
Pin::PinGetXposYpos(int *xpos, int *ypos)
{
  *xpos = xOffset;
  *ypos = yOffset;
}


Pin::Pin() 
{
  PinSetId(0);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id) 
{
  PinSetId(id);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id, int xoffset, int yoffset) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id, int xoffset, int yoffset, char direction) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id, const string& name) 
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, const string& name) 
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetDirection((char)PIN_DIR_INPUT);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, char direction, const string& name)
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, const Cell& parentCell) 
{
  PinSetId(id);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, const Cell& parentCell) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, char direction, const Cell& parentCell) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, const Cell& parentCell, const string& name) 
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, const Cell& parentCell, 
	 const string& name) 
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, char direction, 
	 const Cell& parentCell, const string& name) 
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, const Cell& parentCell, const Net& connectedNet)
{
  PinSetId(id);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, const Cell& parentCell, 
	 const Net& connectedNet)
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  Connect(connectedNet);
}

Pin::Pin(int id, int xoffset, int yoffset, char direction, 
	 const Cell& parentCell, const Net& connectedNet)
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
  PinSetIsClock(false);
  PinSetIsHidden(false);
  PinSetParentCell(parentCell);
  Connect(connectedNet);
}

string
Pin::GetParentCellName()
{
  return ((*ParentCell).CellGetName());
}

uint
Pin::GetParentCellXpos()
{
  return ((*ParentCell).CellGetXpos());
}

uint
Pin::GetParentCellYpos()
{
  return ((*ParentCell).CellGetYpos());
}
