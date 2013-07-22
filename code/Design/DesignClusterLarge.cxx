# include <Design.h>

typedef enum boundaryDirectionType {
  BOUND_TOP,
  BOUND_BOT,
  BOUND_RIGHT,
  BOUND_LEFT
} boundaryDirection;

/* Function to plot the cluster. This will generate two plots.
   The first plot will be an internal view of how the boundary cells
   have been arranged in the cluster. Pins will be plotted with 
   offset here for each cell and boundary pins will be highlighted.
   The second plot will give only the cluster cell and the boundary
   pins */
void
Design::DesignPlotClusterLarge(string plotTitle, string plotFileName,
			       Cell *clusterCell, 
			       map<Cell *, uint> &boundaryCells,
			       vector<Cell *> &clusterCells)
{
  Cell *cellPtr;
  Pin *pinPtr;
  Plot clusterPlot(plotTitle, plotFileName);
  uint numCells, idx;
  uint numClusterPins;
  uint minx, miny, maxx, maxy;
  uint cellXpos, cellYpos;
  uint pinAbsX, pinAbsY;
  uint rowHeight;

  minx = 0; miny = 0; 
  maxx = (*clusterCell).CellGetWidth(); 
  maxy = (*clusterCell).CellGetHeight();

  /* Set the boundary of the plot region */
  clusterPlot.PlotSetBoundary(minx, miny, maxx, maxy);
  numCells = clusterCells.size();
  rowHeight = DesignGetSingleRowHeight();
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = clusterCells[idx];
    cellXpos = (*cellPtr).CellGetXpos();
    cellYpos = (*cellPtr).CellGetYpos();
    clusterPlot.PlotAddCell(*cellPtr);
  }

  clusterPlot.PlotWriteOutput();
}

/* Function to read the cells placed by the external placer */
void
Design::DesignReadPlacerOutput(string outputName, map<string, Cell*> &mapOfCells) 
{
  Cell *cellPtr;
  double xPos, yPos;
  map<string, Cell*>::iterator mapIter;
  ifstream file;
  string line, cellName, orient;
  
  if (!fileExists(outputName)) {
    _ASSERT_TRUE("Error: Cannot open output file name");
  }

  file.open(outputName.data());
  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    if (line.find("#") == 0) {
      continue;
    }
    if (line.find("UCLA") == 0) {
      continue;
    }
    istringstream stream(line, istringstream::in);
    stream >> cellName;
    stream >> xPos;
    stream >> yPos;
    stream >> orient;
    stream >> orient;

    Cell *thisCell = NIL(Cell *);
    objOrient orientation;
    _KEY_EXISTS_WITH_VAL(mapOfCells, cellName, mapIter) {
      thisCell = mapIter->second;
    } else {
      _ASSERT_TRUE("Error: Cell in pl file for cluster not found");
    }
    (*thisCell).CellSetXpos(xPos);
    (*thisCell).CellSetYpos(yPos);

    orientation = N;
    if (orient != "") {
      orientation = getOrientationFromStr(orient);
    }
    (*thisCell).CellSetOrientation(orientation);
  }
}

inline 
void getNTUPlacePlacementData(string placerLog, string placerTimeLog,
			      double &globalPlacementTime,
			      double &HPWL)
{
  ifstream ifile;
  string line, garbage, timeSubStr;
  double globalHPWL, legalHPWL, detailHPWL;
  int pos;

  globalPlacementTime = 0;
  globalHPWL = -1;
  legalHPWL = -1;
  detailHPWL = -1;
  HPWL = 0;
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
  if (placerLog != "") {
    ifile.open(placerLog.data());
    while (!ifile.eof()) {
      getline(ifile, line);
      if (line.find("GLOBAL: Pin") == 0) {
	istringstream stream(line, istringstream::in);
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> globalHPWL;
      } else if (line.find("LEGAL:") == 0) {
	istringstream stream(line, istringstream::in);
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> legalHPWL;
      } else if (line.find("         HPWL=") == 0) {
	istringstream stream(line, istringstream::in);
	stream >> garbage;
	stream >> detailHPWL;
      }
    }
    ifile.close();
  }
  if (detailHPWL > -1) {
    HPWL = detailHPWL;
  } else if (legalHPWL > -1) {
    HPWL = legalHPWL;
  } else if (globalHPWL > -1) {
    HPWL = globalHPWL;
  }
}

inline 
void getFastPlacePlacementData(string placerLog, string placerTimeLog,
			       double &globalPlacementTime,
			       double &HPWL)
{
  ifstream ifile;
  string line, garbage, timeSubStr;
  double xHPWL, yHPWL;
  int pos;

  globalPlacementTime = 0;
  xHPWL = -1;
  yHPWL = -1;
  HPWL = 0;
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
  if (placerLog == "") {
    ifile.open(placerLog.data());
    while (!ifile.eof()) {
      getline(ifile, line);
      if (line.find("# Total Global Placement Time") == 0) {
	istringstream stream(line, istringstream::in);
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> globalPlacementTime;
      } else if (line.find("# Global Placement Wirelength") == 0) {
	istringstream stream(line, istringstream::in);
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> garbage;
	stream >> xHPWL;
	stream >> garbage;
	stream >> garbage;
	stream >> yHPWL;
      }
    }
    ifile.close();
  }
  if (xHPWL > 0 && yHPWL > 0) {
    HPWL = xHPWL + yHPWL;
  }
}

void
getPlacerToPlaceCellsInCluster(string designName, 
			       EnvGlobalPlacerType &placerType)
{
  if (designName == "usb_sie") {
    placerType = ENV_FAST_PLACE_GP;
  } else if (designName == "avr_core") {
    placerType = ENV_FAST_PLACE_GP;
  } else if (designName == "RISC") {
    placerType = ENV_FAST_PLACE_GP;
  } else if (designName == "openfpu64") {
    placerType = ENV_FAST_PLACE_GP;
  } else if (designName == "cordic") {
    placerType = ENV_NTUPLACE_GP;
  } else if (designName == "reedsoldec") {
    placerType = ENV_NTUPLACE_GP;
  } else if (designName == "seq_align") {
    placerType = ENV_NTUPLACE_GP;
  } else if (designName == "pairing") {
    placerType = ENV_NTUPLACE_GP;
  }
  /*
  } else if (designName == "avr_core") {
    placerName = "NTUPlace";    
  } else if (designName == "avr_core") {
    placerName = "NTUPlace";
  } else if (designName == "avr_core") {
    placerName = "NTUPlace";
  } else if (designName == "avr_core") {
  }*/
}

