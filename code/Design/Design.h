# ifndef DESIGN_H
# define DESIGN_H

# include <stdlib.h>
# include <common.h>
# include <Cell.h>
# include <Net.h>
# include <Pin.h>
# include <HyperGraph.h>
# include <PhysRow.h>
# include <Sort.h>
# include <Flags.h>
# include <DesignIter.h>
# include <mosek.h>
# include <TimingModels.h>
# include <CellSpread.h>
# include <Path.h>
# include <Bin.h>
# include <Plot.h>
# include <Env.h>
# include <ConjGradSolver.h>
# include <PriorityQueue.h>

# define MAX_PATHS 1000000

/*******************************************************************************
  Bookshelf format definitions
*******************************************************************************/
/*******************************************************************************
  Extensions of different files to be read
*******************************************************************************/
# define DESIGN_AUX_FILE_EXT ".aux"
# define DESIGN_CELL_FILE_EXT ".nodes"
# define DESIGN_NET_FILE_EXT ".nets"
# define DESIGN_NET_WTS_FILE_EXT ".wts"
# define DESIGN_SCL_FILE_EXT ".scl"
# define DESIGN_PL_FILE_EXT ".pl"
# define DESIGN_NODES_MAP_FILE_EXT ".nodes.map"
# define DESIGN_CMDS_FILE_EXT ".cmds"
# define DESIGN_PINS_MAP_FILE_EXT ".pins.map"
# define DESIGN_CELL_DELAYS_FILE_EXT ".nodes.delays"
# define DESIGN_PATH_DELAYS_FILE_EXT ".timing_paths"

/*******************************************************************************
  Number of lines in the header of each type of file
*******************************************************************************/
# define NUM_NODEFILE_HEADER_LINES 4
# define NUM_NETFILE_HEADER_LINES 4
# define NUM_SCLFILE_HEADER_LINES 4

/*******************************************************************************
  Number of properties before reading objects in the file
*******************************************************************************/
# define NUM_NODE_PROPERTIES 2
# define NUM_NET_PROPERTIES 2
# define NUM_ROW_PROPERTIES 1
# define NUM_FIXED_CELL_PROPERTIES 0

/*******************************************************************************
  Type of placement defined in the AUX file
*******************************************************************************/
# define ROW_BASED_PLACEMENT "RowBasedPlacement"
# define STD_CELL_BASED_PLACEMENT "StdCellBasedPlacement"

/*******************************************************************************
  Properties of the nodes file
*******************************************************************************/
# define NUM_NODES_PROPERTY "NumNodes"
# define NUM_TERMINALS_PROPERTY "NumTerminals"
# define NODE_TERMINAL_KEYWORD "terminal"

/*******************************************************************************
  Properties of the nets file
*******************************************************************************/
# define NUM_NETS_PROPERTY "NumNets"
# define NUM_PINS_PROPERTY "NumPins"
# define NET_DEGREE_PROPERTY "NetDegree"

/*******************************************************************************
  Properties of the scl file
*******************************************************************************/
# define NUM_ROWS_PROPERTY "NumRows"

/*******************************************************************************
  Properties of each row defined in the SCL file
*******************************************************************************/
# define ROW_BEGIN_KEYWORD "CoreRow"
# define ROW_COORDINATE "Coordinate"
# define ROW_HEIGHT "Height"
# define ROW_SITE_WIDTH "Sitewidth"
# define ROW_SITE_SPACING "Sitespacing"
# define ROW_SITE_ORIENTATION "Siteorient"
# define ROW_SITE_SYMMETRY "Sitesymmetry"
# define SUBROW_ORIGIN "SubrowOrigin"
# define SUBROW_NUM_SITES "NumSites"
# define ROW_END_KEYWORD "End"

# define PIN_DIR_INPUT_STRING "I"
# define PIN_DIR_OUTPUT_STRING "O"

# define DIR_SEP "/"

# define ZERO '0'
# define NINE '9'

/*******************************************************************************
  Cluster name prefix for numbering
*******************************************************************************/
# define CLUSTER_NAME_PREFIX "oClust"

/*******************************************************************************
  Define the reference number to be considered to classify a net as high fanout
*******************************************************************************/
# define HIGH_FANOUT 20

