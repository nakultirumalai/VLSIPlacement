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
map<unsigned int, unsigned long>widthStdRanges;
/* Capture areas of standard cells in this map */
map<unsigned int, unsigned long>areaStdRanges;
/* Capture heights of standard cells in this map */
map<unsigned int, unsigned long>heightStdRanges;

/*****************************************************
 MACRO CELL DETAILS
*****************************************************/
/* Capture widths of cells in this map */
map<unsigned int, unsigned long>widthMacroRanges;
/* Capture areas of cells in this map */
map<unsigned int, unsigned long>areaMacroRanges;
/* Capture heights of cells in this map */
map<unsigned int, unsigned long>heightMacroRanges;

/* Other variables for benchmark details */
unsigned int numCells;
unsigned int numNets;
vector<unsigned int> rowHeights;

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
  unsigned int width, height, area;
  unsigned int max_area, max_width, max_height;

  /* Get the row heights from design 
  /* Collect stats for number of outputs standard cells */
  DESIGN_FOR_ALL_CELLS(myDesign, Name, CellPtr) {
    numOutPins = (*CellPtr).CellGetNumPins(PIN_DIR_OUTPUT);
    updateCellOutputs(CellPtr, numOutPins);
    width = (*CellPtr).CellGetWidth();
    height = (*CellPtr).CellGetHeight();
    area = (*CellPtr).CellGetArea();
    if (area > max_area) max_area = area;
    if (width > max_width) max_width = width;
    if (height > max_height) max_height = height;

    if (widthRanges.find(width) != widthRanges.end()) {
      widthRanges[width]++;
    } else {
      widthRanges[width] = 1;
    }
    if (areaRanges.find(area) != areaRanges.end()) {
      areaRanges[area]++;
    } else {
      areaRanges[area] = 1;
    }
    if (heightRanges.find(height) != heightRanges.end()) {
      heightRanges[height]++;
    } else {
      heightRanges[height] = 1;
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
	outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + "Outputs" +  + ".txt";
	outputFile.open(outputFileName.data(), ifstream::out);
	MAP_FOR_ALL_ELEMS(mapSelect, unsigned int, unsigned long, numInputs, count) {
	  outputFile << count << "   " << numInputs << endl;
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
	/* Hack for macro substitution */
	MAP_FOR_ALL_ELEMS(mapSelect, unsigned int, vector<map<unsigned int MCOMMA unsigned int > >, numInputs, myVector) {
	  ofstream outputFile;
	  string outputFileName;
	  unsigned int width, height;

	  map<unsigned int, unsigned int>& subMapSelect1 = myVector[PROP_HEIGHT];
	  outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + 
	    "Outputs" + getStrFromInt(numInputs) + "Inputs" + "Heights" + ".txt";
	  outputFile.open(outputFileName.data(), ifstream::out);
	  MAP_FOR_ALL_ELEMS(subMapSelect1, unsigned int, unsigned int, height, count) {
	    outputFile << count << "  " << height << endl;
	  } END_FOR;
	  outputFile.close();

	  map<unsigned int, unsigned int>& subMapSelect2 = myVector[PROP_WIDTH];
	  outputFileName = DesignDir + "/" + "DesignCell" + getStrFromInt(i) + 
	    "Outputs" + getStrFromInt(numInputs) + "Inputs" + "Widths" + ".txt";
	  outputFile.open(outputFileName.data(), ifstream::out);
	  MAP_FOR_ALL_ELEMS(subMapSelect2, unsigned int, unsigned int, width, count) {
	    outputFile << count << "  " << width << endl;
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
    outputFile.open("DesignCellWidthAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(widthRanges, unsigned int, unsigned long, width, count) {
      outputFile << count << "   " << width << endl;
    } END_FOR;
    outputFile.close();
  }

  {
    ofstream outputFile;
    outputFile.open("DesignCellHeightAnalysis.txt");
    unsigned int count;
    MAP_FOR_ALL_ELEMS(heightRanges, unsigned int, unsigned long, height, count) {
      outputFile << count << "   " << height << endl;
    } END_FOR;
    outputFile.close();
  }

  {
    ofstream outputFile;
    outputFile.open("DesignCellAreaAnalysis.txt");
    unsigned int count;
  /* Have 20 ranges 
     0-100
     100-500
     500-1000
     1000-2000
     2000-3000
     3000-4000
     4000-5000
     5000-6000
     6000-7000
     7000-8000
     8000-9000
     9000-10000
     10000-50000
     50000-100000
     100000-1000000
     1000000+
   */
    string areaRangeStr[17], areaRange[17];
    areaRangeStr[0] = "0-100";
    areaRangeStr[1] = "100-500";
    areaRangeStr[2] = "500-1000";
    areaRangeStr[3] = "1000-2000";
    areaRangeStr[4] = "2000-3000";
    areaRangeStr[5] = "3000-4000";
    areaRangeStr[6] = "4000-5000";
    areaRangeStr[7] = "5000-6000";
    areaRangeStr[8] = "6000-7000";
    areaRangeStr[9] = "7000-8000";
    areaRangeStr[10] = "8000-9000";
    areaRangeStr[11] = "9000-10000";
    areaRangeStr[12] = "10000-50000";
    areaRangeStr[13] = "50000-100000";
    areaRangeStr[14] = "100000-500000";
    areaRangeStr[15] = "500000-1000000";
    areaRangeStr[16] = "1000000+";

    MAP_FOR_ALL_ELEMS(areaRanges, unsigned int, unsigned long, area, count) {
      if (area > 0 && area <= 100) {
	areaRange[0] += count;
      } else if (area > 100 && area <= 500) {
	areaRange[1] += count;
      } else if (area > 500 && area <= 1000) {
	areaRange[2] += count;
      } else if (area > 1000 && area <= 2000) {
	areaRange[3] += count;
      } else if (area > 2000 && area <= 3000) {
	areaRange[4] += count;
      } else if (area > 3000 && area <= 4000) {
	areaRange[5] += count;
      } else if (area > 4000 && area <= 5000) {
	areaRange[6] += count;
      } else if (area > 5000 && area <= 6000) {
	areaRange[7] += count;
      } else if (area > 6000 && area <= 7000) {
	areaRange[8] += count;
      } else if (area > 7000 && area <= 8000) {
	areaRange[9] += count;
      } else if (area > 8000 && area <= 9000) {
	areaRange[10] += count;
      } else if (area > 9000 && area <= 10000) {
	areaRange[11] += count;
      } else if (area > 10000 && area <= 50000) {
	areaRange[12] += count;
      } else if (area > 50000 && area <= 100000) {
	areaRange[13] += count;
      } else if (area > 100000 && area <= 500000) {
	areaRange[14] += count;
      } else if (area > 500000 && area <= 1000000) {
	areaRange[15] += count;
      } else if (area > 1000000) {
	areaRange[16] += count;
      }
    } END_FOR;
    for (int i=0; i < 17; i++) {
      outputFile << i << "   " << areaRangeStr[i] << "  " << areaRange[i] << endl;
    }
    outputFile.close();
  }
}

