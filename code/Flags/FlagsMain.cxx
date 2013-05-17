# include <Flags.h>

/* File having set and clear functions for each flag added to the 
   enum list */

bool CellIsClustered(Cell *cellPtr)
{
  bool rtv;

  void *obj = (void *)cellPtr;
  
  //  rtv = FlagsDefGetFlag(CELL_IS_CLUSTERED_FLAG, obj);
  rtv = (*cellPtr).CellIsClustered();

  return (rtv);
}

void CellSetIsClustered(Cell *cellPtr)
{
  // void *obj = (void *)cellPtr;
  
  //FlagsDefSetFlag(CELL_IS_CLUSTERED_FLAG, obj);
  (*cellPtr).CellSetIsClustered(true);
}

void CellClearIsClustered(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearFlag(CELL_IS_CLUSTERED_FLAG, obj);
}

void CellClearAllIsClustered(void)
{
  FlagsDefClearAllObjs(CELL_IS_CLUSTERED_FLAG);
}

unsigned int PortGetNumRelatedCells(Cell *obj)
{
  unsigned int rtv;

  rtv = FlagsDefGetInt(PORT_NUM_RELATED_CELLS, (void *)obj);
  
  return (rtv);
}

void PortSetNumRelatedCells(Cell *portPtr, unsigned int numCells)
{
  void *obj = (void *)portPtr;
  
  FlagsDefSetInt(PORT_NUM_RELATED_CELLS, obj, numCells);
}

void PortClearAllNumRelatedCells(void)
{
  FlagsDefClearAllObjs(PORT_NUM_RELATED_CELLS);
}

void PortClearNumRelatedCells(Cell *portPtr)
{
  void *obj = (void *)portPtr;

  FlagsDefClearFlag(CELL_IS_CLUSTERED_FLAG, obj);
}

bool NetIsHidden(Net *netPtr)
{
  bool rtv;

  void *obj = (void *)netPtr;
  
  rtv = FlagsDefGetFlag(NET_IS_HIDDEN_FLAG, obj);
  
  return (rtv);
}

void NetSetIsHidden(Net *netPtr)
{
  void *obj = (void *)netPtr;
  
  FlagsDefSetFlag(NET_IS_HIDDEN_FLAG, obj);
}

void NetClearIsHidden(Net *netPtr)
{
  void *obj = (void *)netPtr;

  FlagsDefClearFlag(NET_IS_HIDDEN_FLAG, obj);
}

void NetClearAllIsHidden(Net *netPtr)
{
  FlagsDefClearAllObjs(NET_IS_HIDDEN_FLAG);
}

void PinSetOriginalPin(Pin *thisPin, Pin *originalPin)
{
  void *obj = (void *)thisPin;
  void *ptr = (void *)originalPin;

  FlagsDefSetPtr(PIN_ORIGINAL_PIN_FLAG, obj, ptr);
}

void* PinGetOriginalPin(Pin *thisPin)
{
  void *obj = (void *)thisPin;
  void *ptr = FlagsDefGetPtr(PIN_ORIGINAL_PIN_FLAG, obj);
  
  return (ptr);
}

void PinClearOriginalPin(Pin *thisPin)
{
  void *obj = (void *)thisPin;
  
  FlagsDefClearPtr(PIN_ORIGINAL_PIN_FLAG, obj);
}

void PinClearAllOriginalPin(void)
{
  FlagsDefClearAllObjs(PIN_ORIGINAL_PIN_FLAG);
}

double PathGetCellDelay(Net *thisNet)
{
  void *obj = (void *)thisNet;
  double totalCellDelay = 
    FlagsDefGetDouble(PATH_TOTAL_CELL_DELAY, obj);
}

void PathSetCellDelay(Net *thisNet, double totalCellDelay)
{
  void *obj = (void *)thisNet;
  FlagsDefSetDouble(PATH_TOTAL_CELL_DELAY, obj, 
		    totalCellDelay);
}

void PathClearCellDelay(Net *thisNet)
{
  void *obj = (void *)thisNet;

  FlagsDefClearDouble(PATH_TOTAL_CELL_DELAY, obj);
}

void PathClearAllCellDelay(void)
{
  FlagsDefClearAllObjs(PATH_TOTAL_CELL_DELAY);
}

/* Net is pseudo flag */
bool NetIsPseudo(Net *netPtr)
{
  bool rtv;

  void *obj = (void *)netPtr;
  
  rtv = FlagsDefGetFlag(NET_IS_PSEUDO_FLAG, obj);
  
  return (rtv);
}

void NetSetIsPseudo(Net *netPtr)
{
  void *obj = (void *)netPtr;
  
  FlagsDefSetFlag(NET_IS_PSEUDO_FLAG, obj);
}

void NetClearIsPseudo(Net *netPtr)
{
  void *obj = (void *)netPtr;

  FlagsDefClearFlag(NET_IS_PSEUDO_FLAG, obj);
}

void NetClearAllIsPseudo(void)
{
  FlagsDefClearAllObjs(NET_IS_PSEUDO_FLAG);
}

/* Pin is pseudo flag */
bool PinIsPseudo(Pin *pinPtr)
{
  bool rtv;

  void *obj = (void *)pinPtr;
  
  rtv = FlagsDefGetFlag(PIN_IS_PSEUDO_FLAG, obj);
  
  return (rtv);
}

void PinSetIsPseudo(Pin *pinPtr)
{
  void *obj = (void *)pinPtr;
  
  FlagsDefSetFlag(PIN_IS_PSEUDO_FLAG, obj);
}

