# include <Design.h>

#define _WRITE_HEADER(outputFile)	\
  outputFile << "# Created       :       Jan  6 2005" << endl; \
  outputFile << "# User          :       Gi-Joon Nam & Mehmet Yildiz at IBM Austin Research({gnam, mcan}@us.ibm.com)" << endl;


void DesignWriteNets(Design &myDesign) 
{
}

void DesignWriteNodes(Design &myDesign) 
{
  string cellName, designName, fileName;
  Cell *cellPtr;
  ofstream opFile;

  cout << "Writing nodes file.." << endl;

  designName = myDesign.DesignGetName();
  fileName = designName + ".nodes";

  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nodes 1.0" << endl; 
  _WRITE_HEADER(opFile);
  opFile << "NumNodes :              " << myDesign.DesignGetNumCells();
  opFile << "NumTerminals :           " << myDesign.DesignGetNumTerminalCells();

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

void DesignWritePlacement(Design &myDesign) 
{
}

void DesignWriteBookShelfOutput(Design& myDesign)
{
  DesignWriteNets(myDesign);
  DesignWriteNodes(myDesign);
  DesignWritePlacement(myDesign);
}
