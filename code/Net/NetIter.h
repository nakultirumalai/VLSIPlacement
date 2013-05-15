# ifndef NET_ITER_H
# define NET_ITER_H

/* Macros for iteration over pins and cells of the net */

# define NET_FOR_ALL_CELLS(Net, CellPtr) \
  {						\
  map<string, Pin*> NetPins = Net.NetGetPins();	\
  map<string, Pin*>::iterator mapIter; \
  for (mapIter = NetPins.begin(); mapIter != NetPins.end(); mapIter++) { \
    Cell &ParentCell = (*((Pin *)(mapIter->second))).PinGetParentCell(); \
    CellPtr = &ParentCell;

# define NET_FOR_ALL_PINS(Net, PinPtr) \
  {						\
  map<string, Pin*> NetPins = Net.NetGetPins();	\
  map<string, Pin*>::iterator mapIter; \
  for (mapIter = NetPins.begin(); mapIter != NetPins.end(); mapIter++) { \
    PinPtr = mapIter->second;

# define NET_FOR_ALL_PINS_DIR(Net, direction, PinPtr)	\
  {								\
  map<string, Pin*> NetPins = Net.NetGetPins(direction);	\
  map<string, Pin*>::iterator mapIter;					\
  for (mapIter = NetPins.begin(); mapIter != NetPins.end(); mapIter++) { \
    PinPtr = mapIter->second;

# define NET_END_FOR }}
  
# endif