/* Function to place cells inside the cluster using an external placer */
void
Design::DesignPlaceCellsInClusterNew(vector<Cell *> &clusterCells, 
				     map<Cell*, bool> &mapOfCells,
				     map<string, Cell*> &mapOfCellsStr,
				     vector<Net *> &affectedNets,
				     string clusterCellName, uint clusterWidth, 
				     uint clusterHeight, double &clusterPlacementTime,
				     double &HPWL)
{
  double globalPlacementTime;
  uint singleRowHeight, singleSiteWidth;
  string benchName, globalPlacerOpFile, finalOpFile;
  string designName, placerLogFile, placerName;
  string placerTimeLogFile;
  string msg;
  Env &DesignEnv = DesignGetEnv();
  EnvGlobalPlacerType globalPlacer;
  EnvLegalizer legalizer;
  
  singleRowHeight = DesignGetSingleRowHeight();
  singleSiteWidth = DesignGetSingleSiteWidth();
  benchName = "__" + clusterCellName;
  globalPlacementTime = 0;
  clusterPlacementTime = 0;
  HPWL = 0;
  designName = DesignEnv.EnvGetDesignName();

  getPlacerToPlaceCellsInCluster(designName, globalPlacer);

  /* First try to place the cells using the given global placer */
  if (globalPlacer == ENV_NTUPLACE_GP) {
    globalPlacerOpFile = benchName + ".ntup.pl";
    placerLogFile = benchName + "_NTUPlaceLog";
    placerTimeLogFile = benchName + "_NTUPlaceTimeLog";
  } else if (globalPlacer == ENV_FAST_PLACE_GP) {
    globalPlacerOpFile = benchName + "_FP_gp.pl";
    placerLogFile = benchName + "_FastPlaceLog";
    placerTimeLogFile = benchName + "_FastPlaceTimeLog";
  }

  if (!dirExists(benchName)) {
    makeDir(benchName);
    changeDir(benchName.data());
    DesignWriteClusterData(clusterCells, affectedNets, mapOfCells, benchName, 
			   clusterWidth, clusterHeight, singleRowHeight,
			   singleSiteWidth, false);
    changeDir("..");
  }

  if (!DesignEnv.EnvGetPlaceCellsInCluster()) {
    return;
  } 

  changeDir(benchName.data());
  msg = "Error: Placer not executed for cluster: " + benchName 
    + " of design " + designName;
  if (!fileExists(globalPlacerOpFile)) {
    if (globalPlacer == ENV_NTUPLACE_GP) {
      DesignRunNTUPlace(benchName, benchName, globalPlacementTime, 
			false, false, true, placerLogFile);
      if (!fileExists(globalPlacerOpFile)) {
	DesignRunFastPlace(benchName, benchName, globalPlacementTime, 
			   false, false, true, placerLogFile);
	globalPlacerOpFile = benchName + "_FP_gp.pl";
	if (!fileExists(globalPlacerOpFile)) {
	  _ASSERT_TRUE(msg);
	}
      } 
    } else if (globalPlacer == ENV_FAST_PLACE_GP) {
      DesignRunFastPlace(benchName, benchName, globalPlacementTime, 
			 false, false, true, placerLogFile);
      if (!fileExists(globalPlacerOpFile)) {
	DesignRunNTUPlace(benchName, benchName, globalPlacementTime, 
			  false, false, true, placerLogFile);
	globalPlacerOpFile = benchName + ".ntup.pl";
	if (!fileExists(globalPlacerOpFile)) {
	  _ASSERT_TRUE(msg);
	}
      } 
    }
  }

  if (globalPlacer == ENV_NTUPLACE_GP) {
    getNTUPlacePlacementData(placerLogFile, placerTimeLogFile,
			     clusterPlacementTime, HPWL);
  } else if (globalPlacer == ENV_FAST_PLACE_GP) {
    getFastPlacePlacementData(placerLogFile, placerTimeLogFile,
			      clusterPlacementTime, HPWL);
  }
  /* If control of program reaches here, we have to assume that the 
     placement of this cluster is done */
  DesignReadPlacerOutput(globalPlacerOpFile, mapOfCellsStr);
  changeDir("..");
}

/* Function to deduce the shape of the almost square cluster given the allowance */
void
Design::DesignDeduceHeightAndWidth(vector<Cell *> &clusterCells, double xPercent,
				   double heightVarPercent, double totalCellArea,
				   uint &resultHeight, uint &resultWidth, 
				   double &clusterArea)
{
  Cell *cellPtr;
  double rowHeight;
  double multiple;
  double rheight, rwidth;
  uint minWidth, maxWidth;
  uint cellHeight, cellWidth;
  uint numRows, siteWidth;
  bool sizeDebug;

  sizeDebug = false;
  /* Assuming that withGeometry is off here */
  /* Assuming aspect ratio is 1.0 */
  /* Some rules:                                           */
  /* Minimum width of the cluster should be equal to the   */
  /* max width among all cells                             */
  rowHeight = DesignGetSingleRowHeight();
  siteWidth = DesignGetSingleSiteWidth();
  minWidth = INT_MAX; maxWidth = 0; 
  clusterArea = 0.0;
  /* Deduce the number of rows and sites */
  if (0) {
    resultHeight = (uint)ceil(sqrt(totalCellArea));
    multiple = ceil(resultHeight / rowHeight);
    resultHeight = rowHeight * multiple;
    resultWidth = (uint)ceil(totalCellArea / resultHeight);
    multiple = ceil(((double)resultWidth) / siteWidth);
    resultWidth = siteWidth * multiple;
    clusterArea = resultHeight * resultWidth;
  } else {
    clusterArea = totalCellArea + (xPercent / 100) * totalCellArea;
    multiple = ceil(sqrt(clusterArea) / rowHeight);
    rheight = rowHeight * multiple;
    multiple = ceil(sqrt(clusterArea) / siteWidth);
    rwidth = siteWidth * multiple;
    if (heightVarPercent != 0) {
      rheight *= (1 + heightVarPercent / 100);
      multiple = ceil(rheight / rowHeight);
      rheight = rowHeight * multiple;
      rwidth = clusterArea / rheight;
      multiple = ceil(rwidth / siteWidth);
      rwidth = multiple * siteWidth;
    }
    resultHeight = rheight;
    resultWidth = rwidth;
    clusterArea = resultHeight * resultWidth;
  }
  
  /* Print the total cell area and the deduced cluster area */
  if (sizeDebug) {
    double variation;
    variation = ((clusterArea - totalCellArea) / (totalCellArea)) * 100;
    cout << "CLUSTER_DBG: totalCellArea: " << totalCellArea 
	 << " clusterArea: " << clusterArea
	 << " xpercent: " << xPercent 
	 << " actual: ((CLA - TCA) / (TCA)) * 100 : " << variation
	 << endl;
  }
}

//inline
boundaryDirection getPortBoundaryDir(Cell *relatedPort, 
				     uint maxx, uint maxy)
{
  uint cellXpos, cellYpos;
  uint cellWidth, cellHeight;
  boundaryDirection rtv;
  
  cellWidth = (*relatedPort).CellGetWidth();
  cellHeight = (*relatedPort).CellGetHeight();
  cellXpos = (*relatedPort).CellGetXpos();
  cellYpos = (*relatedPort).CellGetYpos();
  if (cellXpos > 0) {
    cellXpos += cellWidth;
  } 
  if (cellYpos > 0) {
    cellYpos += cellHeight;
  }
  rtv = BOUND_LEFT;
  if (cellXpos == 0) {
    rtv = BOUND_LEFT;
  } else if (cellXpos == maxx) {
    rtv = BOUND_RIGHT;
  } else if (cellYpos == 0) {
    rtv = BOUND_BOT;
  } else if (cellYpos == maxy) {
    rtv = BOUND_TOP;
  } else {
    _ASSERT_TRUE("DesignClusterLarge.cxx:273");
  }
  return rtv;
}

