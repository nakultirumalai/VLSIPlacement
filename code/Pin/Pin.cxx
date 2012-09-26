# include <Pin.h>
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
Pin::PinGetDirection(void)
{
  return (dir);
}

Cell& 
Pin::PinGetParentCell(void)
{
  Cell &parentCell = *ParentCell;
  
  return (parentCell);
}

Net& 
Pin::PinGetNet(void)
{
  Net &connectedNet = *ConnectedNet;
  
  return (connectedNet);
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
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id) 
{
  PinSetId(id);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id, int xoffset, int yoffset) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection((char)PIN_DIR_INPUT);
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}


Pin::Pin(int id, int xoffset, int yoffset, char direction) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
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
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, const string& name) 
{
  PinSetId(id);
  PinSetName(name);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
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
  _setNULL(ParentCell, Cell*);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, const Cell& parentCell) 
{
  PinSetId(id);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, const Cell& parentCell) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection((char)PIN_DIR_INPUT);
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, int xoffset, int yoffset, char direction, const Cell& parentCell) 
{
  PinSetId(id);
  PinSetXOffset(xoffset);
  PinSetYOffset(yoffset);
  PinSetDirection(direction);
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
  PinSetParentCell(parentCell);
  _setNULL(ConnectedNet, Net*);
}

Pin::Pin(int id, const Cell& parentCell, const Net& connectedNet)
{
  PinSetId(id);
  PinSetXOffset(0);
  PinSetYOffset(0);
  PinSetDirection((char)PIN_DIR_INPUT);
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
  PinSetParentCell(parentCell);
  Connect(connectedNet);
}

