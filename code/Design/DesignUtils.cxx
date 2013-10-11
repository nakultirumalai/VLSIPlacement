# include <Design.h>

using namespace std;

double thetaFunc(uint L1, uint R1, uint L2, uint R2)
{
  uint left, right;
  double overlap;

  right = R2;
  if (R1 < R2) right = R1;
  left = L2;
  if (L1 > L2) left = L1;

  overlap = (int)(right - left);
  if (overlap < 0) overlap = 0.0;

  return (overlap);
}

void
Design::DesignProcessProperty(ifstream& file, string& Property, 
			      string& Value)
{
  string line;
  unsigned int numNodes, numTerms;
  int idx1, idx2, idx3;
  bool result;

  if (!file.eof()) getline(file, line);

  while (line == "" && !file.eof()) 
    getline(file, line);

  idx1 = 0;
  while (line[idx1] == ' ' || line[idx1] == '\t') 
    idx1++;

  idx2 = idx1;
  while (line[idx2] != ' ' && line[idx2] != '\t') 
    idx2++;

  idx3 = idx2;
  while (!(line[idx3] >= ZERO && line[idx3] <= NINE)) 
    idx3++;
  
  Property = line.substr(idx1, (idx2 - idx1));
  Value = line.substr(idx3);
}


void
Design::DesignFileReadHeader(ifstream& file)
{
  string buffer;
  int i;

  i = 0;
  while (!file.eof()) {
    getline(file, buffer);
    if (buffer == "") {
      break;
    }
  }
}

bool
DesignCellIsStdCell(Design &myDesign, Cell &thisCell)
{
  map<unsigned int, unsigned int>rowHeights;
  int rowHeight, cellHeight;
  bool stdCell;

  rowHeights = myDesign.DesignGetRowHeights();
  rowHeight = myDesign.DesignGetSingleRowHeight();
  cellHeight = thisCell.CellGetHeight();
  stdCell = false;

  if (rowHeight == -1) {
    if (rowHeights.find(cellHeight) != rowHeights.end()) {
      stdCell = true;
    }
  } else if (cellHeight == rowHeight) {
    stdCell = true;
  }

  return (stdCell);
}

vector<Cell *>
Design::DesignGetCellsOfBin(Bin *binPtr, uint left, uint right, uint bot, uint top,
                            vector<Cell *> &cellsSortedByLeft,
                            vector<Cell *> &cellsSortedByBot,
                            double &overLapArea,
                            double &totalCellWidth)
{
  uint last, sortedListsSize, lastPos;
  uint cellLeftPos, cellRightPos, cellTopPos, cellBotPos;
  uint idxLeft, idxRight, idxBot, idxTop;
  uint cellLeftXpos, cellRightXpos;
  uint cellTopYpos, cellBotYpos;
  double thisCellOverlapArea;
  bool noCellFound;
  vector<Cell *> listOfCells;

  overLapArea = 0.0;
  totalCellWidth = 0.0;
  /* Compute the overlap here after checking the y-positions of the cells in a 
     detailed manner */
  for (int i = 0; i < cellsSortedByLeft.size(); i++) {
    Cell &thisCell = *(cellsSortedByLeft[i]);
    thisCell.CellGetBoundingBox(cellLeftPos, cellBotPos, cellRightPos,
                                cellTopPos);
    thisCellOverlapArea = thetaFunc(left, right, cellLeftPos, cellRightPos) *
      thetaFunc(bot, top, cellBotPos, cellTopPos);
    if (thisCellOverlapArea != 0.0) {
      if ((cellLeftPos >= left && cellLeftPos < right) &&
          (cellBotPos >= bot && cellBotPos < top)) {
        listOfCells.push_back(&thisCell);
        CellSetBin(&thisCell, (void *)binPtr);
        totalCellWidth += thisCell.CellGetWidth();
      }
      overLapArea += thisCellOverlapArea;
    }
  }

  return (listOfCells);
}

