/****** 
FUNCTIONS TO CONVERT THE DESIGN THAT HAS 
BEEN READ TO A HYPERGRAPH 
******/
# include <Design.h>
# include <DesignIter.h>
# include <Cell.h>
# include <Pin.h>
# include <sys/stat.h>
# include <sys/types.h>

# define MAX_OUTPUTS 1000
# define SPACES "   "
# define MCOMMA ,
/**************/

/***********************************************************
 Define output extenstions for tables
***********************************************************/
# define PROP_HEIGHT 0
# define PROP_WIDTH 1

# define TOP_LEVEL_TABLE_EXT ".top.out"
# define NODE_OUTPUT_TABLE_EXT ".cellOutput.out"
# define NODE_INPUT_TABLE_EXT ".cellInput.out"
# define NODE_ADJACENT_NODE_TABLE_EXT ".cellAdj.out"

/***********************************************************
 Global variables to store the classify cells based on 
 inputs
***********************************************************/
/* Vector of maps. Each index of the vector represents the 
   number of outputs in each cell */
vector<map<unsigned int, unsigned long > > cellNumOutputs(MAX_OUTPUTS);

/* Vector of (maps of (vector of maps). Each index of the vector represents 
   the number of outputs in each cell. The data of the map
   is another array of maps which is indexed by the number of inputs.
   There are three maps in all, each providing a height, width and 
   area distribution respectively */
vector<map<unsigned int, vector<map<unsigned int, unsigned int > > > > cellStats(MAX_OUTPUTS);

/* Rule for classification:
   Any cell whose height is different from the row height 
   specified in the .scl file is a standard cell */

/*****************************************************
 STANDARD CELL DETAILS
*****************************************************/
/* Capture widths of standard cells in this map */
map<unsigned int, unsigned int>widthStdRanges;
/* Capture areas of standard cells in this map */
map<unsigned int, unsigned int>areaStdRanges;
/* Capture heights of standard cells in this map */
map<unsigned int, unsigned int>heightStdRanges;

/*****************************************************
 MACRO CELL DETAILS
*****************************************************/
/* Capture widths of macro cells in this map */
map<unsigned int, unsigned int>widthMacroRanges;
/* Capture areas of macro cells in this map */
map<unsigned int, unsigned int>areaMacroRanges;
/* Capture heights of macro cells in this map */
map<unsigned int, unsigned int>heightMacroRanges;
/* Capture the aspect ratios of the macro cells 
   which are of the same height */
map<unsigned int, map<double, unsigned int > > aspectRatioMacroRanges; 

/* Other variables for benchmark details */
unsigned int numCells;
unsigned int numFixedCells;
unsigned int numStdCells;
unsigned int numMacroCells;
unsigned int numFixedMacros;
unsigned int numTerminalCells;
unsigned int numNets;

/*****************************************************
 NET DETAILS
*****************************************************/
/* Structure for storing net details using a single map */
typedef struct NetStats {
  /* Total pin count on each net */
  unsigned int pinCount;
  /* Total number of drivers on the net */
  unsigned int driverCount;
  /* Total number of sinks on the net */
  unsigned int sinkCount;
  /* Total number of end points which are std cells */
  unsigned int stdCellEps;
  /* Total number of end points which are macro cells */
  unsigned int macroCellEps;
  /* Pointer to the actual net */
  Net *NetPtr;
} NetStats;

/* For each net: */
/* Number of pins on the net indexed by net name?*/
map<string, NetStats> netAnalysis;

void 
updateCellOutputs(Cell *CellPtr, unsigned int numOutputs) 
{
  unsigned int numInPins;
  map<unsigned int, unsigned long>& mapSelect1 = cellNumOutputs[numOutputs];
  map<unsigned int, vector<map<unsigned int, unsigned int > > >& mapSelect2 = cellStats[numOutputs];
  
  numInPins = (*CellPtr).CellGetNumPins(PIN_DIR_INPUT);

  if (mapSelect2.find(numInPins) == mapSelect2.end()) {
    mapSelect2[numInPins] = vector<map<unsigned int, unsigned int > > ();
  }

  vector<map<unsigned int, unsigned int > > &vectorSelect = mapSelect2[numInPins];

  if (mapSelect1.find(numInPins) == mapSelect1.end()) {
    mapSelect1[numInPins] = 0;
    if (vectorSelect.size() == 0) {
      vectorSelect.push_back(map<unsigned int, unsigned int> ());
      vectorSelect.push_back(map<unsigned int, unsigned int> ());    
    } 
  } 

  map<unsigned int, unsigned int>& subMapSelect1 = vectorSelect[PROP_HEIGHT];
  map<unsigned int, unsigned int>& subMapSelect2 = vectorSelect[PROP_WIDTH];

  unsigned int height, width;
  height = (*CellPtr).CellGetHeight();
  width = (*CellPtr).CellGetWidth();
  if (subMapSelect1.find(height) != subMapSelect1.end()) {
    subMapSelect1[height] = subMapSelect1[height] + 1;
  } else {
    subMapSelect1[height] = 1;
  }
  if (subMapSelect2.find(width) != subMapSelect2.end()) {
    subMapSelect2[width] = subMapSelect2[width] + 1;
  } else {
    subMapSelect2[width] = 1;
  }
  mapSelect1[numInPins] = mapSelect1[numInPins] + 1;
}

