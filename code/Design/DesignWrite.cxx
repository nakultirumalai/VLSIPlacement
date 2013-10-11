# include <Design.h>

void
DesignWriteHeaderFile(Design &myDesign, ofstream &opFile)
{
  Env &DesignEnv = myDesign.DesignGetEnv();
  opFile << "# Created  :  " << getCurrentTime() << endl;		
  opFile << "# User     :  " << getUserName() << "@" << getHostName() << endl;
  opFile << "# Platform :  " << getPlatformString() << endl;
}

void DesignWriteNets(Design &myDesign, string fname) 
{
  string netName, designName, fileName;
  string pinDir;
  string cellName, netDegree;
  Cell *cellPtr;
  Pin *pinPtr;
  Net *netPtr;
  ofstream opFile;
  uint numPins, numNetPins;
  uint numCellsIterated;
  uint pinXOffset, pinYOffset;

  /* Compute the number of nets and pins */  
  numPins = 0;
  numCellsIterated = 0;
  DESIGN_FOR_ALL_CELLS(myDesign, cellName, cellPtr) {
    numPins += (*cellPtr).CellGetNumPins();
    numCellsIterated++;
  } DESIGN_END_FOR;

  cout << "Read " << numPins << " pins from " 
       << numCellsIterated << " cells" << endl;
  cout << "Writing nets file.." << endl;

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".nets";
  } else {
    fileName = fname + ".nets";
  }

  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nets 1.0" << endl; 
  DesignWriteHeaderFile(myDesign, opFile);
  opFile << endl << "NumNets : " << myDesign.DesignGetNumNets() << endl;
  opFile << "NumPins : " << numPins << endl;
  
  opFile << endl;

  DESIGN_FOR_ALL_NETS(myDesign, netName, netPtr) {
    numNetPins = 0;
    string netString;
    NET_FOR_ALL_PINS((*netPtr), pinPtr) {
      if ((*pinPtr).PinGetDirection() == PIN_DIR_INPUT) {
	pinDir = "I";
      } else {
	pinDir = "O";
      }
      Cell &cellOfPin = (*pinPtr).PinGetParentCell();
      cellName = cellOfPin.CellGetName();
      pinXOffset = (*pinPtr).PinGetXOffset();
      pinYOffset = (*pinPtr).PinGetYOffset();
      pinXOffset -= (cellOfPin.CellGetWidth() / 2);
      pinYOffset -= (cellOfPin.CellGetHeight() / 2);
      netString += "\t" + cellName + "  " + pinDir + "  : " +
	getStrFromInt(pinXOffset) + "  " + getStrFromInt(pinYOffset) + " \n";
      numNetPins++;
    } NET_END_FOR;
    opFile << "NetDegree : " << numNetPins << "  " << netName << endl;
    opFile << netString;
  } DESIGN_END_FOR;

  opFile.close();
}

void DesignWriteNodes(Design &myDesign, string fname) 
{
  string cellName, designName, fileName;
  Cell *cellPtr;
  uint cellCount;
  ofstream opFile;

  cout << "Writing nodes file.." << endl;
  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".nodes";
  } else {
    fileName = fname + ".nodes";
  }
  cellCount = 0;
  DESIGN_FOR_ALL_CELLS(myDesign, cellName, cellPtr) {
    cellCount++;
  } DESIGN_END_FOR;
  
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nodes 1.0" << endl;
  DesignWriteHeaderFile(myDesign, opFile);
  opFile << endl << "NumNodes :              " << cellCount << endl;
  opFile << "NumTerminals :           " << myDesign.DesignGetNumTerminalCells() << endl;

  DESIGN_FOR_ALL_CELLS(myDesign, cellName, cellPtr) {
    opFile << "        " << cellName << " " << (*cellPtr).CellGetWidth();
    opFile << "      " << (*cellPtr).CellGetHeight(); 
    if ((*cellPtr).CellIsTerminal()) {
      opFile << "     " << "terminal";
    }
    opFile << endl;
  } DESIGN_END_FOR;

  opFile.close();
}

void
DesignWritePlacement(Design &myDesign, string fname) 
{
  string designName, fileName;
  string cellName;
  Cell *cellPtr;
  ofstream opFile;

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".pl";
  } else {
    fileName = fname + ".pl";
  }

  _STEP_BEGIN("Writing placement for current design");
  opFile.open(fileName.data(), ifstream::out);

  opFile << "UCLA pl 1.0" << endl;
  DesignWriteHeaderFile(myDesign, opFile);
  opFile << endl;

  DESIGN_FOR_ALL_CELLS(myDesign, cellName, cellPtr) {
    uint cellXpos;
    uint cellYpos;
    cellXpos = (*cellPtr).CellGetXpos();
    cellYpos = (*cellPtr).CellGetYpos();
    opFile << cellName << "\t" << cellXpos << "\t" << cellYpos 
	   << "\t:\t" << getStrForOrientation((*cellPtr).CellGetOrientation());
    if ((*cellPtr).CellIsTerminal() && !(*cellPtr).CellIsPort()) {
      opFile << "\t" << "/FIXED";
    }
    opFile << endl;
  } DESIGN_END_FOR;

  _STEP_END("Writing placement for current design");
  
  opFile.close();
}

