#include <string>
#include <vector>
#include <map>
#include <math.h>

class Pin;
#define RES 0
#define CAP 0

using namespace std;

class NetModel {
  double source_x;
  double source_y;
  map<string,double> sinks_x;
  map<string,double> sinks_y;
  double mean_x;
  double mean_y;
  bool isValid;

 public:

  NetModel() {
    isValid = false;
    //updateNetValues;
  }
  void updateNetValues() {
    //Get pin locations and set mean value

    isValid = true;
  }
  double getNetDelay(Pin*& sink) {
    if(!isValid)
      updateNetValues();

    bool toRight=true;
    double locationX, locationY;
    //Horizontal trunk
    locationX = sink->getLocationX();
    locationY = sink->getLocationY();
    if(source_x>locationX)
      toRight = false;

    double elmoreDelay=0;
    double commonBranch = fabs(mean_y-source_y);
    map<string,double>::iterator it=sinks_x.begin();
    for(; it!=sinks_x.end(); it++) {
      if(it->first==sink->getName()) {
      }
    }
  }

};

class Net {
  string full_name;
  Pin* source_pin;
  vector<Pin*> sink_pins; 
  int sourcePinCount;

 public:
  Net (string name) : full_name(name) {
    sourcePinCount = 0;
  }

  string getName() {
    return full_name;
  }
  void setSinkPin(Pin* pin) {
    sink_pins.push_back(pin);
  }
  void setSourcePin(Pin* pin) {
    sourcePinCount++;
    if(sourcePinCount>1) {
      cout<<"Multiple source pins for net: "<<full_name<<endl;
      cout<<"Pin: "<<pin->getName()<<" "<<pin->getDirection()<<" "<<pin->isPort()<<endl;
    }
    else
      source_pin = pin;
  }

  vector<Pin*> getSinkPins() {
    return sink_pins;
  }
  Pin* getSourcePin() {
    return source_pin;
  }

  void printPins() {
    cout<<source_pin->getName()<<endl;
    for(unsigned i=0; i<sink_pins.size(); i++)
      cout<<sink_pins[i]->getName()<<endl;
  }
};
