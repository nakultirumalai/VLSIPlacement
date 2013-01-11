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
# include <DesignIter.h>

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
  Type definitions for the design class
*******************************************************************************/


class Design {
 private:
  map<string, Cell*> DesignClusters;
  map<unsigned int, unsigned int>RowHeights;

  HyperGraph *DesignGraphPtr;

  unsigned int NumCells;
  unsigned int NumNets;
  unsigned int NumPhysRows;
  unsigned int NumFixedCells;
  unsigned int NumTerminalCells;

  unsigned int singleRowHeight;

  string Name;
  string DesignPath;
  string DesignCellFileName;
  string DesignNetFileName;
  string DesignNetWtsFileName;
  string DesignSclFileName;
  string DesignPlFileName;
  bool RowBasedPlacement;

  ifstream DesignFile;
  
  void DesignInit(void);
  void DesignFileReadHeader(ifstream&);
  void DesignProcessProperty(ifstream&, string &, 
			     string &);
  void DesignAddOneCellToDesignDB(Cell *);
  void DesignAddOneNetToDesignDB(Net *);
  void DesignAddOnePhysRowToDesignDB(PhysRow *);
  void DesignFileReadOneNode(ifstream &);
  void DesignFileReadNodes(ifstream &);
  void DesignFileReadPins(ifstream &, unsigned int,
			  Net &);
  void DesignFileReadOneNet(ifstream &);
  void DesignFileReadOneRow(ifstream &);
  void DesignFileReadNets(ifstream &);
  void DesignFileReadRows(ifstream &);
  void DesignFileReadOneFixedCell(ifstream &file);
  void DesignFileReadFixedCells(ifstream& file);

  void DesignOpenFile(string);
  void DesignCloseFile(void);
  Cell *DesignGetNode(string);
  Cell *DesignClusterSpecifiedCells(vector<void *> listOfCells, 
				    double aspectRatio);
 public:
  map<string, Cell*> DesignCells;
  map<string, Net*> DesignNets;
  vector<PhysRow*> DesignPhysRows;
  
  Design();
  Design(string, string);
  void DesignSetPath();
  void DesignReadCells();
  void DesignReadNets();
  void DesignReadRows();
  void DesignReadCellPlacement();

  map<string, Net*>& DesignGetNets(void);
  map<string, Cell*>& DesignGetCells(void);
  void DesignSetName(string);
  void DesignReadDesign(string, string);
  string DesignGetName(void);
  string DesignGetPath() { }

  void DesignSetGraph(HyperGraph& thisGraph);
  HyperGraph& DesignGetGraph(void);

  int DesignGetSingleRowHeight();
  unsigned int DesignGetNumCells(void);
  unsigned int DesignGetNumNets(void);
  unsigned int DesignGetNumFixedCells(void);
  unsigned int DesignGetNumTerminalCells(void);
  unsigned int DesignGetNumPhysRows(void);

  map<unsigned int, unsigned int> DesignGetRowHeights();
  void DesignClusterCells(HyperGraph&);
  void DesignCollapseCluster(Cell& MasterCell);
};

extern void DesignCreateGraph(Design&, HyperGraph&);
extern void DesignCollectStats(Design& myDesign);
extern void DesignWriteStats(Design& myDesign);
extern bool DesignCellIsStdCell(Design &myDesign, Cell &thisCell);
extern void DesignWriteNodes(Design &myDesign);

#endif
