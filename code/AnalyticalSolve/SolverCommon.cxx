# include <AnalyticalSolve.h>

void
mskGetVarBoundsX(Design& myDesign, vector<Cell*>& listOfCells,
		 vector<uint> &sub_vec, vector<double> &bu_vec,
		 vector<double> &bl_vec, vector<MSKboundkeye> &bk_vec)
{
  uint maxx, maxy;
  double upperBound, lowerBound, delta;
  unsigned int numCells;
  unsigned int count;
  
  myDesign.DesignGetBoundingBox(maxx, maxy);
  numCells = listOfCells.size();
  count = sub_vec.size();
  delta = 0.001;
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *(listOfCells[i]);
    sub_vec.push_back(count++);
    upperBound = ((double)maxx)/GRID_COMPACTION_RATIO - 
      ((double)thisCell.CellGetWidth()) / GRID_COMPACTION_RATIO;
    lowerBound = 0.0 + delta;
    bu_vec.push_back(upperBound);
    bl_vec.push_back(lowerBound);
    bk_vec.push_back(MSK_BK_RA);
  }
}

void
mskGetVarBoundsY(Design& myDesign, vector<Cell*>& listOfCells,
		 vector<uint> &sub_vec, vector<double> &bu_vec,
		 vector<double> &bl_vec, vector<MSKboundkeye> &bk_vec)
{
  uint maxx, maxy;
  double upperBound, lowerBound, delta;
  unsigned int numCells;
  unsigned int count;
  
  myDesign.DesignGetBoundingBox(maxx, maxy);
  numCells = listOfCells.size();  
  count = sub_vec.size();
  delta = 0.001;
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *(listOfCells[i]);
    sub_vec.push_back(count++);
    upperBound = ((double)maxy)/GRID_COMPACTION_RATIO - 
      ((double)thisCell.CellGetHeight()) / GRID_COMPACTION_RATIO;
    lowerBound = 0.0 + delta;
    bu_vec.push_back(upperBound);
    bl_vec.push_back(lowerBound);
    bk_vec.push_back(MSK_BK_RA);
  }
}

