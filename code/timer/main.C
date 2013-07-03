#include "Design.h"
#include "Iterator.h"
#include "TimingGraph.h"
#include <iostream>
#include <fstream>

Design* Design::curDesign=NULL;

extern FILE *yyin;
extern int yyparse();

int main() {

  //Read cells.rpt
  string cellName, libCellName, seqStr;
  bool seq;
  ifstream fin;
  fin.open("cells.rpt");
  fin >> cellName;
  Design* curDesign = new Design(cellName);
  Design::setCurDesign(curDesign);

  Cell* cell;
  LibCell* libCell;
  while (1) {
    fin >> cellName >> libCellName >> seqStr;
    if(fin.eof())
      break;
    if(cellName.find("Logic")!=string::npos) continue;
    if(seqStr.compare("true")==0)
      seq = true;
    else
      seq = false;

    libCell = curDesign->findLibCell(libCellName);
    if(!libCell) {
      libCell = new LibCell(libCellName);
      curDesign->add(libCell);
    }
    cell = new Cell(cellName, libCell, seq);
    curDesign->add(cell);
    //cout << cellName.c_str() << " " << libCellName.c_str() << " " << seq << endl;
  }
  fin.close();

  Design_CellIterator dc;
  dc.attach(curDesign);
  while( (cell=dc.next())!=NULL ) {
    //cout << cell->getName().c_str() << endl;
  }

  //Read pins.rpt
  string pinName, direction;
  bool isClock;
  string clockStr;
  Pin* pin;
  LibPin* lPin;
  fin.open("pins.rpt");
  size_t found; //string::npos is of type long
  while (1) {
    fin >> pinName >> direction >> clockStr;
    if(fin.eof())
      break;
    if(pinName.find("Logic")!=string::npos) continue;

    if(clockStr=="true")
      isClock = true;
    else
      isClock = false;

    found = pinName.find_last_of("/");
    //cout<<found<<" "<<string::npos<<endl;
    if(found==string::npos) {
      pin = new Pin(pinName,direction);
    }
    else {
      string libPinName = pinName.substr(found+1);
      cellName = pinName.substr(0,found);
      cell = curDesign->findCell(cellName);
      libCell = cell->getLibCell();
      pin = new Pin(pinName,direction,cell,isClock);
      lPin = libCell->findLibPin(libPinName);
      if(lPin==NULL) {
	lPin = new LibPin(libPinName,libCell);
	libCell->insertPin(lPin);
      }
      pin->setLibPin(lPin);
    }
    curDesign->add(pin);
    //cout<<cellName.c_str()<<" "<<pinName.c_str()<<endl;
  }
  fin.close();

  //  curDesign->checkDesign();

  Design_PinIterator dp;
  dp.attach(curDesign);
  while( (pin=dp.next()) != NULL ) {
    //cout<<pin->getCell()->getName().c_str()<<" "<<pin->getName().c_str()<<endl;
  }

  //Read nets
  fin.open("nets.rpt");
  string netName;
  bool skip;
  Net* net;
  int numPins;
  while (1) {
    skip = false;
    fin >> netName >> numPins;
    if(fin.eof())
      break;
    if(netName.find("Logic")!=string::npos) skip=true;
    if(!skip) {
      net = new Net(netName);
      curDesign->add(net);
    }

    while(numPins>0) {
      fin >> pinName;
      if(fin.eof())
	break;
      numPins--;
      if(skip) continue;
      pin = curDesign->findPin(pinName);
      if( (pin->getDirection()=="in" && pin->isPort()) || (pin->getDirection()=="out" && !pin->isPort()) ) {
	net->setSourcePin(pin);
	pin->setNet(net);
      }
      else {
	net->setSinkPin(pin);
	pin->setNet(net);
      }
    }
  }
  fin.close();

  Design_NetIterator dn;
  dn.attach(curDesign);
  while( (net=dn.next()) != NULL ) {
    //cout<<"Net: "<<net->getName()<<endl;
    //net->printPins();
  }


  FILE *f = fopen("saed90nm_typ.lib","r"); 
  yyin = f;
  yyparse();

  TimingGraph theTG;
  TimingPoint* tpPtr;
  theTG.createTimingGraph(curDesign);
  fin.open("RAT.rpt");
  int edge;
  double testValue;
  vector<TestData*> testsList;
  TestData* td;
  while(1) {
    fin >> pinName >> edge >> testValue;
    if(fin.eof())
      break;
    tpPtr = theTG.findTimingPoint(pinName);
    testsList = tpPtr->getTests();
    if(testsList.size()==0) {
      td = new TestData(tpPtr);
      tpPtr->insertTest(td);
    }
    else
      td = testsList[0];
    td->setTestRAT(edge,testValue);
  }
  fin.close();
  theTG.levelizeGraph();

  double clkPeriod, rEdge, fEdge;
  fin.open("clocks.rpt");
  fin >> pinName >> rEdge >> fEdge >> clkPeriod;
  tpPtr = theTG.findTimingPoint(pinName);
  tpPtr->setHasClock();
  theTG.setClockWaveform(rEdge,fEdge,clkPeriod);
  fin.close();

  theTG.analyzeTiming();
  //theTG.updateTiming();

  return 0;
}
