# include <AnalyticalSolve.h>

void
getDesignBounds(Design& myDesign, double *minx, double *miny, 
		double *maxx, double *maxy) 
{
  *minx = 0.0;
  *miny = 0.0;
  *maxx = 119.0;
  *maxy = 119.0;
}

void
mskGetVarBounds(Design& myDesign, HyperGraph& myGraph, 
		vector<Cell*>& listOfCells,
		MSKidxt **subj, MSKrealt **bu, MSKrealt **bl,
		MSKboundkeye **bk, unsigned int& numNonZero) 
{
  double maxx, maxy, minx, miny;
  unsigned int numVars;
  unsigned int numCells;
  
  numCells = listOfCells.size();
  numVars = (2 * numCells);
  *subj = (MSKidxt *) malloc(sizeof(MSKidxt) * numVars);
  *bu = (MSKrealt *) malloc(sizeof(MSKrealt) * numVars);
  *bl = (MSKrealt *) malloc(sizeof(MSKrealt) * numVars);
  *bk = (MSKboundkeye *) malloc(sizeof(MSKboundkeye) * numVars);

  getDesignBounds(myDesign, &minx, &miny, &maxx, &maxy);

  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *(listOfCells[i]);
    (*subj)[i] = i;
    (*bu)[i] = maxx - 
      ((double)thisCell.CellGetWidth()) / GRID_COMPACTION_RATIO;
    (*bl)[i] = minx;
    (*bk)[i] = MSK_BK_RA;
    (*subj)[i+numCells] = i + numCells;
    (*bu)[i+numCells] = maxy - 
      ((double)thisCell.CellGetHeight()) / GRID_COMPACTION_RATIO;
    (*bl)[i+numCells] = miny;
    (*bk)[i+numCells] = MSK_BK_RA;
  }
  
  numNonZero = numVars;
}

double
getMaxWLForDesign(Design &myDesign) 
{
  double rtv;

  rtv = 400.00;
}
