# include <Design.h>

void
DesignWriteClusterHeaderFile(ofstream &opFile)
{
  opFile << "# Created  :  " << getCurrentTime() << endl;		
  opFile << "# User     :  " << getUserName() << "@" << getHostName() << endl;
  opFile << "# Platform :  " << getPlatformString() << endl;
}

void 
DesignWriteClusterNets(vector<Net *> &affectedNets, 
		       map<Cell *, bool> &mapOfCells, string fname) 
{
  Cell *cellPtr;
  Pin *pinPtr;
  Net *netPtr;
  string netName, fileName;
  string pinDir, cellName, netDegree;
  ofstream opFile;
  uint numPins, numNets, numNetPins;
  uint numCellsIterated;
  uint pinXOffset, pinYOffset;

  /* Compute the number of nets and pins */  
  numNets = affectedNets.size();
  numPins = 0;
  VECTOR_FOR_ALL_ELEMS(affectedNets, Net*, netPtr) {
    NET_FOR_ALL_PINS((*netPtr), pinPtr) {
      Cell &thisCell = (*pinPtr).PinGetParentCell();
      cellPtr = &thisCell;
      _KEY_DOES_NOT_EXIST(mapOfCells, cellPtr) {
	continue;
      }
      numPins++;
    } NET_END_FOR;
  } END_FOR;
  //  cout << "Writing nets file.." << endl;

  fileName = fname + ".nets";
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nets 1.0" << endl; 
  DesignWriteClusterHeaderFile(opFile);
  opFile << endl << "NumNets : " << numNets << endl;
  opFile << "NumPins : " << numPins << endl;
  opFile << endl;

  VECTOR_FOR_ALL_ELEMS(affectedNets, Net*, netPtr) {
    netName = (*netPtr).NetGetName();
    numNetPins = 0;
    string netString;
    NET_FOR_ALL_PINS((*netPtr), pinPtr) {
      Cell &cellOfPin = (*pinPtr).PinGetParentCell();
      cellPtr = &cellOfPin;
      _KEY_DOES_NOT_EXIST(mapOfCells, cellPtr) {
	continue;
      }
      if ((*pinPtr).PinGetDirection() == PIN_DIR_INPUT) {
	pinDir = "I";
      } else {
	pinDir = "O";
      }
      cellName = cellOfPin.CellGetName();
      pinXOffset = (*pinPtr).PinGetXOffset();
      pinYOffset = (*pinPtr).PinGetYOffset();
      pinXOffset -= cellOfPin.CellGetWidth();
      pinYOffset -= cellOfPin.CellGetHeight();
      netString += "\t" + cellName + "  " + pinDir + "  : " +
	getStrFromInt(pinXOffset) + "  " + getStrFromInt(pinYOffset) + " \n";
      numNetPins++;
    } NET_END_FOR;
    opFile << "NetDegree : " << numNetPins << "  " << netName << endl;
    opFile << netString;
  } END_FOR;

  opFile.close();
}

void 
DesignWriteClusterNodes(vector<Cell *> &clusterCells, uint terminalCount,
			string fname, bool noTerminals) 
{
  Cell *cellPtr;
  string cellName, fileName;
  uint cellCount, idx;
  ofstream opFile;

  //  cout << "Writing nodes file.." << endl;
  fileName = fname + ".nodes";
  cellCount = clusterCells.size();
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nodes 1.0" << endl;
  DesignWriteClusterHeaderFile(opFile);
  opFile << endl << "NumNodes : " << cellCount << endl;
  if (noTerminals) {
    opFile << "NumTerminals : 0 " << endl;
  } else {
    opFile << "NumTerminals : " << terminalCount << endl;
  }
  VECTOR_FOR_ALL_ELEMS(clusterCells, Cell *, cellPtr) {
    cellName = (*cellPtr).CellGetName();
    opFile << "   " << cellName << " " << (*cellPtr).CellGetWidth();
    opFile << "   " << (*cellPtr).CellGetHeight(); 
    if (!noTerminals) {
      if ((*cellPtr).CellIsClusterFixed()) {
	opFile << "   " << "terminal";
      }
    }
    opFile << endl;
  } END_FOR;

  opFile.close();
}