void
Design::DesignPlotData(string plotTitle, string plotFileName)
{
  Cell *cellPtr;
  vector<Cell *> cellsToSolve;
  string cellName;
  Bin *binPtr;
  uint binIdx;

  Plot newPlot(plotTitle, plotFileName);
  newPlot.PlotSetBoundary(*this);

  /*
  Cell *cell1 = DesignGetNode("o90");  listOfCells.push_back(cell1);
  Cell *cell2 = DesignGetNode("o89");  listOfCells.push_back(cell2);
  Cell *cell3 = DesignGetNode("o2138");  listOfCells.push_back(cell3);
  Cell *cell4 = DesignGetNode("o1778");   listOfCells.push_back(cell4);
  Cell *cell5 = DesignGetNode("o91");   listOfCells.push_back(cell5);
  Cell *cell6 = DesignGetNode("o1986"); listOfCells.push_back(cell6);
  Cell *cell7 = DesignGetNode("o92"); listOfCells.push_back(cell7);
  */

  map<string, Cell*> DesignCells = (*this).DesignGetCells();
  map<string, Cell*> DesignClusters = (*this).DesignGetClusters();
  DesignCells.insert(DesignClusters.begin(), DesignClusters.end());
  map<string, Cell*>::iterator mapIter;
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) {
    cellName = mapIter->first;
    cellPtr = mapIter->second;
    if ((*cellPtr).CellIsHidden()) continue;
    if ((*cellPtr).CellIsTerminal()) {
      continue;
    }
    /*
    if (cellName != "o90" && cellName != "o89" &&
	cellName != "o2138" && cellName != "o1778" &&
	cellName != "o91" && cellName != "o1986" &&
	cellName != "o92" && cellName != "o1987" &&
	cellName != "o0") {
	continue;
    }
    */
    //    if (cellName != "c2741") {
    //      continue;
    //    }
    cellsToSolve.push_back(cellPtr);
  }  

  newPlot.PlotAddCells(cellsToSolve);
  
  DESIGN_FOR_ALL_BINS((*this), binIdx, binPtr) {
    newPlot.PlotAddBin(*binPtr);
  } DESIGN_END_FOR;
  
  newPlot.PlotWriteOutput();
}

void
Design::DesignPlotDataSelected(string plotTitle, string plotFileName,
			       vector<Cell *>& cellsToSolve)
{
  Cell *cellPtr;
  string cellName;
  uint binIdx;

  //  VECTOR_FOR_ALL_ELEMS(cellsToSolve, Cell*, cellPtr) {
    //    cout << " CELL: " << (*cellPtr).CellGetName() << " Address: " << cellPtr 
    //	 << "  has position: " << (*cellPtr).CellGetXpos() << ", " << (*cellPtr).CellGetYpos() << endl;
  //  } END_FOR;

  Plot newPlot(plotTitle, plotFileName);
  newPlot.PlotSetBoundary(*this);
  newPlot.PlotAddCells(cellsToSolve);
  newPlot.PlotWriteOutput();
}

void
Design::DesignClearPseudoNetWeights(void)
{
  Net *pseudoNetPtr;
  uint idx;
  uint numPseudoNets;
  
  numPseudoNets = PseudoNets.size();
  for (idx = 0; idx < numPseudoNets; idx++) {
    pseudoNetPtr = PseudoNets[idx];
    (*pseudoNetPtr).NetSetWeight(0.0);
  }
}

/*********************************************************************
   Given a cell pointer, uses the hypergraph to get a list of 
   connected cells.
*********************************************************************/
vector<Cell*>
DesignGetConnectedCells(HyperGraph &myGraph, Cell *cellPtr)
{
  uint count;
  vector<void*> connectedCells;
  vector<Cell*> returnCells;
  map<Cell*, bool> addedCells;
  Cell *connectedCellPtr;

  count = 0;
  connectedCells = myGraph.GetConnectedNodes((void *)cellPtr);
  VECTOR_FOR_ALL_ELEMS(connectedCells, Cell*, connectedCellPtr) {
    Cell &thisCell = *connectedCellPtr;
    if (addedCells.find(connectedCellPtr) != addedCells.end()) {
      _ASSERT_TRUE("DUPLICATES FOUND!!!");
      exit(0);
      continue;
    }
    if (thisCell.CellIsHidden()) {
      continue;
    }
    if (thisCell.CellIsTerminal()) {
      continue;
    }
    returnCells.push_back(connectedCellPtr);
    count++;
    if (count == 3) {
      break;
    }
  } END_FOR;

  return (returnCells);
}

void
getNTUPlaceGlobalPlacementTime(string DesignName, double &globalPlacementTime)
{
  ifstream ifile;
  string line, garbage;
  string placerTimeLog;
  string timeSubStr;
  int pos;

  globalPlacementTime = 0;
  placerTimeLog = DesignName + "_NTUPlaceTimeLog";

  if (placerTimeLog != "") {
    ifile.open(placerTimeLog.data());
    while (!ifile.eof()) {
      getline(ifile, line);
      pos = line.find("user");
      if (pos != line.npos) {
        timeSubStr = line.substr(0, pos);
        istringstream stream(line, istringstream::in);
        stream >> globalPlacementTime;
        break;
      }
    }
    ifile.close();
  }
}

