# include <Design.h>

#define _WRITE_HEADER(outputFile)	\
  outputFile << "# Created       :       Jan  6 2005" << endl; \
  outputFile << "# User          :       Gi-Joon Nam & Mehmet Yildiz at IBM Austin Research({gnam, mcan}@us.ibm.com)" << endl;


void DesignWriteNets(Design &myDesign) 
{
}

void DesignWriteNodes(Design &myDesign, string fname) 
{
  string cellName, designName, fileName;
  Cell *cellPtr;
  ofstream opFile;

  cout << "Writing nodes file.." << endl;

  if (fname == "") {
    designName = myDesign.DesignGetName();
    fileName = designName + ".nodes";
  } else {
    fileName = fname;
  }
  
  opFile.open(fileName.data(), ifstream::out);
  opFile << "UCLA nodes 1.0" << endl; 
  _WRITE_HEADER(opFile);
  opFile << "NumNodes :              " << myDesign.DesignGetNumCells() << endl;
  opFile << "NumTerminals :           " << myDesign.DesignGetNumTerminalCells() << endl;

  DESIGN_FOR_ALL_CELLS_ON_TOP(myDesign, cellName, cellPtr) {
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
  string cellName;
  Cell *cellPtr;
  ofstream opFile;

  _STEP_BEGIN("Writing placement for current design");
  opFile.open(fname.data(), ifstream::out);
  opFile << "UCLA pl 1.0" << endl;
  opFile << "# Created      : " << getCurrentTime() << endl;
  opFile << "# User         : tirumanl@arugula" << endl;
  opFile << "# Platform     : Linux 3.2.0-35-generic" << endl;
  opFile << endl << endl;

  DESIGN_FOR_ALL_CELLS_ON_TOP(myDesign, cellName, cellPtr) {
    opFile << cellName << "\t" << (*cellPtr).CellGetXpos() << "\t" << (*cellPtr).CellGetYpos() << 
      "\t:\t" << getStrForOrientation((*cellPtr).CellGetOrientation());
    if ((*cellPtr).CellIsTerminal() && !(*cellPtr).CellIsPort()) {
      opFile << "\t" << "/FIXED";
    }
    opFile << endl;
  } DESIGN_END_FOR;

  _STEP_END("Writing placement for current design");
  
  opFile.close();
}

void DesignWriteBookShelfOutput(Design& myDesign)
{
  //DesignWriteNets(myDesign);
  //  DesignWriteNodes(myDesign, "");
  DesignWritePlacement(myDesign, (myDesign.DesignGetName() + ".ourplacer.pl"));
}

