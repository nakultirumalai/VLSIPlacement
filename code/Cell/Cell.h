# ifndef CELL_H
# define CELL_H

# include <common.h>
# include <Pin.h>
# include <CellMacros.h>

using namespace std;

class Cell {
 private:
  int x;
  int y;
  int height;
  int width;
  int numPins;
  int numInPins;
  int numOutPins;
  objOrient orient;
  bool terminalCell;
  bool isCluster;
  bool isMacro;
  bool isFixed;
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
  Cell(int, int, int, int, objOrient);
  Cell(int, int, int, int, objOrient, string);
  Cell(int, int, int, int, objOrient, string, bool);

  /* Set functions */
  void CellSetXpos(int);
  void CellSetYpos(int);
  void CellSetPos(int, int);
  void CellSetHeight(int);
  void CellSetWidth(int);
  void CellSetOrientation(objOrient);
  void CellSetName(const string &);
  void CellSetNumPins(int);
  void CellSetNumInPins(int);
  void CellSetNumOutPins(int);
  void CellSetIsTerminal(const bool&);
  void CellSetIsCluster(const bool&);
  void CellSetIsMacro(const bool &);
  void CellSetIsFixed(const bool &);
  void CellAddPin(Pin *);
  
  /* Get functions */
  int CellGetXpos(void);
  int CellGetYpos(void);
  int CellGetHeight(void);
  int CellGetWidth(void);
  int CellGetNumPins(int);
  int CellGetNumPins(void);
  objOrient CellGetOrientation(void);
  unsigned int CellGetArea(void);
  bool CellIsTerminal(void);
  bool CellIsCluster(void);
  bool CellIsMacro(void);
  bool CellIsFixed(void);
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

