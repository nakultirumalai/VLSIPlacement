# ifndef CELL_H
# define CELL_H

# include <common.h>
# include <Pin.h>
# include <CellMacros.h>

# define CELL_ORIENTATION_ZERO_DEG 0x1
# define CELL_ORIENTATION_NINETY_DEG 0x1 << 1
# define CELL_ORIENTATION_ONE_EIGHTY_DEG 0x1 << 2
# define CELL_ORIENTATION_TWO_SEVENTY_DEG 0x1 << 3

using namespace std;

class Cell {
 private:
  int x;
  int y;
  int height;
  int width;
  int numPins;
  char orientation;
  bool terminalCell;
  bool isCluster;
  vector<Pin*> Pins;

 public:
  string name;
  /* Constructor & Destructor */
  Cell(int, int);
  Cell(int, int, string);
  Cell(int, int, string, bool);  
  Cell(int, int, int, int);
  Cell(int, int, int, int, string);
  Cell(int, int, int, int, string, bool);
  Cell(int, int, int, int, char);
  Cell(int, int, int, int, char, string);
  Cell(int, int, int, int, char, string, bool);

  /* Set functions */
  void CellSetXpos(int);
  void CellSetYpos(int);
  void CellSetPos(int, int);
  void CellSetHeight(int);
  void CellSetWidth(int);
  void CellSetOrientation(int);
  void CellSetName(const string &);
  void CellSetTerminal(const bool& terminalCell);
  void CellSetNumPins(int);
  void CellSetIsCluster(const bool& isCluster);
  
  /* Get functions */
  int CellGetXpos(void);
  int CellGetYpos(void);
  int CellGetHeight(void);
  int CellGetWidth(void);
  int CellGetNumPins(int);
  int CellGetNumPins(void);
  int CellGetOrientation(void);
  unsigned int CellGetArea(void);
  bool CellGetTerminal(void);
  bool CellGetIsCluster(void);
  string CellGetName(void);
  vector<Pin*> CellGetPins(int);
  vector<Pin*> CellGetPins(void);

  /* Other functions */
  void CellMoveRight(int);
  void CellMoveLeft(int);
  void CellMoveUp(int);
  void CellMoveDown(int);
  void CellMoveCell(int, int);
};

# endif