/*******************************************************************************
  DEFINITIONS FOR FORCES
*******************************************************************************/
# define FORCE_DIR_NO_FORCE 0
# define FORCE_DIR_FIRST_QUAD 1
# define FORCE_DIR_SECOND_QUAD 2
# define FORCE_DIR_THIRD_QUAD 3
# define FORCE_DIR_FOURTH_QUAD 4
# define FORCE_DIR_RIGHT 5
# define FORCE_DIR_LEFT 6
# define FORCE_DIR_TOP 7
# define FORCE_DIR_BOT 8
# define MAX_DBL 1000000000000000

/*******************************************************************************
  Type definitions for the design class
*******************************************************************************/
typedef enum {
  DEFAULT_CLUSTER, 
  FC_CLUSTER,
  NET_CLUSTER,
  ESC_CLUSTER,
  TOTAL_CLUSTERING_TECHNIQUES,
} clusteringType;

/*******************************************************************************
  A class for storing object pair information for the clustering algorithm
*******************************************************************************/
template<class T>
class objPairScore {
 private: 
  double score;
  T obj1, obj2;
 public:
  objPairScore() 
  {
  }
  objPairScore(T obj1,T obj2, double score) 
  {
    this->obj1 = obj1;
    this->obj2 = obj2;
    this->score = score;
  }
  ~objPairScore() {
  }
  double GetScore() const
  {
    return score;
  }
  double GetObjs(T &obj1, T &obj2) 
  {
    obj1 = this->obj1;
    obj2 = this->obj2;
  }
};

class objPairScoreCompare {
 public:
  bool operator() (const objPairScore<Cell *> &pair1,
                   const objPairScore<Cell *> &pair2) const
  {
    return (pair1.GetScore() < pair2.GetScore());
  }
};

class objPairScoreCompareUint {
 public:
  bool operator() (const objPairScore<uint> &pair1,
                   const objPairScore<uint> &pair2) const
  {
    return (pair1.GetScore() < pair2.GetScore());
  }
};

class scoreCmpType {
 public:
  bool operator() (const pair<Cell *, double> &pair1,
                   const pair<Cell *, double> &pair2) const
  {
    double score1 = pair1.second;
    double score2 = pair2.second;

    return (score1 < score2);
  }
};

class scoreCmpTypeUint {
 public:
  bool operator() (const pair<uint, double> &pair1,
                   const pair<uint, double> &pair2) const
  {
    return (pair1.second < pair2.second);
  }
};

class Design {
 private:
  Env DesignEnv;
  map<uint, uint>RowHeights;
  HyperGraph *DesignGraphPtr;
  uint NumCells;
  uint NumClustersSeenSofar;
  uint NumClusters;
  uint NumStdCells;
  uint NumMacroCells;
  uint NumFixedCells;
  uint NumTerminalCells;
  uint NumHiddenCells;
  uint NumNets;
  uint NumPins;
  uint NumPaths;
  uint NumTopNets;
  uint NumPhysRows;
  uint singleRowHeight;
  int maxx, maxy;
  int preShiftLeft, preShiftBot, preShiftRight, preShiftTop;

  string Name;
  string DesignPath;
  string DesignCellFileName;
  string DesignNetFileName;
  string DesignNetWtsFileName;
  string DesignSclFileName;
  string DesignPlFileName;
  string DesignMapFileName;
  string DesignCmdsFileName;
  string DesignPinsMapFileName;
  string DesignCellDelaysFileName;
  string DesignPathDelaysFileName;

  /* Stores clock period in nanoseconds */
  double clockPeriod;
  bool RowBasedPlacement;

  /* Generic input file stream from which all
     design related data is read */
  ifstream DesignFile;

  /* Bin and utilization related stuff */
  uint peakUtilizationBinIdx;
  uint numBinRows, numBinCols;
  double peakUtilization;

  /* Other performance related parameters */
  double hpwl;

  /* Average cell width */
  double averageStdCellHeight, averageStdCellWidth;

  void DesignSetVarsPostRead(void);

