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
 Macro for printing out tables into files. Data is always 
***********************************************************/
# define _WRITE_TABLE(opFileName, dataTitle, Col1Title, Col2Title,	\
		      mapToUse, keyType, dataType)			\
  {									\
  keyType key;								\
  dataType count, totalCount;						\
  vector<double> statData;						\
  ofstream opFile;							\
  opFile.open(opFileName.data(), ifstream::out);			\
  opFile << dataTitle << endl;						\
  opFile << Col1Title << SPACES << Col2Title << endl;			\
  totalCount = 0;							\
  MAP_FOR_ALL_ELEMS(mapToUse, keyType, dataType, key, count) {		\
    opFile << count << SPACES << key << endl;				\
    totalCount+=count;							\
  } END_FOR;								\
  opFile << "#TOTAL" << SPACES << totalCount << endl;			\
  statData = DesignGetStatData<keyType>(mapToUse);                      \
  opFile << "#MEAN" << SPACES << statData[0] << endl; 			\
  opFile << "#MAX" << SPACES << statData[1] << endl; 			\
  opFile << "#MIN" << SPACES << statData[2] << endl; 			\
  opFile << "#VARIANCE" << SPACES << statData[3] << endl;		\
  opFile << "#STD DEVIATION" << SPACES << statData[4] << endl;		\
  opFile.close();							\
  }									

/***********************************************************
 Global variables to store the classify cells based on 
 inputs
***********************************************************/
/* Vector of maps. Each index of the vector represents the 
   number of outputs in each cell */
vector<map<unsigned int, unsigned int > > cellNumOutputs(MAX_OUTPUTS);

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
/* Capture the aspect ratios regardless of height */
map<double, unsigned int> aspectRatioAllMacroRanges; 

/* Other variables for benchmark details */
unsigned int numCells;
unsigned int numFixedCells;
unsigned int numStdCells;
unsigned int numMacroCells;
unsigned int numFixedMacros;
unsigned int numTerminalCells;
unsigned int numNets;
unsigned int numPhysRows;

/*****************************************************
 NET DETAILS
*****************************************************/
/* Structure for storing net details using a single map */
typedef struct NetStatsStruct {
  /* Total pin count on each net */
  unsigned int pinCount;
  /* Total number of drivers on the net */
  unsigned int driverCount;
  /* Total number of load on the net */
  unsigned int loadCount;
  /* Total number of end points which are std cells */
  unsigned int stdCellEps;
  /* Total number of end points which are macro cells */
  unsigned int macroCellEps;
  /* Total number of end points which are on terminal cells */
  unsigned int terminalCellEps;
  /* Pointer to the actual net */  
  Net *NetPtr;
} NetStatsStruct;

typedef struct NetStatsStruct * NetStat;
typedef struct NetStatsStruct NetStatsStruct;

/* For each net: */
/* Number of pins on the net indexed by net name?*/
vector<NetStat> netStats;

NetStat
createNetStats()
{
  NetStat thisNetStat = new NetStatsStruct();  
  thisNetStat->pinCount = 0;
  thisNetStat->driverCount = 0;
  thisNetStat->loadCount = 0;
  thisNetStat->stdCellEps = 0;
  thisNetStat->macroCellEps = 0;
  thisNetStat->terminalCellEps = 0;
  return (thisNetStat);
}

void
updateNetStats(Net *NetPtr, NetStat thisNetStat)
{
  Net &thisNet = (*NetPtr);
  Pin *PinPtr;

  _ASSERT("Net status pointer is NULL", (thisNetStat == NIL(NetStat)));
  _ASSERT("Net pointer is NULL", (NetPtr == NIL(Net *)));
  
  thisNetStat->pinCount = thisNet.NetGetPinCount();
  thisNetStat->driverCount = thisNet.NetGetDriverCount();
  thisNetStat->loadCount = thisNet.NetGetLoadCount();

  NET_FOR_ALL_PINS(thisNet, PinPtr) {
    Cell &cellOfPin = (*PinPtr).PinGetParentCell();
        if (cellOfPin.CellIsTerminal()) {
          thisNetStat->terminalCellEps++;
    }
    if (cellOfPin.CellIsMacro()) {
      thisNetStat->macroCellEps++;
    } else {
      thisNetStat->stdCellEps++;
    }
  } NET_END_FOR;
  thisNetStat->NetPtr = NetPtr;
}

