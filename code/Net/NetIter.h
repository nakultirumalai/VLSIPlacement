# ifndef NET_ITER_H
# define NET_ITER_H

/* Macros for iteration over pins and cells of the net */

/* Skip hidden pins and cells */
# define NET_FOR_ALL_CELLS(Net, CellPtr) \
  {						\
  Pin *pinPtr;					\
  map<string, Pin*> NetPins = Net.NetGetPins();	\
  map<string, Pin*>::iterator mapIter; \
  for (mapIter = NetPins.begin(); mapIter != NetPins.end(); mapIter++) { \
    pinPtr = (Pin*)(mapIter->second);					\
    if ((*pinPtr).isHidden) continue;					\
    Cell &ParentCell = (*pinPtr).PinGetParentCell();			\
    CellPtr = &ParentCell;

# define NET_FOR_ALL_PINS(Net, PinPtr)			\
  {							\
  vector<Pin*> &netPins = Net.NetGetAllPinsVector();	\
  uint __numPins = netPins.size();			\
  for (uint idx = 0; idx < __numPins; idx++) {		\
  PinPtr = netPins[idx];				\
  if ((*PinPtr).isHidden) continue;                                

# define NET_FOR_ALL_PINS_NO_FILT(Net, PinPtr) \
  {						\
  map<string, Pin*> NetPins = Net.NetGetPins();	\
  map<string, Pin*>::iterator mapIter; \
  for (mapIter = NetPins.begin(); mapIter != NetPins.end(); mapIter++) { \
  PinPtr = mapIter->second;						\

# define NET_FOR_ALL_PINS_DIR(Net, direction, PinPtr)	\
  {								\
  map<string, Pin*> NetPins = Net.NetGetPins(direction);	\
  map<string, Pin*>::iterator mapIter;					\
  for (mapIter = NetPins.begin(); mapIter != NetPins.end(); mapIter++) { \
  PinPtr = mapIter->second;						\
  if ((*PinPtr).PinIsHidden()) continue;                                

# define NET_END_FOR }}
  
# endif

