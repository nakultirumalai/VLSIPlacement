# include <Design.h>
# include <assert.h>

void
Design::DesignReadDesign(string DesignPath, string DesignName)
{
  string fullFileName, fileName;
  string listOfFiles;
  string fullDesignPath;
  bool rowBasedPlacement;
  vector<string> fileList;
  
  fullDesignPath = DesignPath + DIR_SEP;
  fullFileName = fullDesignPath + DesignName + DESIGN_AUX_FILE_EXT;

  /* Open the aux fle and read all the other files that need to be
     read */
  ifstream DesignFile(fullFileName.data(), ios_base::in);
  getline(DesignFile, listOfFiles);
  if (listOfFiles == "") {
    return;
  }

  /* Collect all files to be read from the aux file */
  istringstream stream(listOfFiles, istringstream::in);
  do {
    stream >> fileName;
    if (fileName == ROW_BASED_PLACEMENT) {
      RowBasedPlacement = true;
      continue;
    } else if (fileName.find('.', 0) == string::npos) {
      continue;
    }
    fileList.push_back(fileName);
  } while (stream);
  
  VECTOR_FOR_ALL_ELEMS(fileList, string, fileName) {
    int dotpos = fileName.find('.', 0);
    _ASSERT("Filename of unexpected type", dotpos < 0);

    string ext = fileName.substr(dotpos);
    if (ext == DESIGN_CELL_FILE_EXT) {
      DesignCellFileName = fullDesignPath + fileName;
    } else if (ext == DESIGN_NET_FILE_EXT) {
      DesignNetFileName = fullDesignPath + fileName;
    } else if (ext == DESIGN_NET_WTS_FILE_EXT) {
      DesignNetWtsFileName = fullDesignPath + fileName;
    } else if (ext == DESIGN_SCL_FILE_EXT) {
      DesignSclFileName = fullDesignPath + fileName;
    } else if (ext == DESIGN_PL_FILE_EXT) {
      DesignPlFileName = fullDesignPath + fileName;
    } else {
      _ASSERT("Unknown file type found in aux file", 1);
    }
  } END_FOR;

  DesignMapFileName = fullDesignPath + DesignName + DESIGN_NODES_MAP_FILE_EXT;
  if (!fileExists(DesignMapFileName)) {
    DesignMapFileName = "";
  }
  DesignCmdsFileName = fullDesignPath + DesignName + DESIGN_CMDS_FILE_EXT;
  if (!fileExists(DesignCmdsFileName)) {
    DesignCmdsFileName = "";
  }
  DesignPinsMapFileName = fullDesignPath + DesignName + DESIGN_PINS_MAP_FILE_EXT;
  if (!fileExists(DesignPinsMapFileName)) {
    DesignPinsMapFileName = "";
  }
  DesignCellDelaysFileName = fullDesignPath + DesignName + DESIGN_CELL_DELAYS_FILE_EXT;
  if (!fileExists(DesignCellDelaysFileName)) {
    DesignCellDelaysFileName = "";
  }

  this->Name = DesignName;
  DesignReadRows();
  DesignReadCells();
  DesignReadMapFile();
  DesignReadNets();
  DesignReadCellPlacement();
  DesignReadCmdsFile();
  DesignReadPinsMapFile();
  DesignReadCellDelaysFile();

  DesignFile.close();
}

void
Design::DesignFileReadOneNode(ifstream& file)
{
  Cell *newCell;
  string NodeName, terminalString;
  string line;
  unsigned int height, width;
  bool terminalCell;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }

    istringstream stream(line, istringstream::in);

    stream >> NodeName; stream >> width; stream >> height;
    if (!stream.eof()) stream >> terminalString;

    if (terminalString == NODE_TERMINAL_KEYWORD) {
      terminalCell = true;
    }
    newCell = new Cell(height, width, NodeName, terminalCell);
    DesignAddOneCellToDesignDB(newCell);
    break;
  }
}


