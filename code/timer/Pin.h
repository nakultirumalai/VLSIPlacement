#include <string>

class Net;
class Cell;

using namespace std;

class Pin {
  string full_name;
  float offset_x;
  float offset_y;
  string direction;
  Cell* owner_cell;
  bool is_port;
  bool is_clock_pin;
  LibPin* libPin;
  Net* connectedNet;

 public:
  Pin (string name, string inout, Cell*& cell, bool isClock) : full_name(name), direction(inout), owner_cell(cell), is_clock_pin(isClock) {
    offset_x = offset_y = 0.0;
    is_port = false;
    libPin = NULL;
    connectedNet = NULL;
  }
  Pin (string name, string inout) : full_name(name), direction(inout) {
    offset_x = offset_y = 0.0;
    is_port = true;
    is_clock_pin = false;
    libPin = NULL;
    connectedNet = NULL;
  }

  string getName() {
    return full_name;
  }
  Cell* getCell() {
    return owner_cell;
  }
  bool isPort() {
    return is_port;
  }
  string getDirection() {
    return direction;
  }
  bool isClockPin() {
    return is_clock_pin;
  }
  void setLibPin(LibPin*& pin) {
    libPin = pin;
  }
  LibPin* getLibPin() {
    return libPin;
  }
  void setNet(Net*& net) {
    connectedNet = net;
  }
  Net* getNet() {
    return connectedNet;
  }
  double getLocationX() {
    return (owner_cell->getLocationX() + offset_x);
  }
  double getLocationY() {
    return (owner_cell->getLocationY() + offset_y);
  }
};
