# ifndef FLAGS_H
# define FLAGS_H

# include <common.h>
# include <Cell.h>
# include <Net.h>
# include <PhysRow.h>

class PhysRow;
class LegalizeBin;

typedef enum {
  PATH_TOTAL_CELL_DELAY = 0,
  CELL_DBLX,
  CELL_DBLY,
  CELL_ORIG_DBLX,
  CELL_ORIG_DBLY,
  CELL_MOVE_COST,
  PHYSROW_SUPPLY,
  LEGALIZE_BIN_L_VALUE,
  LEGALIZE_BIN_B_VALUE,
  TOTAL_NUM_DOUBLE_FLAGS
} flagDoubleType;

typedef enum {
  CELL_IS_CLUSTERED_FLAG = 0,
  NET_IS_HIDDEN_FLAG,
  PORT_NUM_RELATED_CELLS,
  PIN_IS_PSEUDO_FLAG,
  NET_IS_PSEUDO_FLAG,
  CELL_IS_PSEUDO_FLAG,
  CELL_IS_FIXED_FLAG,
  CELL_IS_LOCKED_FLAG,
  CELL_IS_STAR_NODE_FLAG,
  CELL_TO_BE_MOVED_FLAG,
  TOTAL_NUM_INT_FLAGS
} flagIntType;

typedef enum {
  PIN_ORIGINAL_PIN_FLAG = 0,
  CELL_PSEUDO_PORT,
  BIN_OF_CELL,
  CLUSTER_OF_CELL,
  TOTAL_NUM_PTR_FLAGS
} flagPtrType;

extern void FlagsInit();

/* Set functions */
extern void FlagsDefSetFlag(flagIntType fType, void* obj);
extern void FlagsDefSetInt(flagIntType fType, void* obj, unsigned int val);
extern void FlagsDefSetPtr(flagPtrType fType, void* obj, void *ptr);
extern void FlagsDefSetDouble(flagDoubleType fType, void* obj, double val);

/* Get functions */ 
extern bool FlagsDefGetFlag(flagIntType fType, void *obj);
extern unsigned int FlagsDefGetInt(flagIntType fType, void* obj);
extern void* FlagsDefGetPtr(flagPtrType fType, void *obj);
extern double FlagsDefGetDouble(flagDoubleType fType, void *obj);

/* Clear functions */
extern void FlagsDefClearFlag(flagIntType fType, void* obj);
extern void FlagsDefClearInt(flagIntType fType, void* obj);
extern void FlagsDefClearPtr(flagPtrType fType, void* obj);
extern void FlagsDefClearDouble(flagDoubleType fType, void* obj);

/* Clear all functions */
extern void FlagsDefClearAllObjs(flagIntType fType);
extern void FlagsDefClearAllObjs(flagPtrType fType);
extern void FlagsDefClearAllObjs(flagDoubleType fType);

/* FLAG: Check if a cell is a cluster */
extern bool CellIsClustered(Cell *);
extern void CellSetIsClustered(Cell *);
extern void CellClearIsClustered(Cell *);
extern void CellClearAllIsClustered(void);

/* PTR: Set the cluster object of a corresponding cluster cell */
extern void* CellGetCluster(Cell *);
extern void CellSetCluster(Cell *, void *);
extern void CellClearCluster(Cell *);
extern void CellClearAllCluster(void);

extern unsigned int PortGetNumRelatedCells(Cell *);
extern void PortSetNumRelatedCells(Cell *, unsigned int);
extern void PortClearAllNumRelatedCells(void);
extern void PortClearNumRelatedCells(Cell *);

extern bool NetIsHidden(Net *);
extern void NetSetIsHidden(Net *);
extern void NetClearIsHidden(Net *);
extern void NetClearAllIsHidden(Net *);

extern bool NetIsPseudo(Net *);
extern void NetSetIsPseudo(Net *);
extern void NetClearIsPseudo(Net *);
extern void NetClearAllIsPseudo(void);

extern double PathGetCellDelay(Net *);
extern void PathSetCellDelay(Net *, double);
extern void PathClearCellDelay(Net *);
extern void PathClearAllCellDelay(void);

extern void PinClearAllOriginalPin(void);
extern void PinClearOriginalPin(Pin *);
extern void* PinGetOriginalPin(Pin *);
extern void PinSetOriginalPin(Pin *, Pin *);

extern bool PinIsPseudo(Pin *);
extern void PinSetIsPseudo(Pin *);
extern void PinClearIsPseudo(Pin *);
extern void PinClearAllIsPseudo(Pin *);

extern bool CellIsPseudo(Cell *);
extern void CellSetIsPseudo(Cell *);
extern void CellClearIsPseudo(Cell *);
extern void CellClearAllIsPseudo(void);

extern void* CellGetPseudoPort(Cell *);
extern void CellSetPseudoPort(Cell *, Cell *);
extern void CellClearPseudoPort(Cell *);
extern void CellClearAllPseudoPort(void);

extern bool CellIsFixed(Cell *);
extern void CellSetIsFixed(Cell *);
extern void CellClearIsFixed(Cell *);
extern void CellClearAllIsFixed(Cell *);

extern double CellGetDblX(Cell *);
extern void CellSetDblX(Cell *, double);
extern void CellClearDblX(Cell *);
extern void CellClearAllDblX(Cell *);

extern double CellGetDblY(Cell *);
extern void CellSetDblY(Cell *, double);
extern void CellClearDblY(Cell *);
extern void CellClearAllDblY(Cell *);

extern double CellGetOrigDblX(Cell *);
extern void CellSetOrigDblX(Cell *, double);
extern void CellClearOrigDblX(Cell *);
extern void CellClearAllOrigDblX(Cell *);

extern double CellGetOrigDblY(Cell *);
extern void CellSetOrigDblY(Cell *, double);
extern void CellClearOrigDblY(Cell *);
extern void CellClearAllOrigDblY(Cell *);

extern double CellGetMoveCost(Cell *);
extern void CellSetMoveCost(Cell *, double);
extern void CellClearMoveCost(Cell *);
extern void CellClearAllMoveCost(void);

extern bool CellIsLocked(Cell *);
extern void CellSetIsLocked(Cell *);
extern void CellClearIsLocked(Cell *);
extern void CellClearAllIsLocked(void);

extern bool CellIsStarNode(Cell *);
extern void CellSetIsStarNode(Cell *);
extern void CellClearIsStarNode(Cell *);
extern void CellClearAllIsStarNode(void);

extern double PhysRowGetSupply(PhysRow *);
extern void PhysRowSetSupply(PhysRow *, double);
extern void PhysRowClearSupply(PhysRow *);
extern void PhysRowClearAllSupply(void);

extern void* CellGetBin(Cell *);
extern void CellSetBin(Cell *, void *);
extern void CellClearBin(Cell *);
extern void CellClearAllBin(void);

extern double LegalizeBinGetLValue(LegalizeBin*);
extern void LegalizeBinSetLValue(LegalizeBin*, double);
extern void LegalizeBinClearLValue(LegalizeBin*);
extern void LegalizeBinClearAllLValue(void);

extern double LegalizeBinGetBValue(LegalizeBin*);
extern void LegalizeBinSetBValue(LegalizeBin*, double);
extern void LegalizeBinClearBValue(LegalizeBin*);
extern void LegalizeBinClearAllBValue(void);

/* FLAG: Check if a cell is to be moved */
extern bool CellToBeMoved(Cell *);
extern void CellSetToBeMoved(Cell *);
extern void CellClearToBeMoved(Cell *);
extern void CellClearAllToBeMoved(void);

# endif