void
getMPL6GlobalPlacementTime(string DesignName, double &globalPlacementTime)
{
  ifstream ifile;
  string line, garbage;
  string placerTimeLog;
  string timeSubStr;
  int pos;

  globalPlacementTime = 0;
  placerTimeLog = DesignName + "_MPL6TimeLog";

  if (placerTimeLog != "") {
    ifile.open(placerTimeLog.data());
    while (!ifile.eof()) {
      getline(ifile, line);
      pos = line.find("user");
      if (pos != line.npos) {
        timeSubStr = line.substr(0, pos);
        istringstream stream(line, istringstream::in);
        stream >> globalPlacementTime;
        break;
      }
    }
    ifile.close();
  }
}

int 
Design::DesignRunKHMetis(string graphFileName, uint numWays, uint UBfactor,
			 uint Nruns, uint CType, uint OType, uint VCycle, 
			 uint dbglvl)
{
  int status;
  char *hmetisPath; 
  string hmetisCommand;
  string DesignName;
  char path[PATH_MAX];
  FILE *progRun;

  Env &DesignEnv = DesignGetEnv();
  hmetisPath = getenv("KHMETIS_FULL_PATH");

  if (hmetisPath == NIL(char *)) {
    hmetisCommand = "~/Downloads/Tools/Partitioning/hmetis-1.5-linux/khmetis";
  } else {
    hmetisCommand = hmetisPath;
  }

  DesignName = DesignGetName();
  hmetisCommand += " ./" + graphFileName;
  hmetisCommand += " " + getStrFromInt(numWays);
  hmetisCommand += " " + getStrFromInt(UBfactor);
  hmetisCommand += " " + getStrFromInt(Nruns);
  hmetisCommand += " " + getStrFromInt(CType);
  hmetisCommand += " " + getStrFromInt(OType);
  hmetisCommand += " " + getStrFromInt(VCycle);
  hmetisCommand += " " + getStrFromInt(dbglvl);
  hmetisCommand += " | tee " + DesignName + "_KHmetisLog";
  cout << "Executing khmetis command: " << hmetisCommand << endl;
  status = executeCommand(hmetisCommand);

  /* READ THE CLUSTERS HERE */
  //  oldPlFileName = this->DesignPlFileName;
  //  this->DesignPlFileName = clusterDesName + ".gp.pl";
  //  DesignReadCellPlacement(true);
  //  this->DesignPlFileName = oldPlFileName;

  //  changeDir("..");

  return (status);
}

int 
Design::DesignRunKHMetis2(string graphFileName, bool recursiveBisection,
			  string rType, string cType, string oType,
			  double UBfactor, uint Nruns, uint NVCycle, 
			  uint Nparts, uint dbglvl)
{
  int status;
  char *hmetisPath; 
  string hmetisCommand;
  string DesignName;
  char path[PATH_MAX];
  FILE *progRun;

  Env &DesignEnv = DesignGetEnv();
  hmetisPath = getenv("KHMETIS2_FULL_PATH");

  if (hmetisPath == NIL(char *)) {
    hmetisCommand = "~/Downloads/Tools/Partitioning/hmetis-2.0/hmetis-2.0";
  } else {
    hmetisCommand = hmetisPath;
  }

  DesignName = DesignGetName();
  hmetisCommand += " -ptype ";
  if (recursiveBisection == true) {
    hmetisCommand += " rb";
  } else {
    hmetisCommand += "kway";
  }
  hmetisCommand += " -ctype " + cType;
  //  hmetisCommand += " -rtype " + rType;
  hmetisCommand += " -otype " + oType;
  hmetisCommand += " -ufactor " + getStrFromDouble(UBfactor);
  hmetisCommand += " -nruns " + getStrFromInt(Nruns);
  hmetisCommand += " -nvcycles " + getStrFromInt(NVCycle);
  hmetisCommand += " -seed 5 ";
  hmetisCommand += " -dbglvl " + getStrFromInt(dbglvl);
  hmetisCommand += " ./" + graphFileName;
  hmetisCommand += " " + getStrFromInt(Nparts);
  hmetisCommand += " | tee " + DesignName + "_KHmetis2Log";
  cout << "Executing khmetis command: " << hmetisCommand << endl;
  status = executeCommand(hmetisCommand);

  cout << "Finished executing k-way partitioning" << endl;
  /* READ THE CLUSTERS HERE */
  //  oldPlFileName = this->DesignPlFileName;
  //  this->DesignPlFileName = clusterDesName + ".gp.pl";
  //  DesignReadCellPlacement(true);
  //  this->DesignPlFileName = oldPlFileName;

  //  changeDir("..");

  return (status);
}

