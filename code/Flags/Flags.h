# ifndef FLAGS_H
# define FLAGS_H
# include <common.h>
# include <Cell.h>
# include <Net.h>

typedef enum {
  PATH_TOTAL_CELL_DELAY = 0,
  TOTAL_NUM_DOUBLE_FLAGS
} flagDoubleType;

typedef enum {
  CELL_IS_CLUSTERED_FLAG = 0,
  NET_IS_HIDDEN_FLAG,
  PORT_NUM_RELATED_CELLS,
  TOTAL_NUM_INT_FLAGS
} flagIntType;

typedef enum {
  PIN_ORIGINAL_PIN_FLAG = 0,
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

extern bool CellIsClustered(Cell *);
extern void CellSetIsClustered(Cell *);
extern void CellClearIsClustered(Cell *);
extern void CellClearAllIsClustered(void);

extern unsigned int PortGetNumRelatedCells(Cell *);
extern void PortSetNumRelatedCells(Cell *, unsigned int);
extern void PortClearAllNumRelatedCells(void);
extern void PortClearNumRelatedCells(Cell *);

extern bool NetIsHidden(Net *);
extern void NetSetIsHidden(Net *);
extern void NetClearIsHidden(Net *);
extern void NetClearAllIsHidden(Net *);

extern double PathGetCellDelay(Net *);
extern void PathSetCellDelay(Net *, double);
extern void PathClearCellDelay(Net *);
extern void PathClearAllCellDelay(void);

extern void PinClearAllOriginalPin(void);
extern void PinClearOriginalPin(Pin *);
extern void* PinGetOriginalPin(Pin *);
extern void PinSetOriginalPin(Pin *, Pin *);

# endif