void PinClearIsPseudo(Pin *pinPtr)
{
  void *obj = (void *)pinPtr;

  FlagsDefClearFlag(PIN_IS_PSEUDO_FLAG, obj);
}

void PinClearAllIsPseudo(void)
{
  FlagsDefClearAllObjs(PIN_IS_PSEUDO_FLAG);
}

/* Cell is pseudo flag */
bool CellIsPseudo(Cell *cellPtr)
{
  bool rtv;

  void *obj = (void *)cellPtr;

  rtv = FlagsDefGetFlag(CELL_IS_PSEUDO_FLAG, obj);

  return (rtv);
}

void CellSetIsPseudo(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetFlag(CELL_IS_PSEUDO_FLAG, obj);
}

void CellClearIsPseudo(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearFlag(CELL_IS_PSEUDO_FLAG, obj);
}

void CellClearAllIsPseudo(void)
{
  FlagsDefClearAllObjs(CELL_IS_PSEUDO_FLAG);
}

/* Pseudo port of cell */
void* CellGetPseudoPort(Cell *cellPtr)
{
  void *rtv;

  void *obj = (void *)cellPtr;
  
  rtv = FlagsDefGetPtr(CELL_PSEUDO_PORT, obj);
  
  return (rtv);
}

void CellSetPseudoPort(Cell *cellPtr, Cell *pseudoPort)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetPtr(CELL_PSEUDO_PORT, obj, (void *)pseudoPort);
}

void CellClearPseudoPort(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefClearPtr(CELL_PSEUDO_PORT, obj);
}

void CellClearAllPseudoPort(void)
{
  FlagsDefClearAllObjs(CELL_PSEUDO_PORT);
}

/* Cell is fixed flag */
bool CellIsFixed(Cell *cellPtr)
{
  bool rtv;

  void *obj = (void *)cellPtr;

  rtv = FlagsDefGetFlag(CELL_IS_FIXED_FLAG, obj);

  return (rtv);
}

void CellSetIsFixed(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetFlag(CELL_IS_FIXED_FLAG, obj);
}

void CellClearIsFixed(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearFlag(CELL_IS_FIXED_FLAG, obj);
}

void CellClearAllIsFixed(void)
{
  FlagsDefClearAllObjs(CELL_IS_FIXED_FLAG);
}

double CellGetDblX(Cell *cellPtr)
{
  double rtv;
  void *obj = (void *)cellPtr;

  rtv = FlagsDefGetDouble(CELL_DBLX, obj);
  
  return rtv;
}

void CellSetDblX(Cell *cellPtr, double cellDblX)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetDouble(CELL_DBLX, obj, cellDblX);
}

void CellClearDblX(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearDouble(CELL_DBLX, obj);
}

void CellClearAllDblX(Cell *cellPtr) 
{
  FlagsDefClearAllObjs(CELL_DBLX);
}

double CellGetDblY(Cell *cellPtr)
{
  double rtv;
  void *obj = (void *)cellPtr;

  rtv = FlagsDefGetDouble(CELL_DBLY, obj);
  
  return rtv;
}

void CellSetDblY(Cell *cellPtr, double cellDblY)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetDouble(CELL_DBLY, obj, cellDblY);
}

void CellClearDblY(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearDouble(CELL_DBLY, obj);
}

void CellClearAllDblY(Cell *cellPtr) 
{
  FlagsDefClearAllObjs(CELL_DBLY);
}

/* Move Cost Flag on Cell */

double CellGetMoveCost(Cell *cellPtr)
{
  double rtv;

  void *obj = (void *)cellPtr;
  
  rtv = FlagsDefGetDouble(CELL_MOVE_COST, obj);
  
  return (rtv);
}

void CellSetMoveCost(Cell *cellPtr, double cost)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetDouble(CELL_MOVE_COST, obj, cost);
}

void CellClearMoveCost(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearDouble(CELL_MOVE_COST, obj);
}

void CellClearAllMoveCost(void)
{
  FlagsDefClearAllObjs(CELL_MOVE_COST);
}

bool CellIsLocked(Cell *cellPtr)
{
  bool rtv;

  void *obj = (void *)cellPtr;
  
  rtv = FlagsDefGetFlag(CELL_IS_LOCKED_FLAG, obj);
  
  return (rtv);
}

void CellSetIsLocked(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;
  
  FlagsDefSetFlag(CELL_IS_LOCKED_FLAG, obj);
}

void CellClearIsLocked(Cell *cellPtr)
{
  void *obj = (void *)cellPtr;

  FlagsDefClearFlag(CELL_IS_LOCKED_FLAG, obj);
}

void CellClearAllIsLocked(void)
{
  FlagsDefClearAllObjs(CELL_IS_LOCKED_FLAG);
}

/* PhysRowSupply Flag */
double PhysRowGetSupply(PhysRow * rowPtr)
{
  double rtv;

  void *obj = (void *)rowPtr;
  
  rtv = FlagsDefGetDouble(PHYSROW_SUPPLY, obj);
  
  return (rtv);
}

void PhysRowSetSupply(PhysRow *rowPtr, double supply)
{
  void *obj = (void *)rowPtr;
  
  FlagsDefSetDouble(PHYSROW_SUPPLY, obj, supply);
}

void PhysRowClearSupply(PhysRow *rowPtr)
{
  void *obj = (void *)rowPtr;

  FlagsDefClearDouble(PHYSROW_SUPPLY, obj);
}

void PhysRowClearAllSupply(void)
{
  FlagsDefClearAllObjs(PHYSROW_SUPPLY);
}