int 
Design::DesignRunNTUPlace(string clusterDirName, string clusterDesName, 
			  double &globalPlacementTime, bool changeDirectory,
			  bool readPlacementTime, bool silent, bool flat,
			  string &placerLogFile)
{
  int status;
  char *placerPath; 
  string placerCommand, oldPlFileName;
  string placerTimeLogFile;
  string DesignName;
  Env &DesigEnv = DesignGetEnv();

  placerPath = getenv("NTUPLACE_FULL_PATH");
  if (changeDirectory) {
    changeDir(clusterDirName);
  }

  placerCommand = "{ time ";
  if (placerPath == NIL(char *)) {
    placerCommand += "~/Downloads/ntuplace/NTUPlace3/ntuplace3";
  } else {
    string pathFromEnv(placerPath);
    placerCommand += pathFromEnv;
  }

  placerLogFile = clusterDesName + "_NTUPlaceLog";
  placerTimeLogFile = clusterDesName + "_NTUPlaceTimeLog";
  if (silent) {
    placerCommand += " -aux ./" + clusterDesName + ".aux > " + placerLogFile + "; } 2> " + placerTimeLogFile;
  } else {
    placerCommand += " -aux ./" + clusterDesName + ".aux | tee " + placerLogFile + "; } 2> " + placerTimeLogFile;
  }
  status = executeCommand(placerCommand);
  
  getNTUPlaceGlobalPlacementTime(clusterDesName, globalPlacementTime);
  //  DesignEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
  if (readPlacementTime) {
    oldPlFileName = this->DesignPlFileName;
    if (flat) {
      this->DesignPlFileName = clusterDesName + ".ntup.pl";
    } else {
      this->DesignPlFileName = clusterDesName + ".gp.pl";
    }
    DesignReadCellPlacement(true);
    this->DesignPlFileName = oldPlFileName;
  }
  if (changeDirectory) {
    changeDir("..");
  }

  return (status);
}

int 
Design::DesignRunFastPlace(string clusterDirName, string clusterDesName,
			   double &globalPlacementTime, bool changeDirectory,
			   bool readPlacementTime, bool silent, bool flat,
			   string &placerLogFile)
{
  int status;
  char *placerPath; 
  string placerCommand;
  string oldPlFileName;
  string DesignName;
  clock_t start, end;
  time_t timer1, timer2;
  double cpuTimeSpent;
  string placerTimeLogFile;

  placerPath = getenv("FASTPLACE_GP_FULL_PATH");
  if (changeDirectory) {
    changeDir(clusterDirName);
  }

  placerCommand = "{ time ";
  if (placerPath == NIL(char *)) {
    placerCommand += "~/Downloads/FastPlace/FastPlace3.1_Linux64/FastPlace3.1_Linux64_GP";
  } else {
    string pathFromEnv(placerPath);
    placerCommand += pathFromEnv;
  }

  placerLogFile = clusterDesName + "_FastPlaceLog";
  placerTimeLogFile = clusterDesName + "_FastPlaceTimeLog";
  time(&timer1);
  if (silent) {
    placerCommand += " . " + clusterDesName + ".aux . > " + placerLogFile + "; } 2> " + placerTimeLogFile ;
  } else {
    //    placerCommand += " . " + clusterDesName + ".aux . | tee " + DesignName + "_FastPlaceGPLog";
    //    placerCommand += " . " + clusterDesName + ".aux .";
    placerCommand += " . " + clusterDesName + ".aux . | tee " + DesignName + "_FastPlaceGPLog" + "; } 2> " + placerTimeLogFile;
  }
  status = executeCommand(placerCommand);
  time(&timer2);
  globalPlacementTime = difftime(timer2, timer1);
  if (readPlacementTime) {
    oldPlFileName = this->DesignPlFileName;
    this->DesignPlFileName = clusterDesName + "_FP_gp.pl";
    DesignReadCellPlacement(true);
    this->DesignPlFileName = oldPlFileName;
  }
  if (changeDirectory) {
    changeDir("..");
  }

  return (status);
}