//inline
boundaryDirection getNextBoundaryDirection(bool cellConnectedToPort,
					   boundaryDirection original, 
					   boundaryDirection current)
{
  boundaryDirection rtv;

  rtv = current;
  if (cellConnectedToPort) {
    /* Juggle is different in this case */
    if (original == BOUND_LEFT) {
      if (current == BOUND_LEFT) rtv = BOUND_BOT;
      else if (current == BOUND_BOT) rtv = BOUND_TOP;
      else if (current == BOUND_TOP) rtv = BOUND_RIGHT;
      else if (current == BOUND_RIGHT) rtv = BOUND_LEFT;
    }
    if (original == BOUND_RIGHT) {
      if (current == BOUND_RIGHT) rtv = BOUND_BOT;
      else if (current == BOUND_BOT) rtv = BOUND_TOP;
      else if (current == BOUND_TOP) rtv = BOUND_LEFT;
      else if (current == BOUND_LEFT) rtv = BOUND_RIGHT;
    }
    if (original == BOUND_TOP) {
      if (current == BOUND_TOP) rtv = BOUND_LEFT;
      else if (current == BOUND_LEFT) rtv = BOUND_RIGHT;
      else if (current == BOUND_RIGHT) rtv = BOUND_BOT;
      else if (current == BOUND_BOT) rtv = BOUND_TOP;
    }
    if (original == BOUND_BOT) {
      if (current == BOUND_BOT) rtv = BOUND_LEFT;
      else if (current == BOUND_LEFT) rtv = BOUND_RIGHT;
      else if (current == BOUND_RIGHT) rtv = BOUND_TOP;
      else if (current == BOUND_TOP) rtv = BOUND_BOT;
    }
  } else {
    /* Juggle in the anti-clockwise direction */
    switch (current) {
    case BOUND_TOP: rtv = BOUND_LEFT; break;
    case BOUND_LEFT: rtv = BOUND_BOT; break;
    case BOUND_BOT: rtv = BOUND_RIGHT; break;
    case BOUND_RIGHT: rtv = BOUND_TOP; break;
    default: _ASSERT_TRUE("Error: Boundary direction! DesignClusterLarge.cxx:289");
    };
  }
  
  return rtv;
}

//inline 
bool placeBoundaryCellHorizontal(Cell *cellPtr,
				 bool direction, uint clusterWidth, 
				 uint cellYpos, uint &leftWidth, 
				 uint &rightWidth, int &availableWidth)
{
  uint cellXpos;
  int cellWidth;
  bool rtv;
  
  cellWidth = (*cellPtr).CellGetWidth();
  cellXpos = 0;
  rtv = true;
  if (direction == true) {
    /* Left direction */
    if (((int)leftWidth) - cellWidth < 0) {
      rtv = false;
    } else {
      cellXpos = leftWidth - cellWidth;
      leftWidth = cellXpos;
    }
  } else if (direction == false) {
    /* Right direction */
    if (((int)rightWidth) + cellWidth > clusterWidth) {
      rtv = false;
    } else {
      cellXpos = rightWidth;
      rightWidth += cellWidth;
    }
  }
  if (rtv == true) {
    /* Place the cell here */
    availableWidth -= cellWidth;
    (*cellPtr).CellSetXpos(cellXpos);
    (*cellPtr).CellSetYpos(cellYpos);
  }
 
  return (rtv);
}

//inline 
bool placeBoundaryCellVertical(Cell *cellPtr, bool direction, 
			       uint singleRowHeight, uint numRows,
			       uint cellXpos,
			       uint &topRow, uint &botRow, 
			       uint &numRemainingRows)
{
  uint cellYpos;
  bool rtv;
  
  rtv = true;
  if (direction == true) {
    if (botRow == 0) {
      rtv = false;
    } else {
      cellYpos = botRow * singleRowHeight;
      botRow -= 1;
    }
  } else if (direction == false) {
    /* Top direction */
    if (topRow == (numRows - 1)) {
      rtv = false;
    } else {
      cellYpos = topRow * singleRowHeight;
      topRow += 1;
    }
  }

  if (rtv == true) {
    /* Place the cell here */
    numRemainingRows -= 1;
    (*cellPtr).CellSetXpos(cellXpos);
    (*cellPtr).CellSetYpos(cellYpos);
  }
 
  return (rtv);
}


bool 
cmpCellInternalConnections(const pair<Cell *, uint>& firstCell, 
			   const pair<Cell *, uint>& secondCell)
{
  return (firstCell.second < secondCell.second);
}

inline 
void createPinsAndAssignOffsets(Cell *clusterCell, Cell *cellPtr, 
				map<Net *, bool> &externalNets, 
				map<Pin *, Pin *> &pinMap, 
				uint clusterCellHeight, 
				uint clusterCellWidth)
{
  Net *netPtr;
  Pin *pinPtr, *newPinPtr;
  string newPinName, clusterCellName;
  uint clusterCellNumPins;
  uint cellXpos, cellYpos;
  uint pinXOffset, pinYOffset;
  
  cellXpos = (*cellPtr).CellGetXpos();
  cellYpos = (*cellPtr).CellGetYpos();  
  clusterCellName = (*clusterCell).CellGetName();
  CELL_FOR_ALL_PINS((*cellPtr), PIN_DIR_ALL, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    netPtr = &connectedNet;
    _KEY_EXISTS(externalNets, netPtr) {
      newPinPtr = new Pin();
      (*newPinPtr).PinSetParentCell(*clusterCell);
      clusterCellNumPins = (*clusterCell).CellGetNumPins();
      newPinName = clusterCellName + "_" + getStrFromInt(clusterCellNumPins);
      (*newPinPtr).PinSetName(newPinName);
      (*newPinPtr).PinSetDirection((*pinPtr).PinGetDirection());
      (*clusterCell).CellAddPin(newPinPtr);
      pinMap[newPinPtr] = pinPtr;
      (*pinPtr).PinSetIsHidden(true);
      (*newPinPtr).Connect(connectedNet);
      connectedNet.NetAddPin(*newPinPtr);
      pinXOffset = (*pinPtr).PinGetXOffset() + cellXpos;
      if (pinXOffset > clusterCellWidth) {
	_ASSERT_TRUE("Error: Pin's x offset goes beyond cluster width");
      }
      pinYOffset = (*pinPtr).PinGetYOffset() + cellYpos;
      if (pinYOffset > clusterCellHeight) {
	_ASSERT_TRUE("Error: Pin's y offset goes beyond cluster width");
      }
    }
  } CELL_END_FOR;
}

