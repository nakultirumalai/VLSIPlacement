# include <Design.h>

# ifndef DESIGN_ITER_H
# define DESIGN_ITER_H

# define STD_CELL_HEIGHT 40

/* Macros for iteration over elements of the design */

/* Use this iterator if you want to see only the cells which are not 
   hidden in the design */
# define DESIGN_FOR_ALL_CELLS(Design, CellName, CellPtr)		\
  {									\
  map<string, Cell*> DesignCells = Design.DesignGetCells();		\
  map<string, Cell*> DesignClusters = Design.DesignGetClusters();	\
  DesignCells.insert(DesignClusters.begin(), DesignClusters.end());	\
  map<string, Cell*>::iterator mapIter;					\
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) { \
  CellName = mapIter->first;						\
  CellPtr = mapIter->second;						\
  if ((*CellPtr).CellIsHidden()) continue;

/* Use this iterator to iterate over all the clusters of the design */
# define DESIGN_FOR_ALL_CLUSTERS(Design, CellName, ClusterCellPtr)	\
  {									\
  map<string, Cell*> &DesignClusters = Design.DesignGetClusters();	\
  map<string, Cell*>::iterator mapIter;					\
  for (mapIter = DesignClusters.begin(); mapIter != DesignClusters.end(); mapIter++) { \
  CellName = mapIter->first;						\
  ClusterCellPtr = mapIter->second;					\

/* Use this iterator if you want to see all the cells in the design */
# define DESIGN_FOR_ALL_CELLS_HIDDEN(Design, CellName, CellPtr)		\
  {									\
  map<string, Cell*> DesignCells = Design.DesignGetCells();		\
  map<string, Cell*> DesignClusters = Design.DesignGetClusters();	\
  DesignCells.insert(DesignClusters.begin(), DesignClusters.end());	\
  map<string, Cell*>::iterator mapIter;					\
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) { \
  CellPtr = mapIter->second;						\
  if ((*CellPtr).CellIsHidden()) continue;

# define DESIGN_FOR_ALL_CELLS_ON_TOP(Design, CellName, CellPtr) \
  { \
  map<string, Cell*> DesignCells = Design.DesignGetCells(); \
  map<string, Cell*>::iterator mapIter; \
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) { \
    CellName = mapIter->first; \
    CellPtr = mapIter->second; \
    if ((*CellPtr).CellIsClusterChild()) \
      continue; 
    

# define DESIGN_FOR_ALL_NETS(Design, NetName, NetPtr) \
  { \
  map<string, Net*> DesignNets = Design.DesignGetNets(); \
  map<string, Net*>::iterator mapIter; \
  for (mapIter = DesignNets.begin(); mapIter != DesignNets.end(); mapIter++) { \
    NetName = (string)mapIter->first; \
    NetPtr = (Net *)mapIter->second;  \
    if ((*NetPtr).NetIsHidden()) continue; 

/* Cell pointers for which the filterFunc returns a TRUE value are filtered */
# define DESIGN_FOR_ALL_STD_CELLS_FILT(Design, CellName, CellPtr, filterFunc)	\
  { \
  map<string, Cell*> DesignCells = Design.DesignGetCells(); \
  map<string, Cell*>::iterator mapIter; \
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) { \
    CellName = mapIter->first; \
    CellPtr = mapIter->second; \
    if ((*CellPtr).CellIsMacro()) \
      continue;			  \
    if (filterFunc(CellPtr))   \
      continue;

# define DESIGN_FOR_ALL_STD_CELLS(Design, CellName, CellPtr) \
  { \
  map<string, Cell*> DesignCells = Design.DesignGetCells(); \
  map<string, Cell*>::iterator mapIter; \
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) { \
    CellName = mapIter->first; \
    CellPtr = mapIter->second; \
    if (!DesignCellIsStdCell(Design, (*CellPtr))) \
      continue;                                   \
    if ((*CellPtr).CellIsClusterChild())	  \
      continue;

# define DESIGN_FOR_ALL_SEQ_CELLS(Design, CellName, CellPtr)	\
  { \
  map<string, Cell*> DesignCells = Design.DesignGetCells(); \
  map<string, Cell*>::iterator mapIter; \
  for (mapIter = DesignCells.begin(); mapIter != DesignCells.end(); mapIter++) { \
    CellName = mapIter->first; \
    CellPtr = mapIter->second; \
    if ((*CellPtr).CellIsMacro()) \
      continue;			  \
    if (!((*CellPtr).CellIsSequential()))	\
      continue;

# define DESIGN_FOR_ALL_ROWS(Design, RowIdx, PhysRowPtr) \
  { \
  vector<PhysRow *> DesignPhysRows = Design.DesignGetRows(); \
  uint NumRows = DesignPhysRows.size();              \
  for (RowIdx = 0; RowIdx < NumRows; RowIdx++) { \
    PhysRowPtr = DesignPhysRows[RowIdx];

# define DESIGN_FOR_ALL_BINS(Design, BinIdx, BinPtr) \
  { \
  vector<Bin*> DesignBins = Design.DesignGetBins(); \
  uint NumBins = DesignBins.size();              \
  for (BinIdx = 0; BinIdx < NumBins; BinIdx++) { \
    BinPtr = DesignBins[BinIdx];

# define DESIGN_FOR_ALL_PATHS(Design, PathIdx, PathPtr)	\
  {							\
  vector<Path*> &DesignPaths = Design.DesignGetPaths();	\
  uint NumPaths = DesignPaths.size();			\
  for (PathIdx = 0; PathIdx < NumPaths; PathIdx++) {	\
  PathPtr = DesignPaths[PathIdx];			

# define DESIGN_END_FOR }}
  
# endif

