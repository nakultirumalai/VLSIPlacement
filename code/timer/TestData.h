#include <iostream>
#include <string>

using namespace std;


class TestData {
  TimingPoint *testNode;
  TimingPoint *refNode;
  TestArc* testArcPtr;
  double refValue[2];
  double cachedTestRAT[2];

 public:
 TestData(TimingPoint*& testTp, TimingPoint*& refTp, TestArc*& arc)
   : testNode(testTp), refNode(refTp), testArcPtr(arc) {}
 TestData(TimingPoint*& testTp) : testNode(testTp) {
    refNode = NULL;
    testArcPtr = NULL;
    refValue[RISE] = refValue[FALL] = MAX_VALUE;
    cachedTestRAT[RISE] = cachedTestRAT[FALL] = MAX_VALUE;
  }

  TimingPoint* getRefNode() {
    return refNode;
  }

  TimingPoint* getTestNode() {
    return testNode;
  }
  double getDelay(int rf) {
    if(refNode) {
      double refSlope, testSlope;
      testSlope = testNode->getSlope(rf);
      string testType = testArcPtr->getTestType();
      if(testType.find("rising")!=string::npos)
	refSlope = refNode->getSlope(0);
      else
	refSlope = refNode->getSlope(1);

      if(rf==0)
	return testArcPtr->getRiseDelay(refSlope, testSlope);
      else
	return testArcPtr->getFallDelay(refSlope, testSlope);
    }
    else {
      return refValue[rf];
    }
  }

  double getTestRAT(int rf) {
    if(refNode) {
      string testType = testArcPtr->getTestType();
      if(testType=="setup_rising") {
	cachedTestRAT[rf] = (refNode->getArrival(RISE) - getDelay(rf));
      }
      else if(testType=="setup_falling") {
	cachedTestRAT[rf] = (refNode->getArrival(FALL) - getDelay(rf));
      }
      else {
	cout<<"Unknown test"<<endl;
	return 0;
      }
    }
    else
      cachedTestRAT[rf] = refValue[rf];

    return cachedTestRAT[rf];
  }

  double getTestRAT(int rf, bool& change) {
    if(refNode) {
      double newTestRAT;
      string testType = testArcPtr->getTestType();
      if(testType=="setup_rising") {
	newTestRAT = (refNode->getArrival(RISE) - getDelay(rf));
      }
      else if(testType=="setup_falling") {
	newTestRAT = (refNode->getArrival(FALL) - getDelay(rf));
      }
      else {
	cout<<"Unknown test"<<endl;
	return 0;
      }

      if(fabs(newTestRAT-cachedTestRAT[rf]) < 0.0001) {
	change = false;
      }
      else {
	cachedTestRAT[rf] = newTestRAT;
	change = true;
      }
    }
    else {
      if(fabs(cachedTestRAT[rf]-refValue[rf]) < 0.0001) {
	change = false;
      }
      else {
	change = true;
	cachedTestRAT[rf] = refValue[rf];
      }
    }

    return cachedTestRAT[rf];
  }

  string getTestType() {
    if(refNode)
      return testArcPtr->getTestType();
    else
      return "po_rat";
  }

  void setTestRAT(int rf, double value) {
    refValue[rf] = value;
  }
};