void
Design::DesignFileReadNodes(ifstream& file)
{
  string Property, Value;
  unsigned int numNodes, numTerminals;
  int idx, cellCount;

  /* Skip empty lines */
  
  for (idx = 0; idx < NUM_NODE_PROPERTIES; idx++) {
    DesignProcessProperty(file, Property, Value);
    if (Property == NUM_NODES_PROPERTY) {
      numNodes = atoi(Value.data());
    } else if (Property == NUM_TERMINALS_PROPERTY) {
      numTerminals = atoi(Value.data());
    }
  }

  while (!file.eof()) {
    DesignFileReadOneNode(file);
  }
}


void
Design::DesignReadCells()
{
  int NumCells;
  string Msg;

  _STEP_BEGIN("Read cells");
  DesignOpenFile(DesignCellFileName);
  DesignFileReadHeader(DesignFile);
  DesignFileReadNodes(DesignFile);

  Msg += "Loaded " + getStrFromInt(DesignCells.size()) + " cells";
  cout << Msg << endl;
  _STEP_END("Read cells");
}


void 
Design::DesignFileReadPins(ifstream &file, unsigned int netDegree,
			   Net &newNet) 
{
  string nodeName, pinDir, garbage;
  string PinName, Msg, line;
  string NetName;
  unsigned int pinNum, cellPinNum;
  float xoffset, yoffset;
  char dir;

  NetName = newNet.NetGetName();
  pinNum = 1;
  for (int i=0; i < netDegree; i++, pinNum++) {
    if (file.eof()) {
      break;
    }
    getline(file, line);
    
    Cell *node;
    istringstream stream(line, istringstream::in);
    stream >> nodeName; stream >> pinDir; 
    stream >> garbage;  stream >> xoffset; 
    stream >> yoffset;

    if (pinDir == PIN_DIR_INPUT_STRING) dir = PIN_DIR_INPUT;
    else if (pinDir == PIN_DIR_OUTPUT_STRING) dir = PIN_DIR_OUTPUT;

    node = DesignGetNode(nodeName);
    /* Adjust the offset so that it is from the left bottom corner of the 
       cell */
    xoffset = xoffset + ((*node).CellGetWidth()/2);
    yoffset = yoffset + ((*node).CellGetHeight()/2);
    cellPinNum = (*node).CellGetNumPins() + 1;
    PinName = nodeName + "_" + getStrFromInt(cellPinNum);
    Pin *newPin = new Pin(cellPinNum, (int)xoffset, (int)yoffset, dir, *node, PinName);

    Msg = "Created Pin " + PinName + " on cell " + nodeName;
    common_message(Msg);                                   
    (*newPin).Connect(newNet);
    (*newPin).PinSetParentCell(*node);
    (*node).CellAddPin(newPin);
    newNet.NetAddPin(*newPin);
  }

  Msg = "Read and created " + getStrFromInt((pinNum - 1)) + " pins";
  common_message(Msg);                                   
}


void
Design::DesignFileReadOneNet(ifstream &file)
{
  string line, garbage;
  string NetDegree, NetName;
  unsigned int netDegree;
  Cell *node;
  Net *newNet;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }

    istringstream stream(line, istringstream::in);
    stream >> NetDegree;

    if (NetDegree != NET_DEGREE_PROPERTY) {
      string Msg = "Error in benchmark file.";
      common_error(Msg);
    }

    stream >> garbage; stream >> netDegree;
    stream >> NetName;
    Net *newNet;
    string Msg;

    newNet = new Net(0, NetName);
    DesignNets[NetName] = newNet;

    Msg = "Created Net " + NetName + " of Degree " + getStrFromInt(netDegree);
    DesignFileReadPins(file, netDegree, *newNet);
    DesignAddOneNetToDesignDB(newNet, 1.0);
    break;
  }
}

void
Design::DesignFileReadNets(ifstream& file)
{
  string Property, Value;
  unsigned int numNets, numPins, numNodes;
  int idx, netCount;

  for (idx = 0; idx < NUM_NET_PROPERTIES; idx++) {
    DesignProcessProperty(file, Property, Value);
    if (Property == NUM_NETS_PROPERTY) {
      numNodes = atoi(Value.data());
    } else if (Property == NUM_PINS_PROPERTY) {
      numPins = atoi(Value.data());
    }
  }

  while (!file.eof()) {
    DesignFileReadOneNet(file);
  }
}

