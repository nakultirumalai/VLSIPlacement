# ifndef CELL_H
# define CELL_H

# include <common.h>
# include <CellIter.h>

class Pin;
class Net;
class Bin;
using namespace std;

class Cell {
 private:
  ulong flags;
  double oldx;
  double oldy;
  int height, width;
  int numPins, numInPins, numOutPins;
  uint clusterLevel;
  objOrient orient;
  bool terminalCell;
  bool isClustered, isCluster, isClusterChild;
  bool isPort, isMacro, isFixed;
  bool isSequential;
  bool isXLegal, isYLegal;
  bool isHidden;
  vector<Pin*> Pins;
  vector<Cell*> childCells;
  map<uint, map<uint, double > > arcDelays;

  string name;
  string origName;
  string libCellName;
  string libName;

 public:
  double x, y;
  Bin *cellBin;

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
  void CellSetXpos(uint);
  void CellSetOldXpos(double);
  void CellSetXposDbl(double);
  void CellSetOldYPos(double);
  void CellSetYpos(uint);
  void CellSetOldYpos(double);
  void CellSetYposDbl(double);
  void CellSetOldPos(double, double);
  void CellSetPosDbl(double, double);
  void CellSetPos(uint, uint);
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
  void CellSetClusterLevel(uint);
  void CellSetIsSequential(const bool &);
  void CellSetIsPort(const bool &);
  void CellSetIsHidden(const bool &);
  void CellSetXIsLegal(const bool&);
  void CellSetYIsLegal(const bool&);
  void CellMarkNetsDirty(void);
  void CellAddChildCell(Cell &thisCell);
  void CellAddPin(Pin *);
  void CellAddArcDelay(Pin *, Pin *, double);
  void CellSetBin(Bin *);
  void CellIncrementClusterLevel(void);
  void CellDecrementClusterLevel(void);

  /* Get functions */
  uint CellGetXpos(void);
  double CellGetXposDbl(void);
  double CellGetOldXpos(void);
  uint CellGetRight(void);
  uint CellGetYpos(void);
  double CellGetOldYpos(void);
  double CellGetYposDbl(void);
  uint CellGetTop(void);
  void CellGetPos(uint &, uint &);
  void CellGetBoundingBox(uint &, uint &, uint&, uint&);
  int CellGetHeight(void);
  int CellGetWidth(void);
  int CellGetNumPins(int);
  int CellGetNumPins(void);
  uint CellGetClusterLevel(void);
  objOrient CellGetOrientation(void);
  uint CellGetArea(void);
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
  Bin *CellGetBin(void);

  /* Other functions */
  void CellMoveRight(int);
  void CellMoveLeft(int);
  void CellMoveUp(int);
  void CellMoveDown(int);
  void CellMoveCell(double, double);
  void CellMoveCellComputeHPWL(double, double, ulong &, ulong &, 
			       ulong &, ulong &);
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

