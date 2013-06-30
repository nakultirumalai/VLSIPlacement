#include <iostream>
#include <string>
#include <vector>

using namespace std;
class TimingArc;
class Pin;
class TestData;

#define RISE 0
#define FALL 1
#define MAX_VALUE 9999
#define MIN_VALUE -9999

class TimingPoint {
  Pin *designPin;
  vector<TimingArc*> fromArcs;
  vector<TimingArc*> toArcs;
  int atLevel;
  double slope[2];
  double arrivalT[2];
  double reqArrivalT[2];
  double slack[2];
  unsigned has_clock : 1;
  unsigned is_endpoint : 1;
  unsigned is_at_valid : 1;
  unsigned is_rat_valid :1;
  vector<TestData*> tests;

 public:
  TimingPoint (Pin*& actualPin) : designPin(actualPin) {
    atLevel = -1;
    slope[RISE] = slope[FALL] = MIN_VALUE;
    arrivalT[RISE] = arrivalT[FALL] = MIN_VALUE;
    reqArrivalT[RISE] = reqArrivalT[FALL] = MAX_VALUE;
    slack[RISE] = slack[FALL] = MAX_VALUE;
    has_clock = 0;
    is_endpoint = 0;
    is_at_valid = 0;
    is_rat_valid = 0;
  }

  void insertFromArc(TimingArc*& arc) {
    fromArcs.push_back(arc);
  }
  void insertToArc(TimingArc*& arc) {
    toArcs.push_back(arc);
  }
  void setLevel(int level) {
    atLevel = level;
  }
  int getLevel() {
    return atLevel;
  }
  vector<TimingArc*> getToArcs() {
    return toArcs;
  }
  vector<TimingArc*> getFromArcs() {
    return fromArcs;
  }
  Pin* getPin() {
    return designPin;
  }
  unsigned hasClock() {
    return has_clock;
  }
  void setHasClock() {
    has_clock = 1;
  }
  void setArrival(int rf, double at) {
    arrivalT[rf] = at;
  }
  double getArrival(int rf) {
    return arrivalT[rf];
  }
  void setSlope(int rf, double s) {
    slope[rf] = s;
  }
  double getSlope(int rf) {
    return slope[rf];
  }
  double getRequired(int rf) {
    return reqArrivalT[rf];
  }
  void setRequired(int rf, double rat) {
    reqArrivalT[rf] = rat;
  }
  void insertTest(TestData*& td) {
    tests.push_back(td);
  }
  vector<TestData*> getTests() {
    return tests;
  }
  double getSlack(int rf) {
    return slack[rf];
  }
  void setSlack(int rf, double slk) {
    slack[rf] = slk;
  }
  double getWorstSlack(int& rf) {
    if(slack[1]<slack[0]) {
      rf = 1;
      return slack[1];
    }
    else {
      rf = 0;
      return slack[0];
    }
  }
  bool isEndPoint() {
    return is_endpoint;
  }
  void setAsEndPoint() {
    is_endpoint = 1;
  }
  void invalidateAT() {
    is_at_valid = 0;
  }
  void invalidateRAT() {
    is_rat_valid = 0;
  }
  unsigned isATValid() {
    return is_at_valid;
  }
  unsigned isRATValid() {
    return is_rat_valid;
  }
  void setATValid() {
    is_at_valid = 1;
  }
  void setRATValid() {
    is_rat_valid = 1;
  }

};