//inline
bool placeCellOnBoundary(Cell *cellPtr, 
			 uint clusterWidth, uint clusterHeight, 
			 uint singleRowHeight, uint numRows,
			 bool cellConnectedToPort, boundaryDirection &dir,
			 uint &topRightPoint, uint &topLeftPoint, 
			 bool &topLeft, int &remainingWidthTop,
			 uint &botRightPoint, uint &botLeftPoint, 
			 bool &botLeft, int &remainingWidthBot,
			 uint &leftTopRow, uint &leftBotRow, 
			 bool &leftBot, uint &remainingLeftRows,
			 uint &rightTopRow, uint &rightBotRow, 
			 bool &rightBot, uint &remainingRightRows)
{
  boundaryDirection nextDir;
  uint cellXpos, cellYpos;
  bool cellPlaced, placeDebug;
  
  nextDir = dir;
  cellPlaced = false;
  placeDebug = false;
  //  cout << endl;
  //  cout << " Cell: " << (*cellPtr).CellGetName() 
  //       << " width: " << (*cellPtr).CellGetWidth() << endl;
  //  cout << endl;
  do {
    switch (nextDir) {
    case BOUND_TOP:
      if (placeDebug) {
	cout << "Attempting to place cell " << (*cellPtr).CellGetName() 
	     << " on top boundary on the ";
	if (topLeft == true) {
	  cout << " on the left side " << endl;
	} else {
	  cout << " on the right side " << endl;
	}
	cout << " BEFORE: topLeftPoint: " << topLeftPoint 
	     << " topRightPoint: " << topRightPoint << endl;
      }
      cellYpos = clusterHeight - singleRowHeight;
      cellPlaced = placeBoundaryCellHorizontal(cellPtr, topLeft, clusterWidth, 
					       cellYpos, topLeftPoint, topRightPoint, 
					       remainingWidthTop);
      if (!cellPlaced) {
	topLeft = !topLeft;
	cellPlaced = placeBoundaryCellHorizontal(cellPtr, topLeft, clusterWidth, 
						 cellYpos, topLeftPoint, topRightPoint, 
						 remainingWidthTop);
      }
      if (placeDebug) {
	if (cellPlaced) 
	  cout << "PLACED: Placed cell " << (*cellPtr).CellGetName() 
	       << " x-pos: " << cellXpos << " y-pos: " << cellYpos << endl;
	cout << " AFTER: topLeftPoint: " << topLeftPoint 
	     << " topRightPoint: " << topRightPoint << endl;
      }
      if (cellPlaced) topLeft = !topLeft;
      break;
    case BOUND_LEFT:
      if (placeDebug) {
	cout << "Attempting to place cell " << (*cellPtr).CellGetName() 
	     << " on left boundary on the ";
	if (leftBot == true) {
	  cout << " on the bottom " << endl;
	} else {
	  cout << " on the top " << endl;
	}
	cout << " BEFORE: leftTopRow: " << leftTopRow << " leftBotRow: " << leftBotRow << endl;
      }
      cellXpos = 0;
      cellPlaced = placeBoundaryCellVertical(cellPtr, leftBot, singleRowHeight,
					     numRows, cellXpos, leftTopRow, leftBotRow, 
					     remainingLeftRows);
      if (!cellPlaced) {
	leftBot = !leftBot;
	cellPlaced = placeBoundaryCellVertical(cellPtr, leftBot, singleRowHeight,
					       numRows, cellXpos, leftTopRow, leftBotRow, 
					       remainingLeftRows);
      }
      if (placeDebug) {
	if (cellPlaced) 
	  cout << "PLACED: Placed cell " << (*cellPtr).CellGetName() 
	       << " x-pos: " << cellXpos << " y-pos: " << cellYpos << endl;
	cout << " AFTER: leftTopRow: " << leftTopRow << " leftBotRow: " << leftBotRow << endl;
      }
      if (cellPlaced) leftBot = !leftBot;
      break;
    case BOUND_BOT:
      if (placeDebug) {
	cout << "Attempting to place cell " << (*cellPtr).CellGetName() 
	     << " on top boundary on the ";
	if (botLeft == true) {
	  cout << " on the left side " << endl;
	} else {
	  cout << " on the right side " << endl;
	}
	cout << " BEFORE: botLeftPoint: " << botLeftPoint << " botRightPoint: " << botRightPoint << endl;
      }
      cellYpos = 0;
      cellPlaced = placeBoundaryCellHorizontal(cellPtr, botLeft, clusterWidth, cellYpos, 
					       botLeftPoint, botRightPoint, 
					       remainingWidthBot);
      if (!cellPlaced) {
	botLeft = !botLeft;
	cellPlaced = placeBoundaryCellHorizontal(cellPtr, botLeft, clusterWidth, cellYpos, 
						 botLeftPoint, botRightPoint, 
						 remainingWidthBot);
      }
      if (placeDebug) {
	if (cellPlaced) 
	  cout << "PLACED: Placed cell " << (*cellPtr).CellGetName() 
	       << " x-pos: " << cellXpos << " y-pos: " << cellYpos << endl;
	cout << " AFTER: botLeftPoint: " << botLeftPoint << " botRightPoint: " << botRightPoint << endl;
      }
      if (cellPlaced) botLeft = !botLeft;
      break;
    case BOUND_RIGHT:
      if (placeDebug) {
	cout << "Attempting to place cell " << (*cellPtr).CellGetName() 
	     << " on right boundary on the ";
	if (rightBot == true) {
	  cout << " on the bottom " << endl;
	} else {
	  cout << " on the top " << endl;
	}
	cout << " BEFORE: rightTopRow: " << rightTopRow << " rightBotRow: " << rightBotRow << endl;
      }
      cellXpos = clusterWidth - (*cellPtr).CellGetWidth();
      cellPlaced = placeBoundaryCellVertical(cellPtr, rightBot, singleRowHeight,
					     numRows, cellXpos, rightTopRow, rightBotRow, 
					     remainingRightRows);
      if (!cellPlaced) {
	rightBot = !rightBot;
	cellPlaced = placeBoundaryCellVertical(cellPtr, rightBot, singleRowHeight,
					       numRows, cellXpos, rightTopRow, rightBotRow, 
					       remainingRightRows);
      }
      if (placeDebug) {
	if (cellPlaced) 
	  cout << "PLACED: Placed cell " << (*cellPtr).CellGetName() 
	       << " x-pos: " << cellXpos << " y-pos: " << cellYpos << endl;
	cout << " AFTER: rightTopRow: " << rightTopRow << " rightBotRow: " << rightBotRow << endl;
      }
      if (cellPlaced) rightBot = !rightBot;
      break;
    default: _ASSERT_TRUE("Error: Unknown port direction!");
    };
    if (cellPlaced) break;
    nextDir = getNextBoundaryDirection(cellConnectedToPort, dir, nextDir);
  } while (nextDir != dir);
  
  return (cellPlaced);
}

