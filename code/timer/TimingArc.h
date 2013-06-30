#include <iostream>
#include <string>

#include "TimingPoint.h"

using namespace std;

union ArcOwner {
  LibArc* theCellArc;
  Net* theNet;
};

class TimingArc {
  TimingPoint *sourceNode;
  TimingPoint *sinkNode;
  ArcOwner owner;
  bool cell_arc;
  double cachedDelay[2];

 public:
  TimingArc (TimingPoint *source, TimingPoint* sink, LibArc*& cellArc) : sourceNode(source), sinkNode(sink) {
    cell_arc = true;
    owner.theCellArc = cellArc;
    cachedDelay[RISE]=cachedDelay[FALL]=MIN_VALUE;
  }

  TimingArc (TimingPoint *source, TimingPoint* sink, Net*& net) : sourceNode(source), sinkNode(sink) {
    cell_arc = false;
    owner.theNet = net;
    cachedDelay[RISE]=cachedDelay[FALL]=MIN_VALUE;
  }

  TimingPoint* getToNode() {
    return sinkNode;
  }
  TimingPoint* getFromNode() {
    return sourceNode;
  }

  double getDelay(int rf, double inSlope) {

    if(cell_arc) {
      double outLoad = 0;
      LibArc* libArcPtr = owner.theCellArc;
      Net* loadNet = sinkNode->getPin()->getNet();
      if(loadNet!=NULL) { 
	vector<Pin*> sinkPins = loadNet->getSinkPins();
	LibPin* lPin;
	for(unsigned int i=0; i<sinkPins.size(); i++) {
	  lPin = sinkPins[i]->getLibPin();
	  if(lPin!=NULL) {
	    outLoad += lPin->getPincap();
	  }
	}
	//if(outLoad==0)
	//cout<<loadNet->getName()<<endl;
	//} else {
      //cout<<"No net found"<<endl;
      }
      if(rf==RISE)
	cachedDelay[rf] = libArcPtr->getCellRiseDelay(inSlope, outLoad);
      else
	cachedDelay[rf] = libArcPtr->getCellFallDelay(inSlope, outLoad);
    }
    else {
      cachedDelay[rf]=0;
    }

    return cachedDelay[rf];
  }

  double getDelay(int rf, double inSlope, bool& change) {
    double newDelay;
    if(cell_arc) {
      double outLoad = 0;
      LibArc* libArcPtr = owner.theCellArc;
      Net* loadNet = sinkNode->getPin()->getNet();
      if(loadNet!=NULL) {
	vector<Pin*> sinkPins = loadNet->getSinkPins();
	LibPin* lPin;
	for(unsigned int i=0; i<sinkPins.size(); i++) {
	  lPin = sinkPins[i]->getLibPin();
	  if(lPin!=NULL)
	    outLoad += lPin->getPincap();
	}
      }

      if(rf==RISE) {
	newDelay = libArcPtr->getCellRiseDelay(inSlope, outLoad);
      }
      else {
	newDelay = libArcPtr->getCellFallDelay(inSlope, outLoad);
      }
      if(fabs(cachedDelay[rf]-newDelay) < 0.0001) {
	change = false;
      }
      else {
	change = true;
	cachedDelay[rf] = newDelay;
      }
    }
    else {
      if(fabs(cachedDelay[rf]-0) < 0.0001) {
	change = false;
      }
      else {
	cachedDelay[rf]=0;
	change = true;
      }
    }

    return cachedDelay[rf];
  }

  double getOutSlope(int rf, double inSlope) {
    if(cell_arc) {
      double outLoad = 0;
      LibArc* libArcPtr = owner.theCellArc;
      Net* loadNet = sinkNode->getPin()->getNet();
      if(loadNet!=NULL) {
	vector<Pin*> sinkPins = loadNet->getSinkPins();
	LibPin* lPin;
	for(unsigned int i=0; i<sinkPins.size(); i++) {
	  lPin = sinkPins[i]->getLibPin();
	  if(lPin!=NULL)
	    outLoad += lPin->getPincap();
	}
      }

      if(rf==RISE)
	return libArcPtr->getRiseTransition(inSlope, outLoad);
      else
	return libArcPtr->getFallTransition(inSlope, outLoad);
    }
    else {
      return inSlope;
    }
  }
  string getTimingType() {
    if(cell_arc) {
      return owner.theCellArc->getTimingType();
    }
    else
      return "";
  }
  bool isCellArc() {
    return cell_arc;
  }
  double getDelay(int rf) {
    return cachedDelay[rf];
  }

  string getTimingSense() {
    if(!cell_arc)
      return "positive_unate";
    else
      return owner.theCellArc->getTimingSense();
  }

};
