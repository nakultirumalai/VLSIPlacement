#include <string>
#include <map>
#include <vector>
#include "Cell.h"
#include "Pin.h"
#include "Net.h"
 
using namespace std;

 
class Design {
  string top_level;
  map<string,Cell*> cells;
  map<string,Pin*> pins;
  map<string,Net*> nets; 
  map<string,LibCell*> libCells;
  map<string,Pin*> ports; 
  static Design* curDesign;

public:
  Design(string name) : top_level(name) {
  }

  string getName() {
    return top_level;
  }

  void add(Cell*& cell) {
    cells.insert(make_pair(cell->getName(),cell));
  }
  void add(LibCell*& libCell) {
    libCells.insert(make_pair(libCell->getName(),libCell));
  }

  void add(Pin*& pin) {
    if(pin->isPort())
      ports.insert(make_pair(pin->getName(),pin));
    else
      //      pins.insert(make_pair(pin->getName(),pin));
      pins[pin->getName()]=pin;
  }

  void add(Net*& net) {
    nets.insert(make_pair(net->getName(),net));
  }

  Cell* findCell(string name) {
    map<string,Cell*>::iterator it;
    if( (it=cells.find(name)) == cells.end() )
      return NULL;
    else
      return it->second;
  }

  LibCell* findLibCell(string name) {
    map<string,LibCell*>::iterator it;
    if( (it=libCells.find(name)) == libCells.end() )
      return NULL;
    else
      return it->second;
  }

  Pin* findPin(string name) {
    map<string,Pin*>::iterator it;
    if( (it=pins.find(name)) == pins.end() ) {
      if( (it=ports.find(name)) == ports.end() )
	return NULL;
      else
	return it->second;
    }
    else
      return it->second;
  }

  Net* findNet(string name) {
    map<string,Net*>::iterator it;
    if( (it=nets.find(name)) == nets.end() )
      return NULL;
    else
      return it->second;
  }

  void checkDesign() {
    map<string,Pin*>::iterator it;
    //for(it=pins.begin(); it!=pins.end(); it++)
    //cout<<it->second->getName().c_str()<<endl;
    for(it=ports.begin(); it!=ports.end(); it++)
      cout<<it->second->getName().c_str()<<endl;
  }

  static Design* getCurDesign() {
    return curDesign;
  }
  static void setCurDesign(Design*& design) {
    curDesign = design;
  }
  friend class Design_CellIterator;
  friend class Design_PinIterator;
  friend class Design_PinPortIterator;
  friend class Design_PortIterator;
  friend class Design_NetIterator;
};