int 
Design::DesignRunMPL6(string dirName, string designName,
		      double &globalPlacementTime, bool changeDirectory,
		      bool readPlacement, bool silent, bool flat)
{
  int status;
  char *placerPath; 
  string placerCommand, oldPlFileName;
  string placerLogFile, placerTimeLogFile;
  string DesignName;

  placerPath = getenv("MPL6_FULL_PATH");
  if (changeDirectory) {
    changeDir(dirName);
  }
  placerCommand = "{ time ";
  if (placerPath == NIL(char *)) {
    placerCommand += "~/Downloads/mPL6-release/mPL6";
  } else {
    string pathFromEnv(placerPath);
    placerCommand += pathFromEnv;
  }
  
  DesignName = DesignGetName();
  placerLogFile = designName + "_MPL6Log";
  placerTimeLogFile = designName + "_MPL6TimeLog";
  if (silent) {
    placerCommand += " -d ./" + designName + ".aux > " + placerLogFile + "; } 2> " + placerTimeLogFile;
  } else {
    placerCommand += " -d ./" + designName + ".aux | tee " + placerLogFile + "; } 2> " + placerTimeLogFile;
  }
  //  cout << "PLACER COMMAND: " << placerCommand << endl;
  status = executeCommand(placerCommand);
  getMPL6GlobalPlacementTime(designName, globalPlacementTime);
  //  DesignEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
  if (readPlacement) {
    oldPlFileName = this->DesignPlFileName;
    if (flat) {
      this->DesignPlFileName = designName + "-mPL.pl";
    } else {
      this->DesignPlFileName = designName + "-mPL-gp.pl";
    }
    DesignReadCellPlacement(true);
    this->DesignPlFileName = oldPlFileName;
  }
  if (changeDirectory) {
    changeDir("..");
  }

  return (status);
}

int 
Design::DesignRunFastPlaceLegalizer(string desDirName, string desName, 
				    bool readPlacement, bool changeDirectory)
{
  int status;
  char *placerPath; 
  string placerCommand;
  string oldPlFileName;

  placerPath = getenv("FAST_PLACE_LEGALIZE_FULL_PATH");
  if (changeDirectory) {
    changeDir(desDirName);
  }

  if (placerPath == NIL(char *)) {
    placerCommand = "~/Downloads/FastPlace/FastPlace3.1_Linux64/FastPlace3.1_Linux64_DP -legalize -noDp";
  } else {
    placerCommand = placerPath;
  }
  
  placerCommand += " . " + desName + ".aux . " + desName + ".pl " + " | tee FastPlaceLegalize";
  status = executeCommand(placerCommand);

  if (readPlacement) {
    oldPlFileName = this->DesignPlFileName;
    this->DesignPlFileName = desName + "_FP_lg.pl";
    DesignReadCellPlacement(true);
    this->DesignPlFileName = oldPlFileName;
  }
  if (changeDirectory) {
    changeDir("..");
  }

  return (status);
}

int 
Design::DesignRunFastPlaceLegalizerForCluster(string desDirName, string desName, 
					      string plFileName)
{
  int status;
  char *placerPath; 
  string placerCommand;
  string oldPlFileName;

  placerPath = getenv("FAST_PLACE_LEGALIZE_FULL_PATH");

  if (placerPath == NIL(char *)) {
    placerCommand = "~/Downloads/FastPlace/FastPlace3.1_Linux64/FastPlace3.1_Linux64_DP -legalize -noDp";
  } else {
    placerCommand = placerPath;
  }
  
  placerCommand += " . " + desName + ".aux . " + plFileName + " | tee FastPlaceLegalize";
  status = executeCommand(placerCommand);

  return (status);
}

int 
Design::DesignRunFastPlaceDetailedPlacer(string desDirName, string desName,
					 bool readPlacement, bool changeDirectory)
{
  int status;
  char *placerPath; 
  string placerCommand;
  string oldPlFileName;

  placerPath = getenv("FAST_PLACE_LEGALIZE_FULL_PATH");
  if (changeDirectory) {
    changeDir(desDirName);
  }

  if (placerPath == NIL(char *)) {
    placerCommand = "~/Downloads/FastPlace/FastPlace3.1_Linux64/FastPlace3.1_Linux64_DP -window 10 -noFlipping ";
  } else {
    placerCommand = placerPath;
  }
  
  placerCommand += " . " + desName + ".aux . " + desName + ".pl " + " | tee FastPlaceDP";
  status = executeCommand(placerCommand);

  if (readPlacement) {
    oldPlFileName = this->DesignPlFileName;
    this->DesignPlFileName = desName + "_FP_dp.pl";
    DesignReadCellPlacement(true);
    this->DesignPlFileName = oldPlFileName;
  }
  if (changeDirectory) {
    changeDir("..");
  }

  return (status);
}