void
DesignWriteClusterPlacement(vector<Cell *> &clusterCells, string fname,
			    bool noTerminals) 
{
  Cell *cellPtr;
  string cellName, fileName;
  ofstream opFile;


  fileName = fname + ".pl";
  _STEP_BEGIN("Writing placement for current design");
  opFile.open(fileName.data(), ifstream::out);

  opFile << "UCLA pl 1.0" << endl;
  DesignWriteClusterHeaderFile(opFile);
  opFile << endl;

  VECTOR_FOR_ALL_ELEMS(clusterCells, Cell*, cellPtr) {
    cellName = (*cellPtr).CellGetName();
    opFile << cellName << "\t" << (*cellPtr).CellGetXpos() << "\t" 
	   << (*cellPtr).CellGetYpos() << "\t:\t" 
	   << getStrForOrientation((*cellPtr).CellGetOrientation());
    if (!noTerminals) {
      if ((*cellPtr).CellIsClusterFixed()) {
	//	if ((*cellPtr).CellGetName() == "o1419") {
	  //	  cout << "break here" << endl;
	//	}
	opFile << "\t" << "/FIXED";
      }
    }
    opFile << endl;
  } END_FOR;

  _STEP_END("Writing placement for current design");
  
  opFile.close();
}

void
DesignWriteClusterScl(uint clusterWidth, uint clusterHeight, 
		      uint singleRowHeight, uint singleSiteWidth, 
		      string fname)
{
  string designName, fileName;
  string cellName;
  ofstream opFile;
  uint rowCoordinate;
  uint subRowOrigin, numSites;
  uint rowIdx, numRows;

  fileName = fname + ".scl";

  _STEP_BEGIN("Writing rows for current design");
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA scl 1.0" << endl;
  DesignWriteClusterHeaderFile(opFile);
  opFile << endl;

  numRows = clusterHeight / singleRowHeight;
  numSites = clusterWidth / singleSiteWidth;
  opFile << "NumRows : " << numRows << endl;
  opFile << endl;
  for (rowIdx = 0; rowIdx < numRows; rowIdx++) {
    rowCoordinate = rowIdx * singleRowHeight;
    opFile << "CoreRow " << "Horizontal" << endl;
    opFile << "  Coordinate    :   " << rowCoordinate << endl;
    opFile << "  Height        :   " << singleRowHeight << endl;
    opFile << "  Sitewidth     :   " << singleSiteWidth << endl;
    opFile << "  Sitespacing   :   " << singleSiteWidth << endl;
    opFile << "  Siteorient    :   " << "N" << endl;
    opFile << "  Sitesymmetry  :   " << "Y" << endl;
    opFile << "  SubrowOrigin  :   " << "0" 
	   << "  NumSites  :  " << numSites << endl;
    opFile << "End" << endl;
  } 

  _STEP_END("Writing rows for current design");
  opFile.close();
}

void
DesignWriteClusterWtsFile(string fname) 
{
  string fileName;
  ofstream opFile;

  _STEP_BEGIN("Writing weights file");

  fileName = fname + ".wts";
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA wts 1.0" << endl;
  DesignWriteClusterHeaderFile(opFile);

  _STEP_END("Writing weights file");

  opFile.close();
}

void
DesignWriteClusterAuxFile(string fname)
{
  string fileName;
  ofstream opFile;

  _STEP_BEGIN("Writing aux file");

  fileName = fname + ".aux";
  opFile.open(fileName.data(), ifstream::out);
  opFile << "RowBasedPlacement : " 
	 << (fname + ".nodes ") 
	 << (fname + ".nets ")
	 << (fname + ".wts ")
	 << (fname + ".pl ")
	 << (fname + ".scl ")
	 << endl;
  _STEP_END("Writing aux file");

  opFile.close();
}

void 
DesignWriteClusterData(vector<Cell *> &clusterCells, vector<Net *> &affectedNets,
		       map<Cell *, bool> &mapOfCells,
		       string benchName, uint clusterWidth, uint clusterHeight,
		       uint singleRowHeight, uint singleSiteWidth, bool noTerminals)
{
  Cell *cellPtr;
  uint terminalCount;
  
  terminalCount = 0;
  VECTOR_FOR_ALL_ELEMS(clusterCells, Cell*, cellPtr) {
    if ((*cellPtr).CellIsClusterFixed()) {
      terminalCount++;
    }
  } END_FOR;

  /* Write cluster nodes */
  DesignWriteClusterNodes(clusterCells, terminalCount, benchName, noTerminals);
  /* Write the placement file */
  DesignWriteClusterPlacement(clusterCells, benchName, noTerminals);
  /* Write the nets file */
  DesignWriteClusterNets(affectedNets, mapOfCells, benchName);
  /* Write the scl file */
  DesignWriteClusterScl(clusterWidth, clusterHeight, singleRowHeight,
			singleSiteWidth, benchName);
  /* Write the wts file */
  DesignWriteClusterWtsFile(benchName);
  /* Write the aux file */
  DesignWriteClusterAuxFile(benchName);
}