void
DesignCollectStats(Design& myDesign)
{
  Cell* CellPtr;
  string Name;
  unsigned int numOutPins;
  unsigned int width, height, area, rowHeight;
  unsigned int max_area, max_width, max_height;
  bool stdCell;
  map<unsigned int, unsigned int>rowHeights;

  /* Get the row heights from design */
  rowHeights = myDesign.DesignGetRowHeights();
  rowHeight = myDesign.DesignGetSingleRowHeight();

  /* Collect stats for number of outputs standard cells */
  DESIGN_FOR_ALL_CELLS(myDesign, Name, CellPtr) {
    if ((*CellPtr).CellGetTerminal() == true) {
      continue;
    }
    
    stdCell = false;
    numOutPins = (*CellPtr).CellGetNumPins(PIN_DIR_OUTPUT);
    updateCellOutputs(CellPtr, numOutPins);
    width = (*CellPtr).CellGetWidth();
    height = (*CellPtr).CellGetHeight();
    area = (*CellPtr).CellGetArea();
    if (area > max_area) max_area = area;
    if (width > max_width) max_width = width;
    if (height > max_height) max_height = height;

    if (rowHeight == -1) {
      if (rowHeights.find(height) != rowHeights.end()) {
	stdCell = true;
      }
    } else if (height == rowHeight) {
      stdCell = true;
    }
    
    if (stdCell == false) {
      if (widthMacroRanges.find(width) != widthMacroRanges.end()) {
	widthMacroRanges[width]++;
      } else {
	widthMacroRanges[width] = 1;
      }
      if (areaMacroRanges.find(area) != areaMacroRanges.end()) {
	areaMacroRanges[area]++;
      } else {
	areaMacroRanges[area] = 1;
      }
      if (heightMacroRanges.find(height) != heightMacroRanges.end()) {
	heightMacroRanges[height]++;
      } else {
	heightMacroRanges[height] = 1;
      }
      if (aspectRatioMacroRanges.find(height) == aspectRatioMacroRanges.end()) {
	aspectRatioMacroRanges[height] = map<double, unsigned int> ();
      }
      map<double, unsigned int> &subMapSelect = aspectRatioMacroRanges[height];
      double aspectRatio = ((double)width)/height;
      aspectRatio = dround(aspectRatio);
      if (subMapSelect.find(aspectRatio) != subMapSelect.end()) {
	subMapSelect[aspectRatio]++;
      } else {
	subMapSelect[aspectRatio] = 1;
      }
    } else {
      if (widthStdRanges.find(width) != widthStdRanges.end()) {
	widthStdRanges[width]++;
      } else {
	widthStdRanges[width] = 1;
      }
      if (areaStdRanges.find(area) != areaStdRanges.end()) {
	areaStdRanges[area]++;
      } else {
	areaStdRanges[area] = 1;
      }
      if (heightStdRanges.find(height) != heightStdRanges.end()) {
	heightStdRanges[height]++;
      } else {
	heightStdRanges[height] = 1;
      }
    }
  } DESIGN_END_FOR;
}