  /* Add functions */
  void DesignInit(void);
  void DesignFileReadHeader(ifstream&);
  void DesignProcessProperty(ifstream&, string &, string &);
  void DesignAddOneCellToDesignDB(Cell *);
  void DesignAddOneClusterToDesignDB(Cell *);
  void DesignAddOneNetToDesignDB(Net *, double);
  void DesignAddOnePhysRowToDesignDB(PhysRow *);
  void DesignAddDelayArc(string, string, string, double);
  void DesignAddPath(Path *);

  /* Erasing from the design database */
  void DesignRemoveOneClusterFromDesignDB(Cell *);
  
  /* Deletion of objects */
  void DesignDeleteCell(Cell *);
  void DesignDeletePin(Pin *);

  /* Hide-Unhide net functions */
  void DesignHideNet(Net *);
  void DesignUnhideNet(Net *);

  /* Hide-Unhide cell functions */
  void DesignHideCell(Cell *);
  void DesignUnhideCell(Cell *);

  void DesignFileReadOneNode(ifstream &);
  void DesignFileReadNodes(ifstream &);
  void DesignFileReadPins(ifstream &, uint, Net &);
  void DesignFileReadOneNet(ifstream &);
  void DesignFileReadOneRow(ifstream &);
  void DesignFileReadNets(ifstream &);
  void DesignFileReadRows(ifstream &);
  void DesignFileReadOnePlacedCell(ifstream &file, bool skipFixed);
  void DesignFileReadPlacedCells(ifstream& file, bool skipFixed);
  void DesignFileReadCellMap(ifstream& file);
  void DesignFileReadCmds(ifstream& file);
  void DesignFileReadPinsMap(ifstream& file);
  void DesignFileReadCellDelays(ifstream& file);
  void DesignFileReadPathDelays(ifstream& file);

  void DesignOpenFile(string);
  void DesignCloseFile(void);
  Cell *DesignGetNode(string);
  vector<Cell*> DesignClusterSpecifiedCells(vector<vector<void * > >, double);
  void DesignHideNets(std::vector<void*>, std::vector<void *>);
  void DesignPropagateTerminals(Cell *, Cell *);

  void DesignUpdateChipDim(PhysRow *);
  void DesignShiftChipToZeroZero(void);

  /* Bin related set functions */
  void DesignSetPeakUtil(double);
  void DesignSetPeakUtilBinIdx(uint);
  void DesignSetNumBinRows(uint);
  void DesignSetNumBinCols(uint);

  /* HPWL related functions */
  void DesignSetHPWL(double);
  void DesignRemoveFromHPWL(double);
  void DesignAddToHPWL(double);
  double DesignGetHPWL(void);

  /* Set the vector of cells to a particular list */
  vector<Cell *>& DesignGetCellsToSolve(void);
  vector<Cell *> DesignGetCellsSortedByLeft(void);
  vector<Cell *> DesignGetCellsSortedByBot(void);

 public:
  map<string, Cell*> DesignCells;
  map<string, Cell*> DesignHiddenCells;
  map<string, Cell*> DesignClusters;
  map<string, Net*> DesignNets;
  map<string, Net*> DesignHiddenNets;
  vector<Path*> DesignPaths;
  vector<PhysRow*> DesignPhysRows;
  vector<Bin *> DesignBins;
  vector<Cell *> cellsToSolve;
  map<string, map<string, map<string, double > > >  libCellDelayDB;
  vector<Net *> PseudoNets;

  Design();
  Design(string, string, Env &);
  void DesignSetPath();
  void DesignReadCells();
  void DesignReadMapFile();
  void DesignReadNets();
  void DesignReadRows();
  void DesignReadCellPlacement(bool skipFixed);
  void DesignReadCmdsFile();
  void DesignReadPinsMapFile();
  void DesignReadCellDelaysFile();
  void DesignReadPathDelays();
  void DesignCreateBins(uint, uint);
  void DesignCreateBins(void);

  void DesignClearBins(void);

  map<string, Net*>& DesignGetNets(void);
  map<string, Cell*>& DesignGetCells(void);
  map<string, Cell*>& DesignGetClusters(void);
  vector<PhysRow*>& DesignGetRows(void);
  vector<Bin*>& DesignGetBins(void);
  vector<Path*>& DesignGetPaths(void);

  void DesignSetName(string);
  void DesignReadDesign(string, string);
  string DesignGetName(void);
  string DesignGetPath() { }

