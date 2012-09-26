# include <stdlib.h>
# include <common.h>
# include <Cell.h>
# include <Net.h>

# ifndef DESIGN_H
# define DESIGN_H

# define NUM_NODEFILE_HEADER_LINES 4
# define NUM_NETFILE_HEADER_LINES 4

# define NUM_NODE_PROPERTIES 2
# define NUM_NET_PROPERTIES 2

# define NUM_NODES_PROPERTY "NumNodes"
# define NUM_TERMINALS_PROPERTY "NumTerminals"

# define NUM_NETS_PROPERTY "NumNets"
# define NUM_PINS_PROPERTY "NumPins"
# define NET_DEGREE_PROPERTY "NetDegree"

# define NODE_TERMINAL_KEYWORD "terminal"

# define DESIGN_AUX_FILE_EXT ".aux"
# define DESIGN_CELL_FILE_EXT ".nodes"
# define DESIGN_NET_FILE_EXT ".nets"
# define DESIGN_NET_WTS_FILE_EXT ".wts"

# define PIN_DIR_INPUT_STRING "I"
# define PIN_DIR_OUTPUT_STRING "O"

# define DIR_SEP "/"

# define ZERO '0'
# define NINE '9'

class Design {
 private:
  map<string, Cell*> DesignCells;
  map<string, Net*> DesignNets;
  map<string, Cell*> DesignClusters;

  int NumCells;
  int NumNets;
  string Name;
  string DesignPath;
  string DesignCellFileName;
  string DesignNetFileName;
  ifstream DesignFile;
  void DesignFileReadHeader(ifstream&);
  void DesignProcessProperty(ifstream&, string &, 
			     string &);
  void DesignAddOneCellToDesignDB(Cell *);
  void DesignAddOneNetToDesignDB(Net *);
  void DesignFileReadOneNode(ifstream &);
  void DesignFileReadNodes(ifstream &);
  void DesignFileReadPins(ifstream &, unsigned int,
			  Net &);
  void DesignFileReadOneNet(ifstream &);
  void DesignFileReadNets(ifstream &);
  void DesignOpenFile(string);
  void DesignCloseFile(void);
  Cell *DesignGetNode(string);
 public:
  Design() { NumNets = 0; NumCells = 0; }
  Design(string, string);
  void DesignSetPath();
  void DesignReadCells();
  void DesignReadNets();
  void DesignSetName(string);
  void DesignReadDesign(string, string);
  string DesignGetName();
  string DesignGetPath() { }
  void DesignClusterCells(vector<Cell*>& CellSet);
    void DesignCollapseCluster(Cell& MasterCell);

};


#endif