void
Design::DesignReadNets()
{
  int NumCells;
  string Msg;

  _STEP_BEGIN("Read Nets");
  DesignOpenFile(DesignNetFileName);
  DesignFileReadHeader(DesignFile);
  DesignFileReadNets(DesignFile);

  Msg += "Loaded " + getStrFromInt(DesignNets.size()) + " nets";

  cout << Msg << endl;
  _STEP_END("Read Nets");
}

void
Design::DesignFileReadOneRow(ifstream &file)
{
  string rowProperty, garbage, line;
  bool rowBegin;
  int rowCoordinate, subRowOrigin;
  unsigned int height, siteWidth, siteSpacing, numSites;
  rowOrientation rowType;
  objOrient siteOrient;
  siteSymmetry symmetry;
  vector<int> subRowOrigins;
  vector<unsigned int> numSitesForSubRows;

  rowBegin = false;
  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    istringstream stream(line, istringstream::in);
    stream >> rowProperty;
    if (strToLower(rowProperty) == strToLower(ROW_BEGIN_KEYWORD)) {
      if (rowBegin == true) {_ASSERT_TRUE("Benchmark error");break;}
      rowBegin = true;
      stream >> rowProperty;
      rowType = PhysRowGetRowTypeFromStr(rowProperty);
    } else if (strToLower(rowProperty) == strToLower(ROW_COORDINATE)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> rowCoordinate;
    } else if (strToLower(rowProperty) == strToLower(ROW_HEIGHT)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> height;
    } else if (strToLower(rowProperty) == strToLower(ROW_SITE_WIDTH)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> siteWidth;
    } else if (strToLower(rowProperty) == strToLower(ROW_SITE_SPACING)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error: Row begin unspecified");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> siteSpacing;
    } else if (strToLower(rowProperty) == strToLower(ROW_SITE_ORIENTATION)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error: Row begin unspecified");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> rowProperty;
      siteOrient = getOrientationFromStr(rowProperty);
    } else if (strToLower(rowProperty) == strToLower(ROW_SITE_SYMMETRY)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error: Row begin unspecified");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> rowProperty;
      symmetry = PhysRowGetSiteSymmetryFromStr(rowProperty);
    } else if (strToLower(rowProperty) == strToLower(SUBROW_ORIGIN)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error: Row begin unspecified");break;}
      stream >> garbage;
      if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
      stream >> subRowOrigin;
      stream >> rowProperty;
      if (strToLower(rowProperty) == strToLower(SUBROW_NUM_SITES)) {
	stream >> garbage;
	if (garbage != ":") {_ASSERT_TRUE("Benchmark error: Missing ':'");break;}
	stream >> numSites;
      } else {_ASSERT_TRUE("Benchmark error: Cannot find NumSites for a subRow");break;}
      subRowOrigins.push_back(subRowOrigin);
      numSitesForSubRows.push_back(numSites);
    } else if (strToLower(rowProperty) == strToLower(ROW_END_KEYWORD)) {
      if (rowBegin == false) {_ASSERT_TRUE("Benchmark error: Row begin unspecified");break;}
      rowBegin = false;
      break;
    } else {
      {_ASSERT_TRUE("Benchmark error: Unknown keyword");break;}
      break;
    }
  }
  PhysRow *row;
  row = new PhysRow(rowType, rowCoordinate, height, siteWidth, siteSpacing,
		    siteOrient, symmetry);
  
  VECTOR_FOR_ALL_ELEMS_DOUBLE(subRowOrigins, int, subRowOrigin,
			      numSitesForSubRows, unsigned int, numSites) {
    (*row).PhysRowAddSubRow(subRowOrigin, numSites);
  } END_FOR;
  DesignAddOnePhysRowToDesignDB(row);
}

