#include <string>
#include <vector>
#include "LibCell.h"

#define G_SIZE 3.0

class Pin;
class LibCell;

using namespace std;

class Cell {
  string full_name;
  float location_x;
  float location_y;
  bool is_sequential;
  vector<Pin*> related_pins;
  LibCell* libCell;

 public:
  Cell (string name, LibCell*& cell, bool seq) : full_name(name), is_sequential(seq), libCell(cell) {
    location_x = location_y = 0.0;
  }

  string getName() {
    return full_name;
  }

  LibCell* getLibCell() {
    return libCell;
  }
  double getLocationX() {
    return location_x;
  }
  double getLocationY() {
    return location_y;
  }
};
