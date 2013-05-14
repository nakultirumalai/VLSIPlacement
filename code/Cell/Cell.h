# ifndef CELL_H
# define CELL_H

# include <common.h>
# include <Pin.h>
# include <CellIter.h>

using namespace std;

class Cell {
 private:
  unsigned long flags;
  int x;
  int y;
  int height;
  int width;
  int numPins;
  int numInPins;
  int numOutPins;
  unsigned int clusterLevel;
  objOrient orient;
  bool terminalCell;
  bool isClustered;
  bool isCluster;
  bool isClusterChild;
  bool isMacro;
  bool isFixed;
  bool isSequential;
  bool isPort;
  bool isXLegal;
  bool isYLegal;
  vector<Pin*> Pins;
  vector<Cell*> childCells;
  map<unsigned int, map<unsigned int, double > > arcDelays;

  string name;
  string origName;
  string libCellName;
  string libName;

 public:
  /* Constructor & Destructor */
  Cell();
  Cell(int, int);
  Cell(int, int, string);
  Cell(int, int, string, bool);  
  Cell(int, int, int, int);
  Cell(int, int, int, int, string);
  Cell(int, int, int, int, string, bool);
  Cell(int, int, int, int, objOrient);
  Cell(int, int, int, int, objOrient, string);
  Cell(int, int, int, int, objOrient, string, bool);
  ~Cell();

  /* Set functions */
  void CellSetXpos(int);
  void CellSetYpos(int);
  void CellSetPos(int, int);
  void CellSetHeight(int);
  void CellSetWidth(int);
  void CellSetOrientation(objOrient);
  void CellSetName(const string &);
  void CellSetOrigName(const string &);
  void CellSetLibCellName(const string &);
  void CellSetLibName(const string &);
  void CellSetNumPins(int);
  void CellSetNumInPins(int);
  void CellSetNumOutPins(int);
  void CellSetIsTerminal(const bool&);
  void CellSetIsCluster(const bool&);
  void CellSetIsClusterChild(const bool&);
  void CellSetIsClustered(const bool&);
  void CellSetIsMacro(const bool &);
  void CellSetIsFixed(const bool &);
  void CellSetClusterLevel(unsigned int);
  void CellSetIsSequential(const bool &);
  void CellSetIsPort(const bool &);
  void CellSetXIsLegal(const bool&);
  void CellSetYIsLegal(const bool&);
  void CellAddChildCell(Cell &thisCell);
  void CellAddPin(Pin *);
  void CellAddArcDelay(Pin *, Pin *, double);
  void CellIncrementClusterLevel(void);
  void CellDecrementClusterLevel(void);

  /* Get functions */
  int CellGetXpos(void);
  int CellGetRight(void);
  int CellGetYpos(void);
  int CellGetTop(void);
  int CellGetHeight(void);
  int CellGetWidth(void);
  int CellGetNumPins(int);
  int CellGetNumPins(void);
  unsigned int CellGetClusterLevel(void);
  objOrient CellGetOrientation(void);
  unsigned int CellGetArea(void);
  bool CellIsTerminal(void);
  bool CellIsCluster(void);
  bool CellIsClusterChild(void);
  bool CellIsClustered(void);
  bool CellIsMacro(void);
  bool CellIsFixed(void);
  bool CellIsSequential(void);
  bool CellIsPort(void);
  bool CellXIsLegal(void);
  bool CellYIsLegal(void);
  string CellGetName(void);
  string CellGetOrigName(void);
  string CellGetLibCellName(void);
  string CellGetLibName(void);
  vector<Pin*> CellGetPins(int);
  vector<Pin*> CellGetPins(void);
  vector<Cell*> CellGetChildCells(void);
  double CellGetArcDelay(Pin *, Pin *);

  /* Other functions */
  void CellMoveRight(int);
  void CellMoveLeft(int);
  void CellMoveUp(int);
  void CellMoveDown(int);
  void CellMoveCell(int, int);
  Pin* CellGetPinByName(const string &PinName);

};

# endif