void
Design::DesignFileReadRows(ifstream& file)
{
  string Property, Value;
  unsigned int numRows;
  int idx, netCount;

  for (idx = 0; idx < NUM_ROW_PROPERTIES; idx++) {
    do {
      DesignProcessProperty(file, Property, Value);
    } while (Property == "" && !file.eof());
    if (Property == NUM_ROWS_PROPERTY) {
      numRows = atoi(Value.data());
    }
  }
  for (idx = 0; idx < numRows; idx++) {
    DesignFileReadOneRow(file);
  }
}

void
Design::DesignReadRows()
{
  int numRows;
  string Msg;

  _STEP_BEGIN("Read physical rows");
  DesignOpenFile(DesignSclFileName);
  DesignFileReadHeader(DesignFile);
  DesignFileReadRows(DesignFile);

  Msg += "Loaded " + getStrFromInt(DesignPhysRows.size()) + " rows";

  cout << Msg << endl;
  _STEP_END("Read physical rows");
}

void
Design::DesignFileReadOneFixedCell(ifstream &file)
{
  string line;
  string cellName, orient, fixed;
  unsigned int xPos, yPos;
  bool cellFixed;
  
  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    istringstream stream(line, istringstream::in);
    stream >> cellName;
    stream >> xPos;
    stream >> yPos;
    stream >> orient;
    stream >> orient;

    cellFixed = false;
    if (stream >> fixed) {
      if (fixed == "/FIXED") {
	cellFixed = true;
      }
    }

    Cell *thisCell;
    objOrient orientation;

    thisCell = DesignGetNode(cellName);
    if (thisCell == NIL(Cell *)) {
      _ASSERT_TRUE("Cell not found in design database!");
    }
    (*thisCell).CellSetXpos(xPos);
    (*thisCell).CellSetYpos(yPos);

    orientation = N;
    if (orient != "") {
      orientation = 
	getOrientationFromStr(orient);
    }
    (*thisCell).CellSetOrientation(orientation);
    (*thisCell).CellSetIsFixed(cellFixed);
    if (cellFixed) {
      NumFixedCells++;
    }
  }
}

void
Design::DesignFileReadFixedCells(ifstream& file)
{
  string Property, Value;
  unsigned int numCells;
  int idx;

  for (idx = 0; idx < NUM_FIXED_CELL_PROPERTIES; idx++) {
    do {
      DesignProcessProperty(file, Property, Value);
    } while (Property == "" && !file.eof());
  }
  numCells = (*this).DesignGetNumCells();
  for (idx = 0; idx < numCells; idx++) {
    DesignFileReadOneFixedCell(file);
  }
}

void 
Design::DesignReadCellPlacement()
{
  int numFixed;
  string Msg;
  
  _STEP_BEGIN("Read fixed cells");
  DesignOpenFile(DesignPlFileName);
  DesignFileReadHeader(DesignFile);
  DesignFileReadFixedCells(DesignFile);
  
  Msg += "Loaded " + getStrFromInt(NumFixedCells) + " cells positions";
  _STEP_END("Read fixed cells");
}

void 
Design::DesignFileReadCellMap(ifstream &file)
{
  Cell *thisCell;
  string cellName, cellType;
  string cellOrigName, libCellName, cellLibName;
  string line;
  string garbage;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    if (line.find('#') == 0) {
      continue;
    }

    istringstream stream(line, istringstream::in);
    cellType.clear();
    stream >> cellName;
    stream >> cellOrigName;
    stream >> cellLibName;
    stream >> libCellName;
    stream >> cellType;
    
    thisCell = DesignGetNode(cellName);
    (*thisCell).CellSetOrigName(cellOrigName);
    (*thisCell).CellSetLibCellName(libCellName);
    (*thisCell).CellSetLibName(cellLibName);
    if (cellType == "FF") {
      (*thisCell).CellSetIsSequential(true);
    } else if (cellType == "PAD") {
      (*thisCell).CellSetIsPort(true);
    }
  }
}

