# include <TimingModels.h>

/* Helper functions internal to the module */
/*******************************************************************************
   Builds a map which is used for cell lookup instead of using a vector 
   to lookup cells which is required in the objective function formulation
   routines. Given the cell pointer, returns the index of the cell
*******************************************************************************/
map<Cell *, uint>
getCellLookupMap(vector<Cell *>inputCells)
{
  Cell *subjCell;
  map<Cell*, uint> cellIdxToPtrMap;
  uint count;

  count = 0;
  VECTOR_FOR_ALL_ELEMS(inputCells, Cell*, subjCell) {
    cellIdxToPtrMap[subjCell] = count++;
  } END_FOR;
  
  return cellIdxToPtrMap;
}