void
DesignWritePlacementFP(Design &myDesign, string fname) 
{
  string designName, fileName;
  string cellName;
  Cell *cellPtr;
  ofstream opFile;

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".pl";
  } else {
    fileName = fname + ".pl";
  }

  _STEP_BEGIN("Writing placement for current design");
  opFile.open(fileName.data(), ifstream::out);

  opFile << "UCLA pl 1.0" << endl;
  DesignWriteHeaderFile(myDesign, opFile);
  opFile << endl;

  DESIGN_FOR_ALL_CELLS(myDesign, cellName, cellPtr) {
    uint cellXpos;
    uint cellYpos;
    cellXpos = (*cellPtr).CellGetXpos();
    cellYpos = (*cellPtr).CellGetYpos();
    opFile << cellName << "\t" << cellXpos << "\t" << cellYpos 
	   << "\t:\t" << getStrForOrientation((*cellPtr).CellGetOrientation());
    if ((*cellPtr).CellIsTerminal()) {
      opFile << "\t" << "/FIXED";
    }
    opFile << endl;
  } DESIGN_END_FOR;

  _STEP_END("Writing placement for current design");
  opFile.close();
}

void
DesignWriteScl(Design &myDesign, string fname)
{
  map<uint, uint> subRows;
  string designName, fileName;
  string cellName;
  PhysRow *rowPtr;
  ofstream opFile;
  uint rowIdx;
  uint subRowOrigin, numSites;

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".scl";
  } else {
    fileName = fname + ".scl";
  }

  _STEP_BEGIN("Writing rows for current design");
  opFile.open(fileName.data(), ifstream::out);

  opFile << "UCLA scl 1.0" << endl;
  DesignWriteHeaderFile(myDesign, opFile);
  opFile << endl;
  opFile << "NumRows : " << myDesign.DesignGetNumPhysRows() << endl;
  opFile << endl;

  DESIGN_FOR_ALL_ROWS(myDesign, rowIdx, rowPtr) {
    opFile << "CoreRow " << (*rowPtr).PhysRowGetTypeStr() << endl;
    opFile << "  Coordinate    :   " << (*rowPtr).PhysRowGetCoordinate() << endl;
    opFile << "  Height        :   " << (*rowPtr).PhysRowGetHeight() << endl;
    opFile << "  Sitewidth     :   " << (*rowPtr).PhysRowGetSiteWidth() << endl;
    opFile << "  Sitespacing   :   " << (*rowPtr).PhysRowGetSiteSpacing() << endl;
    opFile << "  Siteorient    :   " << (*rowPtr).PhysRowGetSiteOrientationStr() << endl;
    opFile << "  Sitesymmetry  :   " << (*rowPtr).PhysRowGetSiteSymmetryStr() << endl;
    subRows = (*rowPtr).PhysRowGetSubRows();
    MAP_FOR_ALL_ELEMS(subRows, uint, uint, subRowOrigin, numSites) {
      opFile << "  SubrowOrigin  :   " << subRowOrigin 
	     << "  NumSites  :  " << numSites << endl;
    } END_FOR;
    opFile << "End" << endl;
  } DESIGN_END_FOR;

  _STEP_END("Writing rows for current design");

  opFile.close();
}

void
DesignWriteWtsFile(Design &myDesign, string fname) 
{
  string designName, fileName;
  ofstream opFile;

  _STEP_BEGIN("Writing weights file");

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".wts";
  } else {
    fileName = fname + ".wts";
  }

  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA wts 1.0" << endl;
  DesignWriteHeaderFile(myDesign, opFile);

  _STEP_END("Writing weights file");

  opFile.close();
}

void
DesignWriteAuxFile(Design &myDesign, string fname)
{
  string designName, fileName;
  ofstream opFile;

  _STEP_BEGIN("Writing aux file");

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".aux";
  } else {
    designName = fname;
    fileName = fname + ".aux";
  }

  opFile.open(fileName.data(), ifstream::out);
  opFile << "RowBasedPlacement : " 
	 << (designName + ".nodes ") 
	 << (designName + ".nets ")
	 << (designName + ".wts ")
	 << (designName + ".pl ")
	 << (designName + ".scl ")
	 << endl;
  _STEP_END("Writing aux file");

  opFile.close();
}

void DesignWriteBookShelfOutput(Design &myDesign, string opBenchName)
{
  /* Write the nets, nodes and pl file */
  DesignWriteNets(myDesign, opBenchName);
  DesignWriteNodes(myDesign, opBenchName);
  DesignWritePlacement(myDesign, opBenchName);
  DesignWriteScl(myDesign, opBenchName);

  /* Create the wts file */
  DesignWriteWtsFile(myDesign, opBenchName);
  /* Create the aux file */
  DesignWriteAuxFile(myDesign, opBenchName);
}

void DesignWriteBookShelfOutput(Design &myDesign, string opBenchName,
				bool forMacroPlacer)
{
  /* Write the nets, nodes and pl file */
  DesignWriteNets(myDesign, opBenchName);
  DesignWriteNodes(myDesign, opBenchName);
  if (forMacroPlacer) {
    DesignWritePlacementFP(myDesign, opBenchName);
  } else {
    DesignWritePlacement(myDesign, opBenchName);
  }
  DesignWriteScl(myDesign, opBenchName);

  /* Create the wts file */
  DesignWriteWtsFile(myDesign, opBenchName);
  /* Create the aux file */
  DesignWriteAuxFile(myDesign, opBenchName);
}

void DesignWriteOutputPlacement(Design &myDesign)
{
  DesignWritePlacement(myDesign, (myDesign.DesignGetName() + ".ourplacer"));
}

void DesignWriteOutputPlacement(Design &myDesign, string outputFileName)
{
  DesignWritePlacement(myDesign, outputFileName);
}
