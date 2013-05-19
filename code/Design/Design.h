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
# include <AnalyticalSolve.h>
# include <CellSpread.h>
# include <Bin.h>
# include <Plot.h>
# include <mosek.h>

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
  Type definitions for the design class
*******************************************************************************/
typedef enum {
  DEFAULT_CLUSTER, 
  FC_CLUSTER,
  NET_CLUSTER,
  ESC_CLUSTER,
  TOTAL_CLUSTERING_TECHNIQUES,
} clusteringType;

class Design {
 private:
  map<string, Cell*> DesignClusters;
  map<unsigned int, unsigned int>RowHeights;

  HyperGraph *DesignGraphPtr;
  unsigned int NumCells;
  unsigned int NumTopCells;
  unsigned int NumNets;
  unsigned int NumTopNets;
  unsigned int NumPhysRows;
  unsigned int NumFixedCells;
  unsigned int NumTerminalCells;
  unsigned int singleRowHeight;
  unsigned int maxx, maxy;

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
  
  /* Average cell width */
  double averageCellWidth;

  void DesignInit(void);
  void DesignFileReadHeader(ifstream&);
  void DesignProcessProperty(ifstream&, string &, 
			     string &);
  void DesignAddOneCellToDesignDB(Cell *);
  void DesignAddOneNetToDesignDB(Net *, double);
  void DesignAddOnePhysRowToDesignDB(PhysRow *);
  void DesignAddDelayArc(string, string, string, double);

  void DesignFileReadOneNode(ifstream &);
  void DesignFileReadNodes(ifstream &);
  void DesignFileReadPins(ifstream &, unsigned int, Net &);
  void DesignFileReadOneNet(ifstream &);
  void DesignFileReadOneRow(ifstream &);
  void DesignFileReadNets(ifstream &);
  void DesignFileReadRows(ifstream &);
  void DesignFileReadOneFixedCell(ifstream &file);
  void DesignFileReadFixedCells(ifstream& file);
  void DesignFileReadCellMap(ifstream& file);
  void DesignFileReadCmds(ifstream& file);
  void DesignFileReadPinsMap(ifstream& file);
  void DesignFileReadCellDelays(ifstream& file);

  void DesignOpenFile(string);
  void DesignCloseFile(void);
  Cell *DesignGetNode(string);
  vector<Cell*> DesignClusterSpecifiedCells(vector<vector<void * > >, double);
  void DesignHideNets(std::vector<void*>, std::vector<void *>);
  void DesignPropagateTerminals(Cell *, Cell *);

  void DesignUpdateChipDim(PhysRow *);

  /* Bin related set functions */
  void DesignSetPeakUtil(double);
  void DesignSetPeakUtilBinIdx(uint);
  void DesignSetNumBinRows(uint);
  void DesignSetNumBinCols(uint);

  /* Set the vector of cells to a particular list */
  vector<Cell *>& DesignGetCellsToSolve(void);
  vector<Cell *> DesignGetCellsSortedByLeft(void);
  vector<Cell *> DesignGetCellsSortedByBot(void);

 public:
  map<string, Cell*> DesignCells;
  map<string, Net*> DesignNets;
  vector<PhysRow*> DesignPhysRows;
  vector<Bin *> DesignBins;
  vector<Cell *> cellsToSolve;
  map<string, map<string, map<string, double > > >  libCellDelayDB;
  vector<Net *> PseudoNets;

  Design();
  Design(string, string);
  void DesignSetPath();
  void DesignReadCells();
  void DesignReadMapFile();
  void DesignReadNets();
  void DesignReadRows();
  void DesignReadCellPlacement();
  void DesignReadCmdsFile();
  void DesignReadPinsMapFile();
  void DesignReadCellDelaysFile();
  void DesignCreateBins(uint, uint);

  void DesignClearBins(void);

  map<string, Net*>& DesignGetNets(void);
  map<string, Cell*>& DesignGetCells(void);
  vector<PhysRow*>& DesignGetRows(void);
  vector<Bin*>& DesignGetBins(void);

  void DesignSetName(string);
  void DesignReadDesign(string, string);
  string DesignGetName(void);
  string DesignGetPath() { }

  void DesignSetGraph(HyperGraph& thisGraph);
  HyperGraph& DesignGetGraph(void);

  int DesignGetSingleRowHeight();
  unsigned int DesignGetNumCells(void);
  unsigned int DesignGetNumTopCells(void);
  unsigned int DesignGetNumNets(void);
  unsigned int DesignGetNumTopNets(void);
  unsigned int DesignGetNumFixedCells(void);
  unsigned int DesignGetNumTerminalCells(void);
  unsigned int DesignGetNumPhysRows(void);
  void DesignAddCellToPhysRow(Cell*, vector<vector<int> > &, vector<PhysRow*> &);
  void DesignAddAllCellsToPhysRows(void);

  /* Chip boundary function */
  void DesignGetBoundingBox(uint&, uint&);
  
  map<unsigned int, unsigned int> DesignGetRowHeights();
  void DesignClusterCells(HyperGraph&, clusteringType);
  void DesignCollapseCluster(Cell& MasterCell);

  /* Solver functions */
  void DesignSolveForSeqCells(seqSolverType);
  void DesignSolveForAllCells(allSolverType);
  void DesignSolveForAllCellsTest(allSolverType);
  void DesignSolveAllCells(seqSolverType, allSolverType);
  void DesignSetCellsToSolve(vector<Cell *>);
  void DesignSolveForAllCellsIter(void);
  void DesignSolveForAllCellsIterOld(void);

  /* Clustering functions */
  bool DesignDoDefaultCluster(HyperGraph&);
  bool DesignDoFCCluster(HyperGraph&);
  bool DesignDoNetCluster(HyperGraph&);
  bool DesignDoESCCluster(HyperGraph&);
  
  /* Constraint functions */
  void DesignSetClockPeriod(double);
  double DesignGetClockPeriod(void);
  double DesignGetDelayArc(string, string, string);
  
  /* Property checking function */
  bool DesignCheckSolvedCellsProperty(vector<Cell*>);

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
  void DesignCreatePseudoPort(Cell &, double, double, double, double, 
			      double, double, double, char, 
			      MSKrealt *, MSKrealt *,  
			      MSKrealt *, MSKrealt *, 
			      map<Cell *, uint>&, map<Cell *, uint>&);
  void DesignSpreadCreatePseudoPort(Cell&, Bin&, double, double, MSKrealt*, 
				    MSKrealt*, MSKrealt*, MSKrealt*, 
				    std::map<Cell *, unsigned int>&, 
				    std::map<Cell *, unsigned int>&);
  void DesignStretchBins(void);

  /* Bin related functions */
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
  
};

extern void DesignCreateGraph(Design&, HyperGraph&);
extern void DesignCollectStats(Design& myDesign);
extern void DesignWriteStats(Design& myDesign);
extern bool DesignCellIsStdCell(Design &myDesign, Cell &thisCell);
extern void DesignWriteNodes(Design &myDesign, string fname);
extern void DesignWriteBookShelfOutput(Design& myDesign);
#endif