void 
updateCellOutputs(Cell *CellPtr, unsigned int numOutputs) 
{
  unsigned int numInPins;
  map<unsigned int, unsigned int>& mapSelect1 = cellNumOutputs[numOutputs];
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

/********************************************************************
  Comparison function used by the sorting algorithm to sort a 
  vector of objects of type netStats. Modifying the comparison 
  function results in different sorted orders.
********************************************************************/
bool sortCmpFunc (NetStat obj1, NetStat obj2) {
  bool result = false;
  if (obj1->loadCount > obj2->loadCount) {
    result = true;
  }
  return result;
}

void
DesignCollectStats(Design& myDesign)
{
  Cell* CellPtr;
  Net* NetPtr;
  string Name;
  unsigned int numOutPins;
  unsigned int width, height, area;
  unsigned int max_area, max_width, max_height;
  bool stdCell;


  /*******************************************************
   COLLECT TOP LEVEL INFORMATION
  *******************************************************/
  numCells = myDesign.DesignGetNumCells();
  numFixedCells = myDesign.DesignGetNumFixedCells();
  numNets = myDesign.DesignGetNumNets();
  numPhysRows = myDesign.DesignGetNumPhysRows();

  numStdCells = 0;
  numMacroCells = 0;
  numFixedMacros = 0;
  numTerminalCells = 0;

  /*******************************************************
   COLLECT NET INFORMATION
  *******************************************************/
  DESIGN_FOR_ALL_NETS(myDesign, Name, NetPtr) {
    NetStat thisNetStat = createNetStats();
    updateNetStats(NetPtr, thisNetStat);
    netStats.push_back(thisNetStat);
  } DESIGN_END_FOR;
  sort(netStats.begin(),netStats.end(), sortCmpFunc);

  /*******************************************************
   COLLECT CELL INFORMATION
  *******************************************************/
  /* Collect stats for number of outputs standard cells */
  DESIGN_FOR_ALL_CELLS(myDesign, Name, CellPtr) {
    stdCell = false;
    numOutPins = (*CellPtr).CellGetNumPins(PIN_DIR_OUTPUT);
    updateCellOutputs(CellPtr, numOutPins);
    width = (*CellPtr).CellGetWidth();
    height = (*CellPtr).CellGetHeight();
    area = (*CellPtr).CellGetArea();
    if (area > max_area) max_area = area;
    if (width > max_width) max_width = width;
    if (height > max_height) max_height = height;

    if ((*CellPtr).CellIsTerminal()) {
      numTerminalCells++;
    }

    stdCell = DesignCellIsStdCell(myDesign, *CellPtr);
    if (stdCell == false) {
      numMacroCells++;
      if ((*CellPtr).CellIsFixed()) {
	numFixedMacros++;
      }
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

      double aspectRatio = ((double)width)/height;
      aspectRatio = dround(aspectRatio);
      if (aspectRatioAllMacroRanges.find(aspectRatio) != 
	  aspectRatioAllMacroRanges.end()) {
	aspectRatioAllMacroRanges[aspectRatio]++;
      } else {
	aspectRatioAllMacroRanges[aspectRatio] = 1;
      }
      if (aspectRatioMacroRanges.find(height) == aspectRatioMacroRanges.end()) {
	aspectRatioMacroRanges[height] = map<double, unsigned int> ();
      }
      map<double, unsigned int> &subMapSelect = aspectRatioMacroRanges[height];
      if (subMapSelect.find(aspectRatio) != subMapSelect.end()) {
	subMapSelect[aspectRatio]++;
      } else {
	subMapSelect[aspectRatio] = 1;
      }
    } else {
      numStdCells++;
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

/* 
Get the statistical analysis of a data set.
This function gets the mean, standard deviation 
and anything else that is required. Returned as a
vector of doubles. Table is something that has 
a pair of values: a parameter and a count of the 
parameter.

Return order:
Mean
Variance
Standard deviation
*/
template <typename dataType> 
vector<double> 
DesignGetStatData(map<dataType, unsigned int> table) 
{
  vector<double> rtv;
  unsigned int count, totalCount;
  dataType param;
  double param_mean;
  double max, min;
  double variance;
  double stdDev;
  double diff;
  
  variance = 0;
  param_mean = 0;
  stdDev = 0;
  totalCount = 0;
  diff = 0;
  max = 0;
  min = INT_MAX;

  for(typename map<dataType,unsigned int>::iterator iter = table.begin(); 
      iter != table.end(); ++iter) {		
    param = (dataType)iter->first;			
    count = (unsigned int)iter->second;
    param_mean += (param * count);
    totalCount += count;
    if (max < param) {
      max = (double)param;
    } 
    if (min > param) {
      min = (double)param;
    }
  }

  param_mean /= totalCount;

  for(typename map<dataType,unsigned int>::iterator iter = table.begin(); 
      iter != table.end(); ++iter) {		
    param = (dataType)iter->first;			
    count = (unsigned int)iter->second;
    diff = param_mean - param;
    variance += (diff * diff) * count;
  };

  variance /= totalCount;
  
  stdDev = sqrt(variance);
  
  rtv.push_back(param_mean);
  rtv.push_back(max);
  rtv.push_back(min);
  rtv.push_back(variance);
  rtv.push_back(stdDev);

  return (rtv);
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
  ofstream outputFile;
  string outputFileName;

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
  /***********************************/
  /* Write top level statistics      */
  /***********************************/
  {
    outputFileName = DesignDir + "/" + DesignName + "Top.out";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "Design Name                : " << DesignName << endl;
    outputFile << "Number of nets             : " << numNets << endl;
    outputFile << "Number of cells            : " << numCells << endl;
    outputFile << "Number of standard cells   : " << numStdCells << endl;
    outputFile << "Number of macro cells      : " << numMacroCells << endl;
    outputFile << "Number of fixed macro cells: " << numFixedMacros << endl;
    outputFile << "Number of terminal cells   : " << numTerminalCells << endl << endl;
    outputFile << "Number of physical rows    : " << numPhysRows << endl;
  }
  
  /* Write a gnuplot file header */
  /* Write out main benchmark analysis file */
  {
    outputFileName = DesignDir + "/" + DesignName + "Analysis.txt";
  }

  /***********************************/
  /* Write net statistics            */
  /***********************************/
  {
    NetStat thisNetStat;
    outputFileName = DesignDir + "/" + "DesignNetStats.txt";
    outputFile.open(outputFileName.data(), ifstream::out);
    outputFile << "#TITLE Net statistics for the design" << endl;
    outputFile << "#NETNAME"  << SPACES << "PINCOUNT" << SPACES << "DRIVERCOUNT" << SPACES;
    outputFile << "LOADCOUNT" << SPACES << "STDCELLEPS" << SPACES << "MACROCELLEPS" << SPACES;
    outputFile << "TERMINALCELLEPS" << endl;
    VECTOR_FOR_ALL_ELEMS(netStats, NetStat, thisNetStat) {
      outputFile << (*(thisNetStat->NetPtr)).NetGetName() << SPACES;
      outputFile << thisNetStat->pinCount << SPACES;
      outputFile << thisNetStat->driverCount << SPACES;
      outputFile << thisNetStat->loadCount << SPACES;
      outputFile << thisNetStat->stdCellEps << SPACES;
      outputFile << thisNetStat->macroCellEps << SPACES;
      outputFile << thisNetStat->terminalCellEps;
      outputFile << endl;
    } END_FOR;
  }
    
  /***********************************/
  /* Write output to top level table */
  /***********************************/
  {
    unsigned int count;
    unsigned int numInputs;
    for (int i = 0; i < cellNumOutputs.size(); i++) {
      map<unsigned int, unsigned int>& mapSelect = cellNumOutputs[i];
      if (mapSelect.size() > 0) {
	outputFileName = DesignDir + "/" + "DesignCell" + 
	  getStrFromInt(i) + "Outputs" +  + ".txt";
	string mainTitle = "";
	mainTitle = mainTitle + "#TITLE " + "Table for cells with ";
	mainTitle = mainTitle + getStrFromInt(i) + " outputs";
	_WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "INPUTS", 
		     mapSelect, unsigned int, unsigned int);
      }
    }
  }

  /**********************************************************/
  /* For a cell of given number of inputs and outputs write */ 
  /* out the height and width distribution.                 */
  /**********************************************************/
  {
    unsigned int count;
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
	  unsigned int width, height;
	  map<unsigned int, unsigned int>& subMapSelect1 = myVector[PROP_HEIGHT];
	  outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + 
	    "Outputs" + getStrFromInt(numInputs) + "Inputs" + "Heights" + ".txt";
	  string mainTitle = "";
	  mainTitle = mainTitle + "#TITLE " + "Table for height distribution of cells with " + getStrFromInt(i);
	  mainTitle = " Outputs and " + getStrFromInt(numInputs) + "Inputs";
	  _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "HEIGHTS", subMapSelect1, 
		       unsigned int, unsigned int);

	  map<unsigned int, unsigned int>& subMapSelect2 = myVector[PROP_WIDTH];
	  outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + 
	    "Outputs" + getStrFromInt(numInputs) + "Inputs" + "Widths" + ".txt";

	  mainTitle = "";
	  mainTitle = mainTitle + "#TITLE " + "Table for width distribution of cells with " + getStrFromInt(i);
	  mainTitle = " Outputs and " + getStrFromInt(numInputs) + "Inputs";
	  _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "WIDTHS", subMapSelect2, 
		       unsigned int, unsigned int);
	} END_FOR;
      }
    }
  }
  
  /**********************************************************/
  /* Write cell width analysis                              */ 
  /**********************************************************/
  {
    string mainTitle = "";
    /* Width of standard cells */
    outputFileName = DesignDir + "/" + "DesignStandardCell" + "WidthAnalysis" + ".txt";
    mainTitle = mainTitle + "#TITLE " + "Table for width distribution of STD cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "WIDTH", widthStdRanges, unsigned int, 
		 unsigned int);

    /* Width of macro cells */
    mainTitle = "";
    outputFileName = DesignDir + "/" + "DesignMacroCell" + "WidthAnalysis" + ".txt";
    mainTitle = mainTitle + "#TITLE " + "Table for width distribution of MACRO cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "WIDTH", widthMacroRanges, unsigned int, 
		 unsigned int);
  }

  /**********************************************************/
  /* Write cell height analysis                             */ 
  /**********************************************************/
  {
    string mainTitle = "";
    /* Height of standard cells */
    outputFileName = DesignDir + "/" + "DesignStandardCell" + "HeightAnalysis" + ".txt";
    mainTitle = mainTitle + "#TITLE " + "Table for height distribution of STD cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "HEIGHT", heightStdRanges, unsigned int, 
		 unsigned int);

    /* Height of macro cells */
    outputFileName = DesignDir + "/" + "DesignMacroCell" + "HeightAnalysis" + ".txt";
    mainTitle = "";
    mainTitle = mainTitle + "#TITLE " + "Table for height distribution of MACRO cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "HEIGHT", heightMacroRanges, unsigned int, 
		 unsigned int);
  }

  /**********************************************************/
  /* Write cell area analysis                               */ 
  /**********************************************************/
  {
    string mainTitle = "";
    /* Area of standard cells */
    outputFileName = DesignDir + "/" + "DesignStandardCell" + "AreaAnalysis" + ".txt";
    mainTitle = mainTitle + "#TITLE " + "Table for area distribution of STD cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "AREA", areaStdRanges, unsigned int, 
		 unsigned int);

    /* Area of macro cells */
    mainTitle = "";
    outputFileName = DesignDir + "/" + "DesignMacroCell" + "AreaAnalysis" + ".txt";
    mainTitle = mainTitle + "#TITLE " + "Table for area distribution of MACRO cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "AREA", areaMacroRanges, unsigned int, 
		 unsigned int);
  }

  /**********************************************************/
  /* Write aspect ratio analysis                            */ 
  /**********************************************************/
  {
    string mainTitle = "";
    /* 
       For aspect ratios of macros, a height-wise ordering is required. 
       This is because there is no point in comparing two macros who have
       the same aspect ratio of the. However for now we will only 
       be doing a direct recording and outputting.
    */
    /* Aspect ratio of macro cells */
    outputFileName = DesignDir + "/" + "DesignMacroCell" + "AspectRatioAnalysis" + ".txt";
    mainTitle = mainTitle + "#TITLE " + "Table for aspect ratio distribution of MACRO cells";
    _WRITE_TABLE(outputFileName, mainTitle, "#COUNT", "ASPECT_RATIO", aspectRatioAllMacroRanges, 
		 double, unsigned int);
  }
}

