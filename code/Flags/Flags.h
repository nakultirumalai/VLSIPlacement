# ifndef FLAGS_H
# define FLAGS_H
# include <common.h>
# include <Cell.h>
# include <Net.h>

typedef enum {
  CELL_IS_CLUSTERED_FLAG = 1,
  NET_IS_HIDDEN_FLAG,
  TOTAL_NUM_FLAGS
} flagType;

extern void FlagsInit();
extern void FlagsDefSetFlag(flagType fType, void* obj);
extern void FlagsDefClearFlag(flagType fType, void* obj);
extern void FlagsDefClearFlagAllObjs(flagType fType);
extern bool FlagsDefGetFlag(flagType fType, void *obj);

extern bool CellIsClustered(Cell *);
extern void CellSetIsClustered(Cell *);
extern void CellClearIsClustered(Cell *);
extern void CellClearAllIsClustered(void);

extern bool NetIsHidden(Net *);
extern void NetSetIsHidden(Net *);
extern void NetClearIsHidden(Net *);
extern void NetClearAllIsHidden(Net *);

# endif
