#include <iostream>
#include <string>
#include <vector>

#include "LibArc.h"
#include "TestArc.h"
//#include "LibPin.h"

using namespace std;

class LibCell {
  string name;
  float height;
  float width;
  vector<LibArc*> arcs;
  vector<TestArc*> testArcs;
  map<string,LibPin*> pins;

 public:
  LibCell (string name, float height, float width) : name(name), height(height), width(width) {}

  LibCell (string name) : name(name) {}

  float getHeight() {
    return height;
  }
  float getWidth() {
    return width;
  }
  string getName() {
    return name;
  }

  void insertArc(LibArc*& arc) {
    arcs.push_back(arc);
  }
  void insertTestArc(TestArc*& arc) {
    testArcs.push_back(arc);
  }
  void insertPin(LibPin*& pin) {
    //pins.insert(make_pair(pin->getName(),pin));
    pins[pin->getName()]=pin;
  }
  LibPin* findLibPin(string name) {
    map<string,LibPin*>::iterator it;
    it=pins.find(name);
    if(it!=pins.end())
      return it->second;
    else
      return NULL;
  }
  vector<LibArc*> getLibArcs() {
    return arcs;
  }
  vector<TestArc*> getTestArcs() {
    return testArcs;
  }
  LibArc* findLibArc(string fromPin, string toPin) {
    LibArc *theArc;
    for(unsigned int i=0; i<arcs.size(); i++) {
      theArc = arcs[i];
      if(theArc->getFromPin()->getName()==fromPin && theArc->getToPin()->getName()==toPin)
	return theArc;
    }
    return NULL;
  }

  void checkLibPins() {
    map<string,LibPin*>::iterator it;
    cout<<name<<endl;
    it = pins.begin();
    while(it!=pins.end()) {
      cout<<it->second->getName()<<" "<<it->second->getPincap()<<endl;
      it++;
    }
  }
};