//inline
void placeBoundaryCells(vector<Cell *> &clusterCells,
			map<Cell *, uint> &boundaryCells,
			uint clusterHeight, uint clusterWidth,
			uint designMaxx, uint designMaxy,
			uint singleRowHeight, uint singleSiteWidth,
			double &placementTime)
{
  Net *netPtr;
  Cell *cellPtr, *relatedPort;
  vector<pair<Cell *, uint> > boundaryCellVec;
  pair<Cell *, uint> thisPair;
  vector<Cell *> cellsWithPortConns;
  map<Cell *, bool> placedCells;
  uint cellXpos, cellYpos;
  uint numInternalConns;
  bool connectedToPort, cellPlaced;
  boundaryDirection portBoundaryDir, nextDirection;

  /* VARIABLES RELATED TO THE BOUNDARY POSITIONS */
  uint topLeftPoint, topRightPoint, botLeftPoint, botRightPoint;
  uint leftTopRow, leftBotRow, numLeftEmptyRows;
  uint rightTopRow, rightBotRow, numRightEmptyRows;
  bool leftBot, rightBot, topLeft, botLeft;
  uint numRows, midRow;
  int topWidth, botWidth;
  
  /* Initialize variables related to placing cells on the boundary */
  numRows = clusterHeight / singleRowHeight;
  topLeftPoint = floor((((double)clusterWidth) / 2) / singleSiteWidth);
  topLeftPoint *= singleSiteWidth;
  topRightPoint = topLeftPoint; 
  botLeftPoint = topLeftPoint; 
  botRightPoint = topLeftPoint; 
  topWidth = clusterWidth; 
  botWidth = clusterWidth;
  numLeftEmptyRows = numRows;
  numRightEmptyRows = numRows;
  midRow = numRows / 2;
  if (numRows % 2 != 0) { 
    midRow = (numRows + 1) / 2; 
  }
  leftTopRow = midRow + 1; leftBotRow = midRow; 
  rightTopRow = midRow + 1; rightBotRow = midRow;
  leftBot = false; rightBot = false; topLeft = false; botLeft = false;
  nextDirection = BOUND_TOP;

  /* Create a vector of pair of boundary cell pointer and the number 
     of internal connections it has */
  MAP_FOR_ALL_ELEMS(boundaryCells, Cell *, uint, cellPtr, numInternalConns) {
    boundaryCellVec.push_back(make_pair(cellPtr, numInternalConns));
    if ((*cellPtr).CellIsAdjacentToPort()) {
      cellsWithPortConns.push_back(cellPtr);
    }
  } END_FOR;
  placementTime = getCPUTime();
  if (0) {
    /* First place boundary cells with port connections */
    VECTOR_FOR_ALL_ELEMS(cellsWithPortConns, Cell*, cellPtr) {
      connectedToPort = false;
      portBoundaryDir = BOUND_LEFT;
      CELL_FOR_ALL_NETS((*cellPtr), PIN_DIR_ALL, netPtr) {
	NET_FOR_ALL_CELLS((*netPtr), relatedPort) {
	  if ((*relatedPort).CellIsPort()) {
	    /* Get which boundary the port is associated with */
	    portBoundaryDir = getPortBoundaryDir(relatedPort, designMaxx, designMaxy);
	    connectedToPort = true;
	    break;
	  }
	} NET_END_FOR;
      } CELL_END_FOR;
      if (!connectedToPort) {
	_ASSERT_TRUE("Error: cellsWithPortConns vector has a cell without a port connection: DesignClusterLarge.cxx:321");
      }
      cellPlaced = placeCellOnBoundary(cellPtr, clusterWidth, clusterHeight, 
				       singleRowHeight, numRows, true, portBoundaryDir,
				       topRightPoint, topLeftPoint, topLeft, topWidth,
				       botRightPoint, botLeftPoint, botLeft, botWidth,
				       leftTopRow, leftBotRow, leftBot, numLeftEmptyRows,
				       rightTopRow, rightBotRow, rightBot, numRightEmptyRows);
      if (cellPlaced == false) {
	string msg;
	//      msg = "Error: Could not place cell " + (*cellPtr).CellGetName() + " connected to a port inside the cluster";
	//      _ASSERT_TRUE(msg);
	//      cout << msg << endl;
	continue;
      } else {
	placedCells[cellPtr] = true;
	(*cellPtr).CellSetIsClusterFixed(true);
      }
    } END_FOR;
  }

  /* Sort the boundary cells on the basis of the number of internal connections */
  sort(boundaryCellVec.begin(), boundaryCellVec.end(), cmpCellInternalConnections);
  VECTOR_FOR_ALL_ELEMS(boundaryCellVec, pair <Cell * MCOMMA uint>, thisPair) {
    cellPtr = thisPair.first;
    //    _KEY_EXISTS(placedCells, cellPtr) {
    //      continue;
    //    }
    cellPlaced = placeCellOnBoundary(cellPtr, clusterWidth, clusterHeight, 
				     singleRowHeight, numRows, false, nextDirection, 
				     topRightPoint, topLeftPoint, topLeft, topWidth,
				     botRightPoint, botLeftPoint, botLeft, botWidth,
				     leftTopRow, leftBotRow, leftBot, numLeftEmptyRows,
				     rightTopRow, rightBotRow, rightBot, numRightEmptyRows);
    if (cellPlaced == false) {
      break;
    } else {
      (*cellPtr).CellSetIsClusterFixed(true);
    }
  } END_FOR;
  placementTime = getCPUTime() - placementTime;
}
				     
inline
void getNetsAndCellsOfCluster(HyperGraph &myGraph, 
			      Cell *clusterCell,
			      double &cellArea,
			      vector<Cell *> &clusterCells,
			      map<Cell *, bool> &internalCells, 
			      map<Cell *, uint> &boundaryCells,
			      map<Net *, bool> &internalNets, 
			      map<Net *, bool> &externalNets,
			      vector<Net *> &affectedNets,
			      map<Pin *, Pin *> &pinMap)
{
  Net *netPtr;
  Pin *pinPtr, *newPinPtr;
  Cell *cellPtr;
  void *netObj;
  map<void *, bool> internalNetObjs;
  map<void *, bool> externalNetObjs;
  string newPinName, clusterCellName;
  uint idx, numCells;
  uint numInternal, numExternal;
  uint clusterNumPins;
  bool boundaryCell;

  numCells = clusterCells.size();
  clusterCellName = (*clusterCell).CellGetName();

  /* Make corresponding changes in the hypergraph */
  vector<void *> listOfNodes(clusterCells.begin(), clusterCells.end());
  myGraph.ClusterNodes(listOfNodes, (void *)clusterCell, externalNetObjs,
                       internalNetObjs);

  /* Add objects in the internal net objs to the internal nets map */
  MAP_FOR_ALL_KEYS(internalNetObjs, void*, bool, netObj) {
    netPtr = (Net *)netObj;
    internalNets[netPtr] = 1;
    affectedNets.push_back(netPtr);
    //    affectedNets[netPtr] = 1;
    //    DesignHideNet(netPtr);
  } END_FOR;
  
  /* Add objects in the external net objs to the external nets map */
  MAP_FOR_ALL_KEYS(externalNetObjs, void*, bool, netObj) {
    netPtr = (Net *)netObj;
    externalNets[netPtr] = 1;
    affectedNets.push_back(netPtr);
    //    affectedNets[netPtr] = 1;
  } END_FOR;
  
  /* Add objects as internal cells and external cells */
  cellArea = 0;
  for (idx = 0; idx < numCells; idx++) {
    boundaryCell = false;
    numInternal = 0;
    numExternal = 0;
    cellPtr = clusterCells[idx];
    Cell &thisCell = *cellPtr;
    CELL_FOR_ALL_PINS(thisCell, PIN_DIR_ALL, pinPtr) {
      Net &thisNet = (*pinPtr).PinGetNet();
      netPtr = &thisNet;
      _KEY_EXISTS(externalNets, netPtr) {
	boundaryCell = true;
	numExternal++;
	/* Create a cluster pin for this pin */
	newPinPtr = new Pin();
        (*newPinPtr).PinSetParentCell(*clusterCell);
	clusterNumPins = (*clusterCell).CellGetNumPins();
        newPinName = clusterCellName + "_" + getStrFromInt((clusterNumPins + 1));
        (*newPinPtr).PinSetName(newPinName);
        (*newPinPtr).PinSetDirection((*pinPtr).PinGetDirection());
        (*clusterCell).CellAddPin(newPinPtr);
	pinMap[newPinPtr] = pinPtr;
      } else {
	numInternal++;
      }
    } CELL_END_FOR;
    cellArea += thisCell.CellGetArea();
    if (boundaryCell) {
      boundaryCells[cellPtr] = numInternal;
    } 
  }
}

inline
void populateShapeVariations(vector<double> &heightVariations, double hPercent, 
			     uint numSteps)
{
  uint idx;
  double stepPercent, percentageChange;
  stepPercent = hPercent / numSteps;
  
  for (idx = numSteps; idx > 0; idx--) {
    percentageChange = stepPercent * idx;
    heightVariations.push_back(-percentageChange);
  }
  for (idx = numSteps; idx > 0; idx--) {
    percentageChange = stepPercent * idx;
    heightVariations.push_back(+percentageChange);
  }
}