  void DesignSetGraph(HyperGraph& thisGraph);
  HyperGraph& DesignGetGraph(void);
  void DesignCreateGraph(HyperGraph& thisGraph);
  void DesignBuildGraph(void);
  void DesignDeleteGraph(void);
  void DesignRebuildGraph(void);

  int DesignGetSingleRowHeight();
  uint DesignGetNumCells(void);
  uint DesignGetNumClusters(void);
  uint DesignGetNumClustersSeenSoFar(void);
  uint DesignGetNumTopCells(void);
  uint DesignGetNumNets(void);
  uint DesignGetNumPaths(void);
  uint DesignGetNumTopNets(void);
  uint DesignGetNumFixedCells(void);
  uint DesignGetNumTerminalCells(void);
  uint DesignGetNumPhysRows(void);
  void DesignAddCellToPhysRow(Cell*, vector<vector<int> > &, vector<PhysRow*> &);
  void DesignAddAllCellsToPhysRows(void);

  /* Set and get functions for the design environment */
  void DesignSetEnv(Env &);
  Env& DesignGetEnv(void);

  /* Top level function for invoking the placer */
  void DesignDoGlobalPlacement(void);
  /* Top level function for invoking the legalizer */
  void DesignDoLegalization(void);
  /* Top level function for invoking the detailed placer */
  void DesignDoDetailedPlacement(void);

  /* Chip boundary function */
  void DesignGetBoundingBox(uint&, uint&);
  
  map<uint, uint> DesignGetRowHeights();

  /* Solver functions */
  void DesignSolveForAllCellsMosekIter(void);
  void DesignSolveForAllCellsConjGradIter(void);
  void DesignSolveForCellsNoHyperGraph(vector<Cell *> &cellsToSolve);
  void DesignSolveForAllCellsConjGradWnLibIter(void);
  void DesignSetCellsToSolve(vector<Cell *>);

  /* Calling internal and external placers */
  void DesignRunExternalPlacer(EnvGlobalPlacerType);
  void DesignRunInternalPlacer(EnvSolverType);
  int DesignRunNTUPlace(string, string);
  int DesignRunFastPlace(string, string);
  int DesignRunFastPlaceLegalizer(string, string);
  int DesignRunFastPlaceDetailedPlacer(string, string);
  int DesignRunMPL6(string, string);

  /* Clustering functions */
  //void DesignClusterCells(HyperGraph&, clusteringType);
  //  void DesignCollapseCluster(Cell& MasterCell);
  bool DesignDoDefaultCluster(HyperGraph&);
  bool DesignDoTimingDrivenCluster(HyperGraph&);
  bool DesignDoBestChoiceClustering(HyperGraph&);
  bool DesignDoFirstChoiceClustering(HyperGraph&);
  bool DesignDoClusterTest(void);

  //  bool DesignDoFCCluster(HyperGraph&);
  //  bool DesignDoNetCluster(HyperGraph&);
  //  bool DesignDoESCCluster(HyperGraph&);
  void DesignCoarsenNetlist(void);
  void DesignCollapseClusters(void);
  void DesignPlotCluster(string plotTitle, string plotFileName, Cell *clusterCell, 
			 vector<Cell *> &boundaryCells, vector<uint> &rowNum, 
			 vector<uint> &xPosInRow, vector<Pin*> &clusterPins);
  void DesignGetClusterDimensions(vector<Cell *> &listOfCells, uint &resultWidth, 
				  uint &resultHeight);
  void DesignAssignPinOffSets(Cell *clusterCell, map<Cell *, uint> &boundaryCells,
			      vector<Pin *> &clusterCellPins, map<Pin *, Pin*> &pinMap,
			      vector<uint> &rowNum, vector<uint> &xPosInRow);
  void DesignExpandCluster(Cell *clusterCell, uint &resultHeight, uint &resultWidth);
  bool DesignPlaceCellsInCluster(vector<Cell *> &boundaryCellVec, vector<uint> &, vector<uint> &,
				 uint resultHeight, uint resultWidth);
  bool DesignPlaceCellsInClusterNoLegal(vector<Cell*>&, vector<uint>&, vector<uint>&, 
					uint, uint);
  void DesignPlaceCellsInClusterInCenter(vector<Cell *> &boundaryCellVec, vector<Pin *> &clusterCellPins,
					 vector<uint> &rowNum, vector<uint> &xPosInRow,
					 uint resultHeight, uint resultWidth);

