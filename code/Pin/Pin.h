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
  int xOffset;
  int yOffset;
  char dir; 
  Cell *ParentCell;
  Net *ConnectedNet;
 public:
  string Name;

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
  void PinSetParentCell(const Cell&);
  void PinSetXOffset(int);
  void PinSetYOffset(int);
  void PinSetDirection(char);

  /* Get functions */
  int PinGetId(void);
  int PinGetXOffset(void);
  int PinGetYOffset(void);
  char PinGetDirection(void) const;
  string PinGetName(void) const;
  Cell& PinGetParentCell(void);
  Net& PinGetNet(void);
  
  /* Other functions */
  void Connect(const Net&);
  Net& Disconnect(void);
  void PinGetXposYpos(int *, int*);
};

# endif