void
Design::DesignCreateClusterObject(vector<Cell *> &clusterCells, double xPercent,
				  double hPercent, uint numSteps, 
				  double &clusterPlacementTime)
{
  Cell *cellPtr, *clusterCell;
  Pin *clusterPinPtr, *pinPtr;
  Net *netPtr;
  double cellArea, clusterArea;
  double stepTime, heightVariation;
  double shapeHPWL;
  map<Net *, bool> internalNets, externalNets;
  vector<Net *> internalNetVec;
  map<Cell *, bool> internalCells; 
  map<Cell *, uint> boundaryCells;
  map<Pin *, Pin *> pinMap;
  vector<Net *> affectedNets;
  vector<double> heightVariations;
  vector<double> shapeHPWLVec;
  vector<double> clusterAreas;
  vector<pair<uint, uint> > dimensions;
  HyperGraph &myGraph = DesignGetGraph();
  uint singleRowHeight, singleSiteWidth;
  uint clusterHeight, clusterWidth;
  uint designMaxx, designMaxy;
  uint numClusters, numCells, idx, cellNum;
  uint numHeightVariations;
  string clusterCellName, clusterShapeName;
  
  /* STEPS INVOLVED IN FORMING THE CLUSTER: */
  /* STEP 0: CREATE A NEW CELL FOR THE CLUSTER */
  numClusters = NumClustersSeenSofar++;
  clusterCellName = "c" + getStrFromInt(numClusters + 1);
  numCells = clusterCells.size();
  vector<vector<double> > cellPositions(numCells);
  
  /* Create a new cell */
  clusterCell = new Cell(0, 0, clusterCellName);
  (*clusterCell).CellSetIsCluster(true);
  clusterPlacementTime = 0;

  /* STEP : IDENTIFY INTERNAL NETS, EXTERNAL NETS, INTERNAL CELLS, BOUNDARY CELLS 
     ALSO COMMIT THE CLUSTER IN THE HYPERGRAPH IN THIS STEP. PINS ARE ALSO
     CREATED HERE */
  stepTime = getCPUTime();
  getNetsAndCellsOfCluster(myGraph, clusterCell, cellArea, clusterCells, internalCells, 
			   boundaryCells, internalNets, externalNets, affectedNets,
			   pinMap);
  clusterPlacementTime += getCPUTime() - stepTime;
  populateShapeVariations(heightVariations, hPercent, numSteps);
  /* ADD THE VARIATION FOR THE SQUARE CLUSTER */ 
  heightVariations.push_back(0);
  numHeightVariations = heightVariations.size();
  for (idx = 0; idx < numHeightVariations; idx++) {
    map<Cell *, bool> mapOfCells;
    map<string, Cell*> mapOfCellsString;
    heightVariation = heightVariations[idx];
    shapeHPWL = 0;
    clusterShapeName = clusterCellName + "_" + getStrFromInt(idx);
    /* STEP : DEDUCE THE HEIGHT AND WIDTH OF THE CLUSTER */
    DesignDeduceHeightAndWidth(clusterCells, xPercent, heightVariations[idx],
			       cellArea, clusterHeight, clusterWidth,
			       clusterArea);
    clusterAreas.push_back(clusterArea);
    dimensions.push_back(make_pair(clusterWidth, clusterHeight));
    (*clusterCell).CellSetWidth(clusterWidth);
    (*clusterCell).CellSetHeight(clusterHeight);

    /* STEP : PLACE ALL THE CELLS OF THE CLUSTER AT (0, 0) OF THE CLUSTER */
    VECTOR_FOR_ALL_ELEMS(clusterCells, Cell *, cellPtr) {
      mapOfCells[cellPtr] = true;
      mapOfCellsString[(*cellPtr).CellGetName()] = cellPtr;
      (*cellPtr).CellSetPosDbl(0,0);
      (*cellPtr).CellSetIsClusterFixed(false);
    } END_FOR;
  
    /* STEP : PLACE THE BOUNDARY CELLS IN THE CLUSTER */
    DesignGetBoundingBox(designMaxx, designMaxy);
    singleRowHeight = DesignGetSingleRowHeight();
    singleSiteWidth = DesignGetSingleSiteWidth();
    placeBoundaryCells(clusterCells, boundaryCells,
		       clusterHeight, clusterWidth,
		       designMaxx, designMaxy, singleRowHeight, 
		       singleSiteWidth, stepTime);
    clusterPlacementTime += stepTime;

    /* STEP : EXECUTE A PLACER TO OBTAIN THE PLACEMENT OF ALL CELLS FOR THE CLUSTER */
    DesignPlaceCellsInClusterNew(clusterCells, mapOfCells, mapOfCellsString, affectedNets, 
				 clusterShapeName, clusterWidth, clusterHeight, 
				 stepTime, shapeHPWL);
    shapeHPWLVec.push_back(shapeHPWL);
    clusterPlacementTime += stepTime;
    /* STEP : RECORD THE PLACEMENTS OF THE PLACED CELLS */
    for (cellNum = 0; cellNum < numCells; cellNum++) {
      cellPtr = clusterCells[cellNum];
      cellPositions[cellNum].push_back((*cellPtr).CellGetXpos());
      cellPositions[cellNum].push_back((*cellPtr).CellGetYpos());
    }

    /* STEP : PLOT THE CLUSTER WHICH HAS THE BOUNDARY CELLS PLACED */
    DesignPlotClusterLarge(clusterShapeName, clusterShapeName, clusterCell, 
			   boundaryCells, clusterCells);

    /* STEP : ASSIGN PIN OFFSETS FOR THE CLUSTER */
    stepTime = getCPUTime();
    if (heightVariations[idx] == 0) {
      MAP_FOR_ALL_ELEMS(pinMap, Pin*, Pin*, clusterPinPtr, pinPtr) {
	Cell &thisCell = (*pinPtr).PinGetParentCell();
	(*clusterPinPtr).PinSetXOffset(thisCell.CellGetXpos() + (*pinPtr).PinGetXOffset());
	(*clusterPinPtr).PinSetYOffset(thisCell.CellGetYpos() + (*pinPtr).PinGetYOffset());
	/* Make pin connection to the net */
	Net &connectedNet = (*pinPtr).PinGetNet();
	(*clusterPinPtr).Connect(connectedNet);
	connectedNet.NetAddPin(*clusterPinPtr);
	(*pinPtr).PinSetIsHidden(true);
      } END_FOR;
    }
    clusterPlacementTime += getCPUTime() - stepTime;
  }
  cout << "Created cluster with " << numHeightVariations 
       << " shapes for: " << clusterCellName << endl;
  stepTime = getCPUTime();
  /* STEP : HIDE INTERNAL NETS, CELLS OF THE CLUSTER */
  MAP_FOR_ALL_KEYS(internalNets, Net*, bool, netPtr) {
    DesignHideNet(netPtr);
    internalNetVec.push_back(netPtr);
  } END_FOR;

  /* STEP : CREATE THE CLUSTER OBJECT */
  uint count = 0;
  for (idx = 0; idx < numCells; idx++) {
    cellPtr = (Cell *)clusterCells[idx];
    //    cout << "Hiding cell: " << (*cellPtr).CellGetName() << endl;
    DesignHideCell(cellPtr);
    //    count++;
  }
  clusterPlacementTime += getCPUTime() - stepTime;

  //  cout << "Hiding " << count << " cells" << endl;
  /* Create the cluster object for the list of cells */
  vector<uint> bCellIndices, rowNum, xPosInRow;
  Cluster *clusterOfCell = new Cluster(clusterCells, bCellIndices, rowNum,
				       xPosInRow, pinMap);
  (*clusterOfCell).ClusterSetNumCells(numCells);
  (*clusterOfCell).ClusterSetBCellsPlaced(true);
  (*clusterOfCell).ClusterSetInternalNets(internalNetVec);
  (*clusterOfCell).ClusterSetCellArea(cellArea);
  (*clusterOfCell).ClusterSetCellPositions(cellPositions);
  (*clusterOfCell).ClusterSetDimensions(dimensions);
  (*clusterOfCell).ClusterSetHeightVariations(heightVariations);
  (*clusterOfCell).ClusterSetShapeHPWL(shapeHPWLVec);
  (*clusterOfCell).ClusterSetNumBoundaryCells(boundaryCells.size());
  (*clusterOfCell).ClusterSetPlacementTime(clusterPlacementTime);
  CellSetCluster(clusterCell, clusterOfCell);

  /* STEP : ADD THE CLUSTER OBJECT TO THE TOP LEVEL */
  DesignAddOneClusterToDesignDB(clusterCell);
}

