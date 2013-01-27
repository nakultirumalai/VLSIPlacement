# ifndef FLAGS_H
# define FLAGS_H
# include <common.h>
# include <Cell.h>
# include <Net.h>

typedef enum {
  CELL_IS_CLUSTERED_FLAG = 0,
  NET_IS_HIDDEN_FLAG,
  TOTAL_NUM_INT_FLAGS
} flagIntType;

typedef enum {
  PIN_ORIGINAL_PIN_FLAG = 0,
  TOTAL_NUM_PTR_FLAGS
} flagPtrType;

extern void FlagsInit();
extern void FlagsDefSetFlag(flagIntType fType, void* obj);
extern void FlagsDefClearFlag(flagIntType fType, void* obj);
extern void FlagsDefClearAllObjs(flagIntType fType);
extern void FlagsDefClearAllObjs(flagPtrType fType);
extern bool FlagsDefGetFlag(flagIntType fType, void *obj);

extern void FlagsDefSetPtr(flagPtrType fType, void* obj, void *ptr);
extern void FlagsDefClearPtr(flagPtrType fType, void* obj);
extern void FlagsDefClearFlagAllObjs(flagPtrType fType);
extern void* FlagsDefGetPtr(flagPtrType fType, void *obj);

extern bool CellIsClustered(Cell *);
extern void CellSetIsClustered(Cell *);
extern void CellClearIsClustered(Cell *);
extern void CellClearAllIsClustered(void);

extern bool NetIsHidden(Net *);
extern void NetSetIsHidden(Net *);
extern void NetClearIsHidden(Net *);
extern void NetClearAllIsHidden(Net *);

extern void PinClearAllOriginalPin(void);
extern void PinClearOriginalPin(Pin *);
extern void* PinGetOriginalPin(Pin *);
extern void PinSetOriginalPin(Pin *, Pin *);

# endif