  Cell* DesignClusterCells(vector<Cell *> &listOfCells, bool, bool);
  Cell* DesignClusterCellsSimple(vector<Cell *> &listOfCells);
  void DesignCommitCluster(Cell *);
  void DesignSimpleCollapseCluster(Cell*, vector<Net*>&, vector<Cell*>&);
  void DesignUnclusterCell(Cell*, bool);

  /* Constraint functions */
  void DesignSetClockPeriod(double);
  double DesignGetClockPeriod(void);
  double DesignGetDelayArc(string, string, string);
  
  /* Property checking function */
  bool DesignCheckSolvedCellsProperty(vector<Cell*>);
  double DesignGetAverageStdCellWidth(void);
  double DesignGetAverageStdCellHeight(void);
  uint DesignGetNumStdCells(void);

  /* Spreading related : pseudo net add / get functions */
  void DesignAddPseudoNet(Net *);
  void DesignClearPseudoNetWeights(void);
  void DesignSpreadCellsFast(HyperGraph &, MSKrealt *, 
			     MSKrealt *, MSKrealt *, 
			     MSKrealt *, map<Cell *,uint> &,
			     map<Cell *,uint> &);
  void DesignGetForceOnCell(Cell &, double, double, 
			    double &, double &, double &, 
			    char &, double&, double &, 
			    double &, double &);
  void DesignCreatePseudoPortOld(Cell &, double, double, double, double, 
				 double, double, double, char, 
				 MSKrealt *, MSKrealt *,  
				 MSKrealt *, MSKrealt *, 
				 map<Cell *, uint>&, map<Cell *, uint>&);
  void DesignCreatePseudoPort(Cell &, double, double, double, double, 
			      double, double, double, char, double &,
			      double &, double &);
  void DesignSpreadCreatePseudoPort(Cell&, Bin&, double, double, 
				    double &, double &, double &);

  /* Bin related functions */
  void DesignStretchBins(void);
  double DesignGetPeakUtil(void);
  uint DesignGetPeakUtilBinIdx(void);
  int DesignGetNextRowBinIdx(uint);
  int DesignGetNextColBinIdx(uint);
  int DesignGetPrevRowBinIdx(uint);
  int DesignGetPrevColBinIdx(uint);

  /* Miscellaneous utility functions */
  vector<Cell *> DesignGetCellsOfBin(Bin *binPtr, uint, uint, uint, uint, 
				     vector<Cell *> &, vector<Cell *> &, 
				     double&, double&);
  void DesignPlotData(string, string);
  void DesignPlotDataSelected(string, string, vector<Cell *>&);

  /* Computation of force on Cell */
  void DesignComputeForceOnCell(Cell *, double &, double &, double &,
				char &, double &, double &, double &, double &,
				uint , uint , uint , uint, map<Net *, uint>&);
  void DesignGetBoundaryPoints(double, double, double,
			       double &, double &, char &, double &, double &, double &,
			       double &, double &, uint, uint , uint , uint ,
			       double &, double &);


  /* Write out netlist in a created directory */
  void DesignWriteCurrentNetlist(string, string);

  /* DEFINING AN EXTENSIVE LIST OF DEBUG FUNCTIONS THAT 
     WILL BE HELPFUL */
  void DesignPrintPorts(uint);
  void DesignPrintTerminals(uint);
  void printNumTopCells(void);
  void printNumHiddenCells(void);
  void printNumClusterCells(void);
};

extern void DesignCreateGraph(Design&, HyperGraph&);
extern void DesignCollectStats(Design& myDesign);
extern void DesignWriteStats(Design& myDesign);
extern bool DesignCellIsStdCell(Design &myDesign, Cell &thisCell);
extern void DesignWriteOutputPlacement(Design& myDesign);
extern void DesignWriteOutputPlacement(Design& myDesign, string outputFileName);
extern void DesignWriteBookShelfOutput(Design& myDesign, string opName);
extern vector<Cell*> DesignGetConnectedCells(HyperGraph &, Cell *);

#endif
