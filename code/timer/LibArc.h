#include <iostream>
#include <string>

#include "LookupTable.h"
#include "LibPin.h"
class LibCell;

using namespace std;

class LibArc {
  LibPin* fromPin;
  LibPin* toPin;
  LibCell* parentCell;
  LookupTable* cellRise;
  LookupTable* cellFall;
  LookupTable* riseTrans;
  LookupTable* fallTrans;
  bool is_disabled;
  string timing_sense;
  string timing_type;

 public:
  LibArc (LibPin*& from, LibPin*& to, LibCell*& parent) : fromPin(from), toPin(to), parentCell(parent) {
    cellRise = cellFall = riseTrans = fallTrans = NULL;
    timing_sense = "";
    timing_type = "";
    is_disabled = false;
  }

  void  setCellRiseTable(LookupTable*& table) {
    cellRise = table;
  }
  void  setCellFallTable(LookupTable*& table) {
    cellFall = table;
  }
  void  setRiseTransTable(LookupTable*& table) {
    riseTrans = table;
  }
  void  setFallTransTable(LookupTable*& table) {
    fallTrans = table;
  }

  double getCellRiseDelay(double inSlope, double outLoad) {
    if(!cellRise) {
      //cout<<"Missing rise delay table "<<fromPin->getName()<<" "<<toPin->getName()<<endl;
      return 0;
    }
    return cellRise->getValue(inSlope,outLoad);
  }
  double getCellFallDelay(double inSlope, double outLoad) {
    if(!cellFall) {
      //cout<<"Missing fall delay table "<<fromPin->getName()<<" "<<toPin->getName()<<endl;
      return 0;
    }
    return cellFall->getValue(inSlope,outLoad);
  }

  double getRiseTransition(double inSlope, double outLoad) {
    if(!riseTrans) {
      //cout<<"Missing rise trans table "<<fromPin->getName()<<" "<<toPin->getName()<<endl;
      return inSlope;
    }
    double value = riseTrans->getValue(inSlope,outLoad);
    if(value==0) return inSlope;
    else return value;
  }
  double getFallTransition(double inSlope, double outLoad) {
    if(!fallTrans) {
      //cout<<"Missing fall trans table "<<fromPin->getName()<<" "<<toPin->getName()<<endl;
      return inSlope;
    }
    double value = fallTrans->getValue(inSlope,outLoad);
    if(value==0) return inSlope;
    else return value;
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
  string getTimingSense() {
    return timing_sense;
  }
  void setTimingSense(string sense) {
    timing_sense = sense;
  }
  string getTimingType() {
    return timing_type;
  }
  void setTimingType(string tt) {
    timing_type = tt;
  }

};