void
Design::DesignReadMapFile()
{
  _STEP_BEGIN("Read map file");
  if (DesignMapFileName == "") {
    cout << "Map file cannot be found.. Skipping" << endl;
  } else {
    DesignOpenFile(DesignMapFileName);
    DesignFileReadCellMap(DesignFile);
  }
  _STEP_END("Read map file");
}

void 
Design::DesignFileReadCmds(ifstream &file)
{
  string cmd, arg;
  string line;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    if (line.find('#') == 0) {
      continue;
    }

    istringstream stream(line, istringstream::in);
    /* Typically a command of the form: <command> <arg>  */
    stream >> cmd;
    if (cmd == "set_clock_port") {
      stream >> arg;
      cout << "Read command \"set_clock_port " << arg << "\"" << endl;
      Cell *thisCell = DesignGetNode(arg);
      Pin *PinPtr;
      CELL_FOR_ALL_PINS((*thisCell), PIN_DIR_ALL, PinPtr) {
	(*PinPtr).PinSetIsClock(true);
      } CELL_END_FOR;
    } else if (cmd == "set_clock_period") {
      double clkPeriod;
      stream >> clkPeriod;
      cout << "Read command \"set_clock_period " << clkPeriod << " ns\"" << endl;
      DesignSetClockPeriod(clkPeriod);
    }
  }
}

void 
Design::DesignReadCmdsFile()
{
  _STEP_BEGIN("Read commands file");
  if (DesignCmdsFileName == "") {
    cout << "Commands file cannot be found.. Skipping" << endl;
  } else {
    DesignOpenFile(DesignCmdsFileName);
    DesignFileReadCmds(DesignFile);
  }
  _STEP_END("Read commands file");
}

void 
Design::DesignFileReadPinsMap(ifstream &file)
{
  string pin, libPin;
  string line;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    if (line.find('#') == 0) {
      continue;
    }

    istringstream stream(line, istringstream::in);
    /* Typically a command of the form: <command> <arg>  */
    stream >> pin;
    stream >> libPin;
    
    unsigned int _pos = pin.find('_');
    if (_pos == string::npos) {
      cout << "Error: Invalid pin name " << pin << endl;
      continue;
    }
    
    string cellName = pin.substr(0, _pos);
    Cell *thisCellPtr = DesignGetNode(cellName);    
    if (thisCellPtr == NIL(Cell *)) {
      cout << "Error: Cell " << cellName << "Not found " << endl;
    }
    Pin *thisPin = (*thisCellPtr).CellGetPinByName(pin);
    (*thisPin).PinSetLibName(libPin);
  }
}

void 
Design::DesignReadPinsMapFile()
{
  _STEP_BEGIN("Read Pins Map file");
  if (DesignPinsMapFileName == "") {
    cout << "Pins Map file cannot be found.. Skipping" << endl;
  } else {
    DesignOpenFile(DesignPinsMapFileName);
    DesignFileReadPinsMap(DesignFile);
  }
  _STEP_END("Read Pins Map file");
}

void 
Design::DesignFileReadCellDelays(ifstream &file)
{
  string libCell, outputPin, inputPin;
  double rise, fall;
  string line;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }
    if (line.find('#') == 0) {
      continue;
    }

    istringstream stream(line, istringstream::in);
    /* Typically a command of the form: <command> <arg>  */
    stream >> libCell;
    stream >> outputPin;
    stream >> inputPin;
    stream >> rise;
    stream >> fall;
    
    if (fall < 0) fall = -fall;
    if (rise < 0) rise = -rise;
    double cellDelay = (fall + rise) / 2;

    DesignAddDelayArc(libCell, outputPin, inputPin, cellDelay);
  }
}

void 
Design::DesignReadCellDelaysFile()
{
  _STEP_BEGIN("Read Cell Delays file");
  if (DesignCellDelaysFileName == "") {
    cout << "Cell Delays file cannot be found.. Skipping" << endl;
  } else {
    DesignOpenFile(DesignCellDelaysFileName);
    DesignFileReadCellDelays(DesignFile);
  }
  _STEP_END("Read Cell Delays file");
}