/*********************************************************************
  COMPUTES THE FORCE ON A CELL. THE TOTAL X AND Y FORCE ARE RETURNED.
  THE TOTAL MAGNITUDE ON THE FORCE IS ALSO RETURNED. 

  IN COMPUTING THE FORCE, NETS AS KEYS IN SKIPNETS ARE SKIPPED.
*********************************************************************/
void
Design::DesignComputeForceOnCell(Cell *thisCell, double &totalXForce, 
				 double &totalYForce, double &magnitude,
				 char &forceDir,
				 double &chipBoundRight, double &chipBoundLeft, 
				 double &chipBoundTop, double &chipBoundBot,
				 uint minx, uint miny, uint maxx, uint maxy,
				 map<Net *, uint>& skipNets)
{
  Pin *pini, *pinj;
  double celliXPos, celliYPos, celljXPos, celljYPos;
  double piniXOffset, piniYOffset, pinjXOffset, pinjYOffset;
  double diffCellXPos, diffCellYPos, diffPinXOffPos, diffPinYOffPos;
  double totalXDiffDist, totalYDiffDist;
  double netWeight;

  totalXForce = 0.0;
  totalYForce = 0.0;
  magnitude = 0.0;
  forceDir = FORCE_DIR_NO_FORCE;
  chipBoundLeft = 0;
  chipBoundRight = 0;
  chipBoundTop = 0;
  chipBoundBot = 0;
  diffCellXPos = 0.0;
  diffCellYPos = 0.0;

  celliXPos = (*thisCell).CellGetXpos();
  celliYPos = (*thisCell).CellGetYpos();
  /* CAN CHANGE WITH NET MODEL */
  CELL_FOR_ALL_PINS((*thisCell), PIN_DIR_ALL, pini) {
    Net &connectedNet = (*pini).PinGetNet();
    netWeight = connectedNet.NetGetWeight();
    piniXOffset = (*pini).PinGetXOffset();
    piniYOffset = (*pini).PinGetYOffset();
    _KEY_EXISTS(skipNets, &connectedNet) {
      continue;
    }
    NET_FOR_ALL_PINS(connectedNet, pinj) {
      Cell &connectedCell = ((*pinj).PinGetParentCell());
      if (pinj == pini) continue;
      pinjXOffset = (*pinj).PinGetXOffset();
      pinjYOffset = (*pinj).PinGetYOffset();
      diffCellXPos -= netWeight * (celliXPos + piniXOffset);
      diffCellYPos -= netWeight * (celliYPos + piniYOffset);
      if (!connectedCell.CellIsTerminal()) {
        celljXPos = CellGetDblX(&connectedCell);
        celljYPos = CellGetDblY(&connectedCell);
      } else {
        celljXPos = connectedCell.CellGetXpos();
        celljYPos = connectedCell.CellGetYpos();
      }
      diffCellXPos += netWeight * (celljXPos + pinjXOffset);
      diffCellYPos += netWeight * (celljYPos + pinjYOffset);
    } NET_END_FOR;
  } CELL_END_FOR;

  totalXForce = diffCellXPos;
  totalYForce = diffCellYPos;
  int intXForce, intYForce;
  intXForce = dtoi(totalXForce); intYForce = dtoi(totalYForce);
  if (intXForce == 0 && intYForce == 0) {
    totalXForce = intXForce;
    totalYForce = intYForce;
    return;
  }
  magnitude = sqrt((totalXForce * totalXForce + totalYForce * totalYForce));
  /* Find the equation of the line y = mx + c and its intersection 
     with the chip boundaries */
  double m, c;
  if (intXForce == 0) {
    m = 0;
    chipBoundLeft = -MAX_DBL;
    chipBoundRight = -MAX_DBL;
    chipBoundTop = maxx;
    chipBoundBot = 0.0;
    if (intYForce > 0) {
      forceDir = FORCE_DIR_TOP;
    } else if (intYForce < 0) {
      forceDir = FORCE_DIR_BOT;
    }
  } else if (intYForce == 0)  {
    chipBoundTop = -MAX_DBL;
    chipBoundBot = -MAX_DBL;
    chipBoundLeft = 0.0;
    chipBoundRight = maxy;
    if (intXForce > 0) {
      forceDir = FORCE_DIR_RIGHT;
    } else if (intXForce < 0) {
      forceDir = FORCE_DIR_LEFT;
    }
  } else {
    m = ((double)intYForce) / intXForce;
    c = (celliYPos - m * celliXPos);
    DesignGetBoundingBox(maxx, maxy);
    /* x-Coordinate of the top boundary */
    chipBoundTop = (maxy - c)/m;
    chipBoundLeft = c;
    chipBoundRight = (maxx * m) + c;
    chipBoundBot = -c / m;
    if (intXForce > 0 && intYForce > 0) {
      forceDir = FORCE_DIR_FIRST_QUAD;
    } else if (intXForce < 0 && intYForce > 0) {
      forceDir = FORCE_DIR_SECOND_QUAD;
    } else if (intXForce < 0 && intYForce < 0) {
      forceDir = FORCE_DIR_THIRD_QUAD;
    } else if (intXForce > 0 && intYForce < 0) {
      forceDir = FORCE_DIR_FOURTH_QUAD;
    }
  }
}

