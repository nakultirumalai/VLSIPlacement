# include <Cell.h>

inline void 
Cell::CellSetXpos(int Xpos) 
{
  x = Xpos;
}

inline void
Cell::CellSetYpos(int Ypos)
{
  y = Ypos;
}

inline void
Cell::CellSetPos(int Xpos, int Ypos) 
{
  x = Xpos;
  y = Ypos;
}

inline void
Cell::CellSetHeight(int Height) 
{
  height = Height;
}

inline void
Cell::CellSetWidth(int Width)
{
  width = Width;
}

inline void
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
Cell::CellSetIsCluster(const bool& isCluster)
{
  this->isCluster = isCluster;
}

inline void
Cell::CellMoveRight(int offset)
{
  x += offset;
}

inline void
Cell::CellMoveLeft(int offset)
{
  x -= offset;
}

inline void
Cell::CellMoveUp(int offset)
{
  y += offset;
}

inline void
Cell::CellMoveDown(int offset)
{
  y -= offset;
}

inline void
Cell::CellMoveCell(int XOffset, int YOffset)
{
  x = x + XOffset;
  y = y + YOffset;
}

inline int 
Cell::CellGetXpos(void)
{
  return (x);
}

inline int 
Cell::CellGetYpos(void)
{
  return (y);
}

inline int 
Cell::CellGetHeight(void)
{
  return (height);
}

inline int 
Cell::CellGetWidth(void)
{
  return (width);
}

inline int 
Cell::CellGetNumPins(void)
{
  return (numPins);
}

inline int 
Cell::CellGetOrientation(void)
{
  return (orientation);
}

inline bool 
Cell::CellGetTerminal(void)
{
  return (terminalCell);
}

inline bool
Cell::CellGetIsCluster(void)
{
  return (isCluster);
}

string
Cell::CellGetName(void)
{
  return name;
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
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(CELL_ORIENTATION_ZERO_DEG);
  CellSetTerminal(false);
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
}

Cell::Cell(int Height, int Width, int Xpos, int Ypos, char Orientation)
{
  CellSetPos(Xpos, Ypos);
  CellSetHeight(Height);
  CellSetWidth(Width);
  CellSetNumPins(0);
  CellSetOrientation(Orientation);
  CellSetTerminal(false);
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
}
