#include <iostream>
#include <string>

using namespace std;

class LibCell;

class LibPin {
  string name;
  LibCell* parentCell;
  double pincap;

 public:
  LibPin (string name, LibCell*& cell) : name(name), parentCell(cell) {
    pincap = 0;
  }

  string getName() {
    return name;
  }
  void setPincap (double cap) {
    pincap = cap;
  }
  double getPincap() {
    return pincap;
  }
  LibCell* getLibCell() {
    return parentCell;
  }
};