/* Top level function to accept a vector of cells and form shapes */
void
Design::DesignClusterCellsFormShapes(vector<Cell *> &clusterCells) 
{
  /* STEPS TO BE FOLLOWED */
  /* Identify the size of the cluster for an aspect ratio close to 1.0 */
  /* Identify two other aspect ratios which are +0.30% of the number of rows
     and -0.30% the number of rows. */
  /* Write out new designs for the aspect ratios 1, +30% height, -30% height
     and try placing it using NTUplace */
}


inline void
getNeighborClusterSize(Cell *clusterCell, HyperGraph &myGraph, double &minArea,
		       double &maxArea, uint &numConnectedClusters)
{
  Cell *connectedCluster;
  void *connectedNode;
  vector<void *> connectedNodes;
  double clusterHeight, clusterWidth, clusterArea;
  uint clusterNodeIdx;
  
  connectedNodes = myGraph.GetConnectedNodes((void*)clusterCell);
  minArea = DBL_MAX;
  maxArea = 0;
  numConnectedClusters = connectedNodes.size();
  VECTOR_FOR_ALL_ELEMS(connectedNodes, void *, connectedNode) {
    connectedCluster = (Cell *)connectedNode;
    clusterHeight = (*connectedCluster).CellGetHeight();
    clusterWidth = (*connectedCluster).CellGetWidth();
    clusterArea = clusterHeight * clusterWidth;
    if (clusterArea > maxArea) maxArea = clusterArea;
    else if (clusterArea < minArea) minArea = clusterArea;
  } END_FOR;
}

inline uint
getNumSequentialCells(vector<Cell *> &cellsOfCluster)
{
  Cell *thisCell;
  uint seqCellCount;

  seqCellCount = 0;
  VECTOR_FOR_ALL_ELEMS(cellsOfCluster, Cell *, thisCell) {
    if ((*thisCell).CellIsSequential()) {
      seqCellCount++;
    }
  } END_FOR;
  
  return (seqCellCount);
}

inline
double getOverLap(double L1, double R1, double L2, double R2)
{
  double left, right;
  double overlap;

  right = R2;
  if (R1 < R2) right = R1;
  left = L2;
  if (L1 > L2) left = L1;

  overlap = (int)(right - left);
  if (overlap < 0) overlap = 0.0;

  return (overlap);
}

inline void
getTotalClusterOverLap(Design &myDesign, double &totalOverlap, double &peakOverlap,
		       double &percentageOverlap)
{
  Cell *clust1, *clust2;
  map<string, Cell *> DesignClustersCopy = myDesign.DesignGetClusters();
  map<string, Cell *> &DesignClusters = myDesign.DesignGetClusters();
  map<Cell *, bool> visitedNodes;
  string cellName1, cellName2;
  double cellLeft1, cellRight1, cellTop1, cellBot1;
  double cellLeft2, cellRight2, cellTop2, cellBot2;
  double overlapx, overlapy;
  double overlapArea, chipArea;
  uint maxx, maxy;
  
  totalOverlap = 0;
  peakOverlap = 0;
  percentageOverlap = 0;
  MAP_FOR_ALL_ELEMS(DesignClustersCopy, string, Cell *, cellName1, clust1) {
    overlapArea = 0;
    cellLeft1 = (*clust1).CellGetXpos();
    cellRight1 = cellLeft1 + (*clust1).CellGetWidth();
    cellBot1 = (*clust1).CellGetYpos();
    cellTop1 = cellBot1 + (*clust1).CellGetHeight();
    _KEY_DOES_NOT_EXIST(visitedNodes, clust1) {
      visitedNodes[clust1] = 0;
    }
    MAP_FOR_ALL_ELEMS(DesignClusters, string, Cell *, cellName2, clust2) {
      _KEY_EXISTS(visitedNodes, clust2) {
	continue;
      }
      cellLeft2 = (*clust2).CellGetXpos();
      cellRight2 = cellLeft2 + (*clust2).CellGetWidth();
      cellBot2 = (*clust2).CellGetYpos();
      cellTop2 = cellBot2 + (*clust2).CellGetHeight();
      overlapx = getOverLap(cellLeft1, cellRight1, cellLeft2, cellRight2);
      overlapy = getOverLap(cellBot1, cellTop1, cellBot2, cellTop2);
      overlapArea += overlapx * overlapy;
    } END_FOR;
    totalOverlap += overlapArea;
    if (overlapArea > peakOverlap) {
      peakOverlap = overlapArea;
    }
  } END_FOR;
  
  myDesign.DesignGetBoundingBox(maxx, maxy);
  chipArea = ((double)maxx) * maxy;
  percentageOverlap = (totalOverlap / chipArea) * 100;
}

