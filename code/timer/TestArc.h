#include <iostream>
#include <string>

class LookupTable;
class LibPin;
class LibCell;

using namespace std;

class TestArc {
  LibPin* fromPin;
  LibPin* toPin;
  LibCell* parentCell;
  LookupTable* riseConstraint;
  LookupTable* fallConstraint;
  bool is_disabled;
  string test_type;

 public:
  TestArc (LibPin*& from, LibPin*& to, LibCell*& parent) : fromPin(from), toPin(to), parentCell(parent) {
    riseConstraint = fallConstraint = NULL;
    is_disabled = false;
    test_type = "";
  }

  void  setRiseConstraintTable(LookupTable*& table) {
    riseConstraint = table;
  }
  void  setFallConstraintTable(LookupTable*& table) {
    fallConstraint = table;
  }

  double getRiseDelay(double inSlope, double outLoad) {
    if(!riseConstraint) {
      //cout<<"Missing rise delay table "<<fromPin->getName()<<" "<<toPin->getName()<<endl;
      return 0;
    }
    return riseConstraint->getValue(inSlope,outLoad);
  }
  double getFallDelay(double inSlope, double outLoad) {
    if(!fallConstraint) {
      //cout<<"Missing fall delay table "<<fromPin->getName()<<" "<<toPin->getName()<<endl;
      return 0;
    }
    return fallConstraint->getValue(inSlope,outLoad);
  }

  LibPin* getFromPin() {
    return fromPin;
  }
  LibPin* getToPin() {
    return toPin;
  }
  void disableArc() {
    is_disabled = true;
  }
  bool isDisabled() {
    return is_disabled;
  }
  void setTestType(string tt) {
    test_type = tt;
  }
  string getTestType() {
    return test_type;
  }

};
