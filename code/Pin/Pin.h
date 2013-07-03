# include <common.h>

# ifndef PIN_H
# define PIN_H

/* Pin direction definitions */
# define PIN_DIR_INPUT 0x1
# define PIN_DIR_OUTPUT 0x1 << 1
# define PIN_DIR_INOUT 0x1 << 2
# define PIN_DIR_ALL 0x1 << 3


class Cell;
class Net;

class Pin {
 private:
  int Id;
  double cellXpos, cellYpos;
  char dir; 
  bool isClock;
  bool isHidden;
  string libPinName;
  string Name;
 public:
  Cell *ParentCell;
  Net *ConnectedNet;
  int xOffset, yOffset;

  /* Constructors */
  Pin();
  Pin(int);
  Pin(char);

  Pin(int, int, int);
  Pin(int, int, int, char);

  Pin(int, const string&);
  Pin(int, int, int, const string&);
  Pin(int, int, int, char, const string&);

  Pin(int, const Cell&);
  Pin(int, int, int, const Cell&);
  Pin(int, int, int, char, const Cell&);

  Pin(int, const Cell&, const string&);
  Pin(int, int, int, const Cell&, const string&);
  Pin(int, int, int, char, const Cell&, const string&);

  Pin(int, const Cell&, const Net&);
  Pin(int, int, int, const Cell&, const Net&);
  Pin(int, int, int, char, const Cell&, const Net&);

  /* Set functions */
  void PinSetId(int);
  void PinSetName(string);
  void PinSetLibName(string);
  void PinSetParentCell(const Cell&);
  void PinSetXOffset(int);
  void PinSetYOffset(int);
  void PinSetDirection(char);
  void PinSetIsClock(const bool&);
  void PinSetIsHidden(const bool&);

  /* Get functions */
  int PinGetId(void);
  int PinGetXOffset(void);
  int PinGetYOffset(void);
  int PinGetAbsXPos(void) const;
  int PinGetAbsYPos(void) const;
  char PinGetDirection(void) const;
  string PinGetName(void) const;
  string PinGetLibName(void) const;
  Cell& PinGetParentCell(void);
  Cell* PinGetParentCellPtr(void);
  Net& PinGetNet(void);
  bool PinIsClock(void);
  bool PinIsHidden(void);

  /* Other functions */
  void Connect(const Net&);
  Net& Disconnect(void);
  void PinGetXposYpos(int *, int*);

  /* To be removed later */
  string GetParentCellName();
  uint GetParentCellXpos();
  uint GetParentCellYpos();
};

# endif