void DesignWriteStats(Design& myDesign)
{
  string DesignName, DesignDir;
  Cell* CellPtr;
  string Name;
  unsigned int numOutPins;
  unsigned int width, height, area;
  unsigned int max_area, max_width, max_height;
  unsigned int count;

  /* Write the gnuplot script file to generate the pdf */
  DesignName = myDesign.DesignGetName();
  DesignDir = DesignName + "_Analysis";
  string DesignDirFullName = "./" + DesignDir;
  
  int result = mkdir(DesignDirFullName.data(), 0777);
  if (result == 0) {
    cout << "Analysis directory created successfully." << endl;
  } else if (result == -1) {
    cout << "Analysis directory could not be created as there are no permissions or it may already exist." << endl;
  }

  /* Write a gnuplot file header */
  /***********************************/
  /* Write output to top level table */
  /***********************************/
  {
    unsigned long count;
    unsigned int numInputs;
    for (int i = 0; i < cellNumOutputs.size(); i++) {
      map<unsigned int, unsigned long>& mapSelect = cellNumOutputs[i];
      if (mapSelect.size() > 0) {
	ofstream outputFile;
	string outputFileName;
	outputFileName = DesignDir + "/" + "DesignCell" + 
	  getStrFromInt(i) + "Outputs" +  + ".txt";
	outputFile.open(outputFileName.data(), ifstream::out);
	outputFile << "#COUNT" << SPACES << "INPUTS" << endl;
	MAP_FOR_ALL_ELEMS(mapSelect, unsigned int, unsigned long, numInputs, count) {
	  outputFile << count << SPACES << numInputs << endl;
	} END_FOR;
	outputFile.close();
      }
    }
  }

  /**********************************************************/
  /* For a cell of given number of inputs and outputs write */ 
  /* out the height and width distribution.                 */
  /**********************************************************/
  {
    unsigned long count;
    unsigned int numInputs;
    for (int i = 0; i < cellNumOutputs.size(); i++) {
      map<unsigned int, vector<map<unsigned int, unsigned int > > >& mapSelect = cellStats[i];
      if (mapSelect.size() > 0) {
	vector<map<unsigned int, unsigned int > > myVector;
	/* Hack to bypass C++ macro substitution use MCOMMA 
	   inside iterators instead of ',' */
	MAP_FOR_ALL_ELEMS(mapSelect, unsigned int, 
			  vector<map<unsigned int MCOMMA unsigned int > >, 
			  numInputs, myVector) {
	  ofstream outputFile;
	  string outputFileName;
	  unsigned int width, height;

	  map<unsigned int, unsigned int>& subMapSelect1 = myVector[PROP_HEIGHT];
	  outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + 
	    "Outputs" + getStrFromInt(numInputs) + "Inputs" + "Heights" + ".txt";
	  outputFile.open(outputFileName.data(), ifstream::out);
	  outputFile << "#COUNT" << SPACES << "HEIGHT" << endl;	  
	  MAP_FOR_ALL_ELEMS(subMapSelect1, unsigned int, unsigned int, height, count) {
	    outputFile << count << SPACES << height << endl;
	  } END_FOR;
	  outputFile.close();

	  map<unsigned int, unsigned int>& subMapSelect2 = myVector[PROP_WIDTH];
	  outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + 
	    "Outputs" + getStrFromInt(numInputs) + "Inputs" + "Widths" + ".txt";
	  outputFile.open(outputFileName.data(), ifstream::out);
	  outputFile << "#COUNT" << SPACES << "WIDTH" << endl;	  
	  MAP_FOR_ALL_ELEMS(subMapSelect2, unsigned int, unsigned int, width, count) {
	    outputFile << count << SPACES << width << endl;
	  } END_FOR;
	  outputFile.close();
	} END_FOR;
      }
    }
  }
  
  /**********************************************************/
  /* Write cell width analysis                              */ 
  /**********************************************************/
  {
    ofstream outputFile;
    /* Width of standard cells */
    string outputFileName;
    outputFileName = DesignDir + "/" + "DesignStandardCell" + "WidthAnalysis" + ".txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    unsigned int count;
    outputFile << "#COUNT" << SPACES << "WIDTH" << endl;	  
    MAP_FOR_ALL_ELEMS(widthStdRanges, unsigned int, unsigned int, width, count) {
      outputFile << count << SPACES << width << endl;
    } END_FOR;
    outputFile.close();

    /* Width of macro cells */
    outputFileName = DesignDir + "/" + "DesignMacroCell" + "WidthAnalysis" + ".txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "#COUNT" << SPACES << "WIDTH" << endl;	  
    MAP_FOR_ALL_ELEMS(widthMacroRanges, unsigned int, unsigned int, width, count) {
      outputFile << count << SPACES << width << endl;
    } END_FOR;
    outputFile.close();
  }

  /**********************************************************/
  /* Write cell height analysis                             */ 
  /**********************************************************/
  {
    ofstream outputFile;
    string outputFileName;
    /* Height of standard cells */
    outputFileName = DesignDir + "/" + "DesignStandardCell" + "HeightAnalysis" + ".txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "#COUNT" << SPACES << "HEIGHT" << endl;	  
    MAP_FOR_ALL_ELEMS(heightStdRanges, unsigned int, unsigned int, height, count) {
      outputFile << count << SPACES << height << endl;
    } END_FOR;
    outputFile.close();

    /* Height of macro cells */
    outputFileName = DesignDir + "/" + "DesignMacroCell" + "HeightAnalysis" + ".txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "#COUNT" << SPACES << "HEIGHT" << endl;	  
    MAP_FOR_ALL_ELEMS(heightMacroRanges, unsigned int, unsigned int, height, count) {
      outputFile << count << SPACES << height << endl;
    } END_FOR;
    outputFile.close();
  }

  /**********************************************************/
  /* Write cell area analysis                               */ 
  /**********************************************************/
  {
    ofstream outputFile;
    string outputFileName;
    outputFileName = DesignDir + "/" + "DesignStandardCell" + "AreaAnalysis" + ".txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "#COUNT" << SPACES << "AREA" << endl;	  
    MAP_FOR_ALL_ELEMS(areaStdRanges, unsigned int, unsigned int, area, count) {
      outputFile << count << SPACES << area << endl;
    } END_FOR;
    outputFile.close();

    outputFileName = DesignDir + "/" + "DesignMacroCell" + "AreaAnalysis" + ".txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "#COUNT" << SPACES << "AREA" << endl;	  
    MAP_FOR_ALL_ELEMS(areaMacroRanges, unsigned int, unsigned int, area, count) {
      outputFile << count << SPACES << area << endl;
    } END_FOR;
    outputFile.close();
  }
}

