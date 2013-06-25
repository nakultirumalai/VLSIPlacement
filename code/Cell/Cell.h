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
  bool isHidden;
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
  void CellSetIsHidden(const bool &);
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
  void CellGetPos(uint &, uint &);
  void CellGetBoundingBox(uint &, uint &, uint&, uint&);
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
  bool CellIsHidden(void);
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

class Cluster {
 private:
  /* Requires N * 64-bit + VECT_CONTAINER_OVERHEAD */
  vector<Cell *> cellsOfCluster; 
  /* Requires N * 64-bit + VECT_CONTAINER_OVERHEAD */
  vector<Net *> internalNets;
  /* Requires N * 32-bit + VECT_CONTAINER_OVERHEAD */
  vector<uint> BCellIndices; 
  vector<uint> rowNum;
  vector<uint> xPosInRow;
  /* Requires N * 128-bit + MAP_CONTAINER_OVERHEAD */
  map<Pin *, Pin *> pinMap;
  /* Indicate if the boundary cells are placed */
  bool bCellsPlaced;
  /* Indicate the level of the cluster */
  char clusterLevel;

 public:
  uint numCells;
  
  /* Constructor and destructor */
  Cluster();
  Cluster(vector<Cell *>&);
  Cluster(vector<Cell *>&, vector<uint>&, vector<uint>&, 
	  vector<uint>&, map<Pin*, Pin*>&);
  ~Cluster();
  
  /* Set functions */
  void ClusterSetNumCells(uint);
  void ClusterSetCellsOfCluster(vector<Cell *>&);
  void ClusterSetInternalNets(vector<Net *> &);
  void ClusterSetBCellIndices(vector<uint> &);
  void ClusterSetRowNums(vector<uint> &);
  void ClusterSetXPosInRows(vector<uint> &);
  void ClusterSetPinMap(map<Pin *, Pin *> &);
  void ClusterSetBCellsPlaced(bool);
  void ClusterSetClusterLevel(char);

  /* Get functions */
  uint ClusterGetNumCells(void);
  vector<Cell *>& ClusterGetCellsOfCluster(void);
  vector<Net *>& ClusterGetInternalNets(void);
  vector<uint>& ClusterGetBCellIndices(void);
  vector<uint>& ClusterGetRowNums(void);
  vector<uint>& ClusterGetXPosInRows(void);
  map<Pin*, Pin*>& ClusterGetPinMap(void);
  bool ClusterGetBCellsPlaced(void);
  char ClusterGetClusterLevel(void);
  
  /* Debug functions */
  void PlotCluster(void);
  void PrintCluster(void);
  
};
# endif