void
Design::DesignGetBoundaryPoints(double cellXpos, double cellYpos, double magnitude,
				double &totalXForce, double &totalYForce, 
				char &forceDir, double &chipBoundLeft, double &chipBoundRight, 
				double &chipBoundTop, double &chipBoundBot,
				double &netWeight, 
				uint minx, uint miny, uint maxx, uint maxy,
				double &fpX, double &fpY)
{
  double spreadForce;
  double portXForce, portYForce;
  double springConstant;
  fpX = 0.0;
  fpY = 0.0;
  springConstant = 0.0;

  switch (forceDir) {
  case FORCE_DIR_NO_FORCE: return;
  case FORCE_DIR_LEFT: fpX = maxx;
    fpY = chipBoundRight; break;
  case FORCE_DIR_RIGHT: fpX = minx;
    fpY = chipBoundLeft; break;
  case FORCE_DIR_TOP: fpY = miny;
    fpX = chipBoundBot; break;
  case FORCE_DIR_BOT: fpY = maxy;
    fpX = chipBoundTop; break;
  case FORCE_DIR_FIRST_QUAD:
    if (chipBoundLeft <= miny) {
      fpX = chipBoundBot; fpY = 0.0;
    } else if (chipBoundBot <= minx) {
      fpX = 0; fpY = chipBoundLeft;
    }
    break;
  case FORCE_DIR_SECOND_QUAD:
    if (chipBoundRight <= miny) {
      fpX = chipBoundBot; fpY = miny;
    } else if (chipBoundBot >= maxx) {
      fpX = maxx; fpY = chipBoundRight;
    }
    break;
  case FORCE_DIR_THIRD_QUAD:
    if (chipBoundTop >= maxx) {
      fpX = maxx; fpY = chipBoundRight;
    } else if (chipBoundRight >= maxy) {
      fpX = chipBoundTop; fpY = maxy;
    }
    break;
  case FORCE_DIR_FOURTH_QUAD:
    if (chipBoundLeft >= maxy) {
      fpX = chipBoundTop; fpY = maxy;
    } else if (chipBoundTop <= minx) {
      fpX = minx; fpY = chipBoundLeft;
    }
    break;
  default: cout << "DEFAULT CASE NOT EXPECTED" << endl;
    exit(0);
  };

  portXForce = fpX - cellXpos;
  portYForce = fpY - cellYpos;
  spreadForce = sqrt(portXForce * portXForce + portYForce * portYForce);
  springConstant = magnitude / spreadForce;
}

/*******************************************************************************
  FUNCTION TO WRITE OUT THE CURRENT STATE OF THE NETLIST IN THE BOOKSHELF 
  FORMAT 
*******************************************************************************/
void
Design::DesignWriteCurrentNetlist(string dirName, string desName)
{
  makeDir(dirName);
  changeDir(dirName.data());
  DesignWriteBookShelfOutput((*this), desName);
  changeDir("..");
}