/* FUNCTION TO DUMP THE FOLLOWING INFORMATION FOR EACH CLUSTER:
   - TOTAL AREA 
   - TOTAL ROWS
   - TOTAL SITES
   - TOTAL NUMBER OF INTERNAL NETS
   - TOTAL NUMBER OF BOUNDARY CELLS
   - TOTAL NUMBER OF CLUSTERS THIS CLUSTER CONNECTS TO
*/
void
Design::DesignDumpClusterInfo(string fileName)
{
  Cluster *clusterOfCell;
  Cell *clusterCellPtr;
  string cellName;
  uint numBoundaryCells, numClusters, numCells;
  uint numSeqCells, numConnectedClusters;
  uint maxx, maxy;
  double chipArea;
  double clusterArea, cellArea, totalClusterArea;
  double minNeighborArea, maxNeighborArea;
  double clusterHeight, clusterWidth;
  double clusterPlacementTime, totalClusterPlacementTime;
  double totalOverlap, peakOverlap, percentOverlap;
  ulong totalClusterHeight, totalClusterWidth;
  double averageClusterWidth, averageClusterHeight;
  ofstream opFile;
  HyperGraph &myGraph = DesignGetGraph();
  
  opFile.open(fileName.data());
  //  opFile << "# ClusterName\tcellArea\tclusterArea\tNumInternalNets\tNumBoundaryCells\tNumOtherClusterConnections" << endl; 
  opFile << "#ClusterName\tNumCells\tNumSeqCells\tCellArea\tClusterArea\tNumInternalNets\tNumBoundCells\t";
  opFile << "NumConnClusters\tLargestConnClustArea\tSmallestConnClustArea\tWirelength\tPlacementTime\t";
  totalClusterHeight = 0;
  totalClusterWidth = 0;
  numClusters = 0;
  clusterPlacementTime = 0;
  totalClusterPlacementTime = 0;
  totalClusterArea = 0;
  DESIGN_FOR_ALL_CLUSTERS((*this), cellName, clusterCellPtr) {
    clusterOfCell = (Cluster *)CellGetCluster(clusterCellPtr);
    getNeighborClusterSize(clusterCellPtr, myGraph, minNeighborArea, maxNeighborArea, 
			   numConnectedClusters);
    vector<Net *> &internalNets = (*clusterOfCell).ClusterGetInternalNets();
    vector<Cell *> &cellsOfCluster = (*clusterOfCell).ClusterGetCellsOfCluster();
    vector<double> &shapeHPWL = (*clusterOfCell).ClusterGetShapeHPWL();
    numCells = cellsOfCluster.size();
    numSeqCells = getNumSequentialCells(cellsOfCluster);
    numBoundaryCells = (*clusterOfCell).ClusterGetNumBoundaryCells();
    cellArea = (*clusterOfCell).ClusterGetCellArea();
    clusterHeight = (*clusterCellPtr).CellGetHeight();
    clusterWidth = (*clusterCellPtr).CellGetWidth();
    clusterArea = clusterHeight * clusterWidth;
    totalClusterArea += clusterArea;
    totalClusterHeight += clusterHeight;
    totalClusterWidth += clusterWidth;
    clusterPlacementTime = (*clusterOfCell).ClusterGetPlacementTime();
    totalClusterPlacementTime += clusterPlacementTime;
    opFile << cellName << "\t"
	   << numCells << "\t"
	   << numSeqCells << "\t"
	   << cellArea << "\t"
	   << clusterArea << "\t"
	   << internalNets.size() << "\t"
	   << numBoundaryCells << "\t"
	   << numConnectedClusters << "\t"
	   << maxNeighborArea << "\t"
	   << minNeighborArea << "\t"
	   << shapeHPWL[shapeHPWL.size() - 1] << "\t"
	   << clusterPlacementTime << "\t"
	   << endl;
    numClusters++;
  } DESIGN_END_FOR;
  DesignGetBoundingBox(maxx, maxy);
  chipArea = ((double)maxx) * maxy;
  averageClusterWidth = ((double)totalClusterWidth) / numClusters;
  averageClusterHeight = ((double)totalClusterHeight) / numClusters;
  DesignSetAverageClusterCellWidth(averageClusterWidth);
  DesignSetAverageClusterCellHeight(averageClusterHeight);  
  getTotalClusterOverLap((*this), totalOverlap, peakOverlap, percentOverlap);
  cout << "Total cluster placement time : " << totalClusterPlacementTime << endl;
  cout << "       Average cluster height: " << averageClusterHeight << endl;
  cout << "       Average cluster width : " << averageClusterWidth << endl;
  cout << "                Cluster Area : " << totalClusterArea << endl;
  cout << "                   Chip Area : " << chipArea << endl;
  cout << "      Total overlapping area : " << totalOverlap << endl;
  cout << " Percentage overlapping area : " << percentOverlap << endl;
  cout << "       Peak overlapping area : " << peakOverlap << endl;
  cout << "           Final Utilization : " << ((double)totalClusterArea)/chipArea << endl;
  opFile.close();
}

void
Design::DesignUnclusterLargeCluster(Cell *clusterCell, bool noDissolve)
{
  Cell *thisCell;
  Pin *pinPtr, *connPinPtr;
  Net *netPtr;
  map<Cell *, uint> cellLookupMap;
  uint clusterHeight, clusterWidth;
  uint lBoundX, uBoundX, lBoundY, uBoundY;
  uint clusterXpos, clusterYpos, cellXpos, cellYpos;
  uint bCellIndex, numRows, numBoundaryCells;
  uint bCellWidth;
  uint rowNum, xPosInRow;
  bool left, right, top, bot;
  bool bCellsPlaced, rowBased;
  uint idx, bCellIdx;
  Env &DesignEnv = DesignGetEnv();

  debug = false;
  /* Get the cluster structure corresponding to the current 
     cluster cell */
  Cluster *clusterCellPtr = (Cluster *)CellGetCluster(clusterCell);
  Cluster &thisCluster = *clusterCellPtr;
  rowBased = thisCluster.ClusterGetRowBased();

  /* Get all the boundary cell and other cell related info */
  clusterXpos = (*clusterCell).CellGetXpos();
  clusterYpos = (*clusterCell).CellGetYpos();

  vector<Cell *> &cellsOfCluster = thisCluster.ClusterGetCellsOfCluster();
  vector<Net *> &netsOfCluster = thisCluster.ClusterGetInternalNets();

  /* Get the pin map from the cluster */
  map<Pin*, Pin*> &pinMap = thisCluster.ClusterGetPinMap();

  /* Add all cells to a lookup map */
  VECTOR_FOR_ALL_ELEMS(cellsOfCluster, Cell*, thisCell) {
    cellXpos = (*thisCell).CellGetXpos();
    cellYpos = (*thisCell).CellGetYpos();
    (*thisCell).CellSetXpos(clusterXpos + cellXpos);
    (*thisCell).CellSetYpos(clusterYpos + cellYpos);
    DesignUnhideCell(thisCell);
    CELL_FOR_ALL_PINS((*thisCell), PIN_DIR_ALL, pinPtr) {
      (*pinPtr).PinSetIsHidden(false);
    } CELL_END_FOR;
  } END_FOR;

  /* Un-hide all the internal nets */
  VECTOR_FOR_ALL_ELEMS(netsOfCluster, Net *, netPtr) {
    DesignUnhideNet(netPtr);
  } END_FOR;

  /* Remove pins of the cluster from the external nets */
  MAP_FOR_ALL_KEYS(pinMap, Pin *, Pin *, pinPtr) {
    Net &connectedNet = (*pinPtr).PinGetNet();
    connectedNet.NetRemovePin(*pinPtr);
    /* Destroy the pin that has been created on the cluster */
    DesignDeletePin(pinPtr);
  } END_FOR;


  /* Clear flag on cluster cell */
  CellClearCluster(clusterCell);
  
  /* Remove the cluster cell from the design database */
  DesignRemoveOneClusterFromDesignDB(clusterCell);
  
  /* Remove the data "Cluster" stored on the cell */
  delete (&thisCluster);

  /* Delete the cluster cell */
  DesignDeleteCell(clusterCell);
}

void
Design::DesignFormClusters(vector<vector<Cell *> > &clusters, double &totalTime)
{
  double actualTime, clusterPlacementTime;
  uint idx, numClusters;
  int status;
  Env &DesignEnv = DesignGetEnv();
  double hVariationPercent = 
    DesignEnv.EnvGetClusterHVariationPercentage();
  double xPercent = 
    DesignEnv.EnvGetClusterAddAreaPercentage();
  uint numSteps = 
    DesignEnv.EnvGetNumHVariationSteps();
  numClusters = clusters.size();
  totalTime = 0;
  actualTime = getCPUTime();
  for (idx = 0; idx < numClusters; idx++) {
    vector<Cell *> &clusterCells = clusters[idx];
    DesignCreateClusterObject(clusterCells, xPercent, hVariationPercent,
			      numSteps, clusterPlacementTime);
    totalTime += clusterPlacementTime;
    //    if (idx == 10) break;
    //  break;
  }
  actualTime = getCPUTime() - actualTime;
}
