# ifndef CELL_MACROS_H
# define CELL_MACROS_H
# include <Flags.h>

/* Define the iterators here */
# define CELL_FOR_ALL_PINS(CellObj, Dir, PinPtr) {	\
  vector<Pin*> CellPins = CellObj.CellGetPins();	\
  for (int n=0; n<CellPins.size(); n++) {		\
    PinPtr = CellPins[n];				\
    if (PinIsPseudo(PinPtr)) continue;			\
    if (Dir != PIN_DIR_ALL &&				\
	Dir != (*PinPtr).PinGetDirection()) {		\
      continue;						\
    }							

# define CELL_FOR_ALL_PINS_NOFILT(CellObj, Dir, PinPtr) {	\
  vector<Pin*> CellPins = CellObj.CellGetPins();	\
  for (int n=0; n<CellPins.size(); n++) {		\
    PinPtr = CellPins[n];				\
    if (Dir != PIN_DIR_ALL &&				\
	Dir != (*PinPtr).PinGetDirection()) {		\
      continue;						\
    }							

# define CELL_FOR_ALL_NETS(CellObj, Dir, NetPtr) {	\
  vector<Pin*> CellPins = CellObj.CellGetPins();	\
  Pin* PinPtr;						\
  Net* NetPtr;						\
  map<Net*, bool> netHash;				\
  for (int n=0; n<CellPins.size(); n++) {		\
    PinPtr = CellPins[n];                               \
    if (PinIsPseudo(PinPtr)) continue;			\
    if (Dir != PIN_DIR_ALL &&				\
	Dir != (*PinPtr).PinGetDirection()) {		\
      continue;						\
    }							\
    NetPtr = &((*PinPtr).PinGetNet());			

# define CELL_END_FOR }}

# endif