void
Design::DesignPrintSpreadIter(uint itrCount, uint numSolverXIter, uint numSolverYIter) 
{
  double peakUtilization;
  string plotFileName, DesignName;

  peakUtilization = DesignGetPeakUtil();
  DesignName = DesignGetName();
  cout << "Iteration: " << itrCount 
       << " CG-X: " << numSolverXIter 
       << " CG-Y: " << numSolverYIter
       << " Peak Util: " << peakUtilization 
       << " Mem usage: " << getMemUsage() << MEM_USAGE_UNIT
       << " CPU TIME:" << getCPUTime() << CPU_TIME_UNIT << endl;

  //  plotFileName = DesignName + "_" + getStrFromInt(itrCount) + ".plt";
  //  DesignPlotData("Title", plotFileName);
}

bool
Design::DesignBreakSolverPhaseI(void)
{
  double averageUtilization;
  double maxUtilization;
  double peakUtilization;
  bool rtv;
  bool cond1, cond2, cond3;

  /* Compute the average density of all the occupied bins */
  Env &DesignEnv = DesignGetEnv();
  
  averageUtilization = (totalUtilization) / (totalNumOccupiedBins);
  maxUtilization = DesignGetMaxUtil();
  peakUtilization = DesignGetPeakUtil();
  
  rtv = false;
  if (0 && (averageUtilization < (0.05) * maxUtilization)) {
    rtv = true;
  } 

  cond1 = (peakUtilization < 25);
  cond2 = (peakUtilization < ((0.01) * maxUtilization));
  cond3 = (peakUtilization < ((0.05) * maxUtilization));
  if (cond2 || (cond1 && cond3)) {
    rtv = true;
  }
  return rtv;
}

bool
Design::DesignBreakSolverPhaseII(void)
{
  double averageUtilization;
  double maxUtilization;
  double peakUtilization;
  bool rtv;

  /* Compute the average density of all the occupied bins */
  Env &DesignEnv = DesignGetEnv();
  
  averageUtilization = (totalUtilization) / (totalNumOccupiedBins);
  maxUtilization = DesignGetMaxUtil();
  peakUtilization = DesignGetPeakUtil();
  
  rtv = false;
  if (0 && (averageUtilization < (0.05) * maxUtilization)) {
    rtv = true;
  } 
  
  if (peakUtilization < (0.1) * maxUtilization) {
    rtv = true;
  }
  return rtv;
}

int 
Design::DesignRunMetaPlacerCapo(string shapesDirName, string shapesDesName, 
				bool changeDirectory, bool readMPlacementTime, 
				bool silent, string &MPlacerLogFile)
{
  int status;
  string MPlacerPath; 
  string MPlacerCommand, oldPlFileName;
  string MPlacerTimeLogFile;
  string DesignName;
  Env &DesigEnv = DesignGetEnv();

  //placerPath = getenv("NTUPLACE_FULL_PATH");
  MPlacerPath = "~/Downloads/UMpack-45-120708/bin/MetaPl-Capo10.5-Lnx32.exe";
  if (changeDirectory) {
    changeDir(shapesDirName);
  }

  MPlacerCommand = "{ time ";
  /*
  if (placerPath == NIL(char *)) {
    placerCommand += "~/Downloads/ntuplace/NTUPlace3/ntuplace3";
  } else {
    string pathFromEnv(placerPath);
    placerCommand += pathFromEnv;
    }*/

  MPlacerCommand += MPlacerPath;

  MPlacerLogFile = shapesDesName + "_MetaPlacer-CapoLog";
  MPlacerTimeLogFile = shapesDesName + "_MetaPlacer-CapoTimeLog";
  if (silent) {
    MPlacerCommand += " -f ../inputShaping/" + shapesDesName + ".aux -ispd05 -fixOr -useHPWL -noFPEvadeObstacles -skipLegal -saveAsBookshelf " + shapesDesName +  " > " + MPlacerLogFile + "; } 2> " + MPlacerTimeLogFile;
  } else {
    MPlacerCommand += " -f ../inputShaping/" + shapesDesName + ".aux -ispd05 -fixOr -useHPWL -noPrePartFix -noFPEvadeObstacles -smallWS 1 -skipLegal -saveAsBookshelf " + shapesDesName +  " | tee " + MPlacerLogFile + "; } 2> " + MPlacerTimeLogFile;
  }
  status = executeCommand(MPlacerCommand);
  
  //  DesignEnv.EnvRecordGlobalPlacementTime(globalPlacementTime);
  if (readMPlacementTime) {
    oldPlFileName = this->DesignPlFileName;
    this->DesignPlFileName = shapesDesName + ".pl";
    DesignReadCellPlacement(true);
    this->DesignPlFileName = oldPlFileName;
  }
  if (changeDirectory) {
    changeDir("..");
  }

  return (status);
}
