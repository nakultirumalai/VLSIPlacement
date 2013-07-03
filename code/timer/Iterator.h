#include <string>
#include <map>
#include <vector>
 
class Pin;
class Cell;
class Net;

using namespace std;

class Design_CellIterator {
  map<string,Cell*> cells;
  map<string,Cell*>::iterator cellIter; 

 public:
  void attach(Design*& d) {
    cells = d->cells;
    cellIter = cells.begin();
  }

  Cell* next() {
    Cell* curCell;
    if(cellIter!=cells.end()) {
      curCell = cellIter->second;
      cellIter++;
      return curCell;
    }
    return NULL;
  }
};

class Design_PinPortIterator {
  map<string,Pin*> pins;
  map<string,Pin*> ports;
  map<string,Pin*>::iterator pinIter; 

public:
  void attach(Design*& d) {
    pins = d->pins;
    ports = d->ports;
    pinIter = pins.begin();
  }

  Pin* next() {
    Pin* curPin;
    if(pinIter!=pins.end() && pinIter!=ports.end()) {
      curPin = pinIter->second;
      pinIter++;
      if(pinIter==pins.end())
	pinIter = ports.begin();
      return curPin;
    }
    return NULL;
  }
};

class Design_PinIterator {
  map<string,Pin*> pins;
  map<string,Pin*>::iterator pinIter; 

public:
  void attach(Design*& d) {
    pins = d->pins;
    pinIter = pins.begin();
  }

  Pin* next() {
    Pin* curPin;
    if(pinIter!=pins.end()) {
      curPin = pinIter->second;
      pinIter++;
      return curPin;
    }
    return NULL;
  }
};


class Design_PortIterator {
  map<string,Pin*> pins;
  map<string,Pin*>::iterator pinIter; 

public:
  void attach(Design*& d) {
    pins = d->ports;
    pinIter = pins.begin();
  }

  Pin* next() {
    Pin* curPin;
    if(pinIter!=pins.end()) {
      curPin = pinIter->second;
      pinIter++;
      return curPin;
    }
    return NULL;
  }
};


class Design_NetIterator {
  map<string,Net*> nets;
  map<string,Net*>::iterator netIter; 

public:
  void attach(Design*& d) {
    nets = d->nets;
    netIter = nets.begin();
  }

  Net* next() {
    Net* curNet;
    if(netIter!=nets.end()) {
      curNet = netIter->second;
      netIter++;
      return curNet;
    }
    return NULL;
  }
};
