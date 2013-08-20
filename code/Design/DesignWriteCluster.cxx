# include <Design.h>

void
DesignWriteClusterHeaderFile(ofstream &opFile)
{
  opFile << "# Created  :  " << getCurrentTime() << endl;		
  opFile << "# User     :  " << getUserName() << "@" << getHostName() << endl;
  opFile << "# Platform :  " << getPlatformString() << endl;
}

void 
DesignWriteClusterNets(vector<Net *> &affectedNets, vector<Net *> &externalNets,
		       map<Cell *, bool> &mapOfCells, string fname, 
		       bool writeFixedPorts)
{
  Cell *cellPtr;
  Pin *pinPtr;
  Net *netPtr;
  map<Net *, pair<double, double> > cellPosMap;
  map<Net *, string> cellNameMap;
  map<Net *, string>::iterator iter;
  string netName, fileName, fixedCellName;
  string pinDir, cellName, netDegree;
  ofstream opFile;
  uint numPins, numNets, numNetPins;
  uint numCellsIterated, idx;
  uint pinXOffset, pinYOffset;

  /* Create a map of external nets->cell positions  
     and external nets->net weights */
  if (writeFixedPorts) {
    numNets = externalNets.size();
    for (idx = 0; idx < numNets; idx++) {
      netPtr = externalNets[idx];
      fixedCellName = "p" + getStrFromInt(idx);
      _KEY_DOES_NOT_EXIST(cellNameMap, netPtr) {
	cellNameMap[netPtr] = fixedCellName;
      }
    }
  }

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

    if (writeFixedPorts) {
      /* Add one for the fixed port added for each external net */
      _KEY_EXISTS(cellNameMap, netPtr) {
	numPins++;
      }
    }
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
      //      cout << "Cell of net is: " << (*cellPtr).CellGetName();
      _KEY_DOES_NOT_EXIST(mapOfCells, cellPtr) {
	//	cout << " Not a cell of the cluster " << endl;
	continue;
      } 
      //      cout << " Is a cell of the cluster " << endl;
      if ((*pinPtr).PinGetDirection() == PIN_DIR_INPUT) {
	pinDir = "I";
      } else {
	pinDir = "O";
      }
      cellName = cellOfPin.CellGetName();
      pinXOffset = (*pinPtr).PinGetXOffset();
      pinYOffset = (*pinPtr).PinGetYOffset();
      pinXOffset -= (cellOfPin.CellGetWidth() / 2);
      pinYOffset -= (cellOfPin.CellGetHeight() / 2);
      netString += "\t" + cellName + "  " + pinDir + "  : " +
	getStrFromInt(pinXOffset) + "  " + getStrFromInt(pinYOffset) + " \n";
      numNetPins++;
    } NET_END_FOR;
    if (writeFixedPorts) {
      _KEY_EXISTS_WITH_VAL(cellNameMap, netPtr, iter) {
	//      cout << "Net: " << (*netPtr).NetGetName() << " is an external net" << endl;
	fixedCellName = iter->second;
	netString += "\t" + fixedCellName + "  " + "O" + "  :  0  0 \n";
	numNetPins++;
      }
    }
    opFile << "NetDegree : " << numNetPins << "  " << netName << endl;
    opFile << netString;
  } END_FOR;

  opFile.close();
}

void 
DesignWriteClusterNodes(vector<Cell *> &clusterCells, vector<Net *> &externalNets,
			uint terminalCount, string fname, bool noTerminals, 
			bool writeFixedPorts)
{
  Cell *cellPtr;
  Net *netPtr;
  string cellName, fileName;
  uint cellCount, idx, numNets;
  ofstream opFile;

  //  cout << "Writing nodes file.." << endl;
  fileName = fname + ".nodes";
  cellCount = clusterCells.size();
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nodes 1.0" << endl;
  DesignWriteClusterHeaderFile(opFile);
  
  numNets = 0;
  if (writeFixedPorts) {
    numNets = externalNets.size();
  }
  opFile << endl << "NumNodes : " << (cellCount + numNets) << endl;
  if (noTerminals) {
    opFile << "NumTerminals : 0 " << endl;
  } else if (numNets > 0) {
    opFile << "NumTerminals : " << numNets << endl;
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

  if (writeFixedPorts) {
    for (idx = 0; idx < numNets; idx++) {
      opFile << "   " << "p" + getStrFromInt(idx) << " 56 " 
	     << " 56 " << "   terminal" << endl;
    } 
  }
  opFile.close();
}

void
DesignWriteClusterPlacement(vector<Cell *> &clusterCells,  
			    vector<Net *> &externalNets,
			    vector<pair<double, double> > &cellPositions,
			    string fname, bool noTerminals, 
			    bool writeFixedPorts)
{
  Cell *cellPtr;
  string cellName, fileName;
  ofstream opFile;
  uint idx, numExtNets;

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

  if (writeFixedPorts) {
    numExtNets = externalNets.size();
    for (idx = 0; idx < numExtNets; idx++) {
      pair<double, double> &position = cellPositions[idx];
      opFile << "   " << "p" + getStrFromInt(idx) << "\t" 
	     << (dtoi(position.first) + 28)  << "\t" << (dtoi(position.second) + 28) << endl;
    } 
  }

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
		       vector<Net *> &externalNets, vector<pair<double, double> > &cellPositions,
		       vector<double> &netWeights, map<Cell *, bool> &mapOfCells,
		       string benchName, uint clusterWidth, uint clusterHeight,
		       uint singleRowHeight, uint singleSiteWidth, bool noTerminals,
		       bool writeFixedPorts)
{
  Cell *cellPtr;
  uint terminalCount;
  
  terminalCount = 0;
  VECTOR_FOR_ALL_ELEMS(clusterCells, Cell*, cellPtr) {
    //    cout << "WRITING CELL: " << (*cellPtr).CellGetName();
    if ((*cellPtr).CellIsClusterFixed()) {
      terminalCount++;
      //      cout << " TERMINAL" << endl;
      //    } else {
      //      cout << " NOT A TERMINAL" << endl;
    }
  } END_FOR;
  //  cout << "TERMINAL COUNT IS: " << terminalCount << endl;
  /* Write cluster nodes */
  DesignWriteClusterNodes(clusterCells, externalNets, terminalCount, 
			  benchName, noTerminals, writeFixedPorts);
  /* Write the placement file */
  DesignWriteClusterPlacement(clusterCells, externalNets, cellPositions,
			      benchName, noTerminals, writeFixedPorts);
  /* Write the nets file */
  DesignWriteClusterNets(affectedNets, externalNets, mapOfCells, benchName,
			 writeFixedPorts);

  /* Write the scl file */
  DesignWriteClusterScl(clusterWidth, clusterHeight, singleRowHeight,
			singleSiteWidth, benchName);
  /* Write the wts file */
  DesignWriteClusterWtsFile(benchName);
  /* Write the aux file */
  DesignWriteClusterAuxFile(benchName);
}

