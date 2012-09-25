# include <Design.h>

using namespace std;

Cell *
Design::DesignGetNode(string nodeName)
{
  Cell *foundNode;
  
  foundNode = DesignCells[nodeName];
  
  return (foundNode);
}

void
Design::DesignFileReadHeader(ifstream& file)
{
  string buffer;
  int i;
  
  i = 0;
  while (!file.eof() && i < 4) {
    getline(file, buffer);
    i++;
  }
}

void
Design::DesignAddOneCellToDesignDB(string NodeName, int height, int width,
				   bool terminalCell)
{
  Cell *newCell = new Cell(height, width, NodeName, terminalCell);
  DesignCells[NodeName] = newCell;
}

void 
Design::DesignFileReadOneNode(ifstream& file)
{
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

    stream >> NodeName; stream >> height; stream >> width;
    if (!stream.eof()) stream >> terminalString;
  
    if (terminalString == NODE_TERMINAL_KEYWORD) {
      terminalCell = true;
    }
  
    DesignAddOneCellToDesignDB(NodeName, height, width, terminalCell);
    NumNets++;
    break;
  }
}

void
Design::DesignFileReadNodes(ifstream& file)
{
  string Property, Value;
  unsigned int numNodes, numTerminals;
  int idx, cellCount;

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

  DesignOpenFile(DesignCellFileName);
  DesignFileReadHeader(DesignFile);
  DesignFileReadNodes(DesignFile);

  Msg += "Loaded " + getStrFromInt(DesignCells.size()) + " cells";

  common_message(Msg);  
}

void 
Design::DesignFileReadOneNet(ifstream &file)
{
  string line, garbage;
  string NetDegree, NetName;
  unsigned int netDegree;

  while (!file.eof()) {
    getline(file, line);
    if (line == "") {
      continue;
    }

    istringstream stream(line, istringstream::in);  
    stream >> NetDegree;

    if (NetDegree != NET_DEGREE_PROPERTY) {
      string msg = "Error in benchmark file.";
      common_error(msg);
    }

    stream >> garbage;
    stream >> netDegree;
    stream >> NetName;

    Cell *node;
    Net *newNet;
    string nodeName, pinDir, garbage;
    string PinName;
    string Msg;
    unsigned int pinNum;
    float xoffset, yoffset;
    char dir;

    newNet = new Net(0, NetName);
    DesignNets[NetName] = newNet;

    Msg = "Created Net " + NetName + " of Degree " + getStrFromInt(netDegree);
    //common_message(Msg);

    pinNum = 1;
    for (int i=0; i < netDegree; i++, pinNum++) {
      getline(file, line);

      istringstream stream(line, istringstream::in);  

      stream >> nodeName;
      stream >> pinDir;
      stream >> garbage;
      stream >> xoffset;
      stream >> yoffset;

      if (pinDir == PIN_DIR_INPUT_STRING) {
	dir = PIN_DIR_INPUT;
      } else if (pinDir == PIN_DIR_OUTPUT_STRING) {
	dir = PIN_DIR_OUTPUT;
      }

      PinName = nodeName + "_" + NetName + "_" + getStrFromInt(pinNum);
      
      Pin *newPin = new Pin(0, (int)xoffset, (int)yoffset, dir, *node, PinName);

      Msg = "Created Pin " + PinName + " on cell " + nodeName;
      //common_message(Msg);

      (*newPin).Connect(*newNet);
      node = DesignGetNode(nodeName);
      (*newPin).PinSetParentCell(*node);
      (*newNet).NetAddPin(*newPin);
    }

    Msg = "Read and created " + getStrFromInt((pinNum - 1)) + " pins";
    //common_message(Msg);
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

  DesignOpenFile(DesignNetFileName);
  DesignFileReadHeader(DesignFile);
  DesignFileReadNets(DesignFile);

  Msg += "Loaded " + getStrFromInt(DesignNets.size()) + " nets";

  common_message(Msg);  
}

void
Design::DesignOpenFile(string FileName)
{
  if (DesignFile.is_open()) {
    DesignFile.close();
  }
  DesignFile.open(FileName.data(), ifstream::in);
}

void
Design::DesignCloseFile(void)
{
  if (DesignFile.is_open()) {
    DesignFile.close();
  }
}

void 
Design::DesignReadDesign(string DesignPath, string DesignName)
{
  Name = DesignName;

  DesignCellFileName = DesignPath + DIR_SEP + DesignName + DIR_SEP + DesignName + DESIGN_CELL_FILE_EXT;
  DesignNetFileName = DesignPath + DIR_SEP + DesignName + DIR_SEP + DesignName + DESIGN_NET_FILE_EXT;

  DesignReadCells();
  DesignReadNets();

  DesignFile.close();
}

Design::Design(string DesignPath, string DesignName)
{
  DesignReadDesign(DesignPath, DesignName);
}

