#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <math.h>
//#include <list>

#include "Path.h"
#include "TimingArc.h"
//#include "TimingPoint.h"
#include "TestData.h"

using namespace std;

class Design;
class Iterator;
class Pin;
class Cell;
class Net;
class LibCell;
class LibPin;
class LibArc;

class ComparatorAT {
 public:
    bool operator()(TimingPoint*& tp1, TimingPoint*& tp2) {
      return (tp1->getLevel()>tp2->getLevel());
    }
};
class ComparatorRAT {
 public:
    bool operator()(TimingPoint*& tp1, TimingPoint*& tp2) {
      return (tp1->getLevel()<tp2->getLevel());
    }
};
class ComparatorSlack {
 public:
    bool operator()(TimingPoint*& tp1, TimingPoint*& tp2) {
      int rf1, rf2;
      return (tp1->getWorstSlack(rf1)>tp2->getWorstSlack(rf2));
    }
};

typedef priority_queue<TimingPoint*,vector<TimingPoint*>,ComparatorAT> PQueue;
typedef priority_queue<TimingPoint*,vector<TimingPoint*>,ComparatorRAT> MaxPQueue;
typedef priority_queue<TimingPoint*,vector<TimingPoint*>,ComparatorSlack> SlackHeap;

class TimingGraph {
  vector<TimingPoint*> inputNodes;
  vector<TimingPoint*> endPoints;
  map<string,TimingPoint*> graphNodes;
  double leading_edge;
  double trailing_edge;
  double clk_period;
  PQueue TimingQ;
  MaxPQueue RatQ;
  SlackHeap sortedEndPoints;
  vector<Path*> timingPaths;

 public:
  void createTimingGraph(Design*& design) {
    TimingPoint *tp;
    Design_PortIterator d_pt;
    Design_PinIterator d_pn;
    Pin* pinPtr;

    d_pt.attach(design);
    while((pinPtr=d_pt.next())!=NULL) {
       tp = new TimingPoint(pinPtr);
      if(pinPtr->getDirection()=="in")
	inputNodes.push_back(tp);
      else {
	endPoints.push_back(tp);
	tp->setAsEndPoint();
      }
      graphNodes.insert(make_pair(pinPtr->getName(),tp));
    }

    d_pn.attach(design);
    while((pinPtr=d_pn.next())!=NULL) {
      //cout<<pinPtr->getLibPin()->getPincap()<<endl;
      //pinPtr->getLibPin()->getLibCell()->checkLibPins();
      tp = new TimingPoint(pinPtr);
      graphNodes.insert(make_pair(pinPtr->getName(),tp));
    }

    Design_CellIterator d_c;
    Cell *cellPtr;
    LibCell *libCellPtr;
    string cellName;
    vector<LibArc*> libArcs;
    vector<TestArc*> testArcsList;

    d_c.attach(design);
    TimingArc *tArc;
    TestData * td;
    map<string,TimingPoint*>::iterator it;
    TimingPoint *tp1=NULL, *tp2=NULL;
    string pinName1, pinName2;

    while((cellPtr=d_c.next())!=NULL) {
      cellName = cellPtr->getName();
      cellName += "/";
      libCellPtr = cellPtr->getLibCell();
      //cout<<cellName<<endl;
      //libCellPtr->checkLibPins();

      libArcs = libCellPtr->getLibArcs();
      LibArc* arc;
      for(unsigned int i=0; i<libArcs.size(); i++) {
	arc = libArcs[i];
	//cout<<"Arc: "<<cellName<<" "<<arc->getFromPin()->getName()<<" "<<arc->getToPin()->getName()<<" "<<arc->getTimingSense()<<endl;
	pinName1 = pinName2 = cellName;
	pinName1 += arc->getFromPin()->getName();
	pinName2 += arc->getToPin()->getName();
	//cout<< pinName1<<" "<<pinName2<<endl;

	it = graphNodes.find(pinName1);
	if(it!=graphNodes.end())
	  tp1 = it->second;
	else {
	  tp1 = NULL;
	  cout<<"No such timing point found: "<<pinName1.c_str()<<endl;
	}

	it = graphNodes.find(pinName2);
	if(it!=graphNodes.end())
	  tp2 = it->second;
	else {
	  tp2 = NULL;
	  cout<<"No such timing point found: "<<pinName2.c_str()<<endl;
	}

	if(tp1!=NULL && tp2!=NULL) {
	  tArc = new TimingArc(tp1,tp2,arc);
	  tp1->insertFromArc(tArc);
	  tp2->insertToArc(tArc);
	}
      }

      testArcsList = libCellPtr->getTestArcs();
      TestArc* testArc;
      for(unsigned int i=0; i<testArcsList.size(); i++) {
	testArc = testArcsList[i];
	//cout<<"Test arc: "<<cellName<<" "<<testArc->getFromPin()->getName()<<" "<<testArc->getToPin()->getName()<<" "<<testArc->getTestType()<<endl;
	string ttype = testArc->getTestType();
	if(!(ttype=="setup_rising"|| ttype=="setup_falling"))
	  continue;
	pinName1 = pinName2 = cellName;
	pinName1 += testArc->getFromPin()->getName();
	pinName2 += testArc->getToPin()->getName();
	//cout<< pinName1<<" "<<pinName2<<endl;

	it = graphNodes.find(pinName1);
	if(it!=graphNodes.end())
	  tp1 = it->second;
	else {
	  tp1 = NULL;
	  cout<<"No such timing point found: "<<pinName1.c_str()<<endl;
	}

	it = graphNodes.find(pinName2);
	if(it!=graphNodes.end())
	  tp2 = it->second;
	else {
	  tp2 = NULL;
	  cout<<"No such timing point found: "<<pinName2.c_str()<<endl;
	}

	if(tp1!=NULL && tp2!=NULL) {
	  td = new TestData(tp2,tp1,testArc);
	  tp2->insertTest(td);
	  tp1->insertTest(td);//Do not mark as endpoint, this is reference node
	  endPoints.push_back(tp2);
	  tp2->setAsEndPoint();
	  //cout<<"Test arc: "<<testArc->getFromPin()->getName()<<" "<<testArc->getToPin()->getName()<<" "<<td->getTestType()<<endl;
	}
      }
    }//Done..cell arcs


    Design_NetIterator d_n;
    Net* netPtr;
    Pin* pinPtr2;
    vector<Pin*> netPins;
    d_n.attach(design);
    while((netPtr=d_n.next())!=NULL) {
      pinPtr = netPtr->getSourcePin();
      netPins = netPtr->getSinkPins();
      it = graphNodes.find(pinPtr->getName());
      if(it!=graphNodes.end())
	tp1 = it->second;
      else {
	tp1 = NULL;
	cout<<"No such timing point found"<<endl;
      }
      for(unsigned int i=0; i<netPins.size(); i++) {
	pinPtr2 = netPins[i];
	it = graphNodes.find(pinPtr2->getName());
	if(it!=graphNodes.end())
	  tp2 = it->second;
	else {
	  tp2 = NULL;
	  cout<<"No such timing point found"<<endl;
	}

	if(tp1!=NULL && tp2!=NULL) {
	  tArc = new TimingArc(tp1,tp2,netPtr);
	  tp1->insertFromArc(tArc);
	  tp2->insertToArc(tArc);
	}
      }
    }

  }

  TimingPoint* findTimingPoint(string name) {
    map<string,TimingPoint*>::iterator it;
    it = graphNodes.find(name);
    if(it!=graphNodes.end())
      return it->second;
    else
      return NULL;
  }

  /*
  void calcLevelRecursive(TimingPoint* tp) {
    int curLevel = tp->getLevel();
    curLevel++;
    TimingPoint *node;
    vector<TimingArc*> fromArcs = tp->getFromArcs();
    for(unsigned int i=0; i<fromArcs.size(); i++) {
      node = fromArcs[i]->getToNode();
      if(node->getLevel()<curLevel) {
	node->setLevel(curLevel);
	cout<<node->getPin()->getName()<<" "<<node->getLevel()<<endl;
	calcLevelRecursive(node);
      }
    }
  }
*/

  void calcLevelRecursive2(TimingPoint* tp, int prevLevel) {

    TimingPoint *node;
    vector<TimingArc*> theArcs = tp->getToArcs();
    int maxLevel=prevLevel;
    for(unsigned int i=0; i<theArcs.size(); i++) {
      node = theArcs[i]->getFromNode();
      int nodeLevel = node->getLevel();
      if(nodeLevel==-2) {
	cout<<"Cycle found in timing graph"<<endl;
      }
      else if(nodeLevel==-1) {
	tp->setLevel(-2); //Temporary marking
	calcLevelRecursive2(node,-1);
	nodeLevel=node->getLevel();
	if(nodeLevel>maxLevel)
	  maxLevel = nodeLevel;
      }
      else {
	if(nodeLevel>maxLevel)
	  maxLevel = nodeLevel;
      }
    }
    tp->setLevel(++maxLevel);
    //TimingQ.push(tp);
    //cout<<tp->getPin()->getName()<<"   "<<tp->getLevel()<<endl;
  }

  void calcLevelRecursive(TimingPoint* tp, int prevLevel) {

    TimingPoint *node;
    vector<TimingArc*> theArcs;
    if(tp->getLevel()<0) {  //Have to fix the need to start from all input nodes
      theArcs = tp->getToArcs();
      int maxLevel=prevLevel;
      for(unsigned int i=0; i<theArcs.size(); i++) {
	node = theArcs[i]->getFromNode();
	int nodeLevel = node->getLevel();
	if(nodeLevel==-1) {
	  tp->setLevel(-2); //Temporary marking
	  calcLevelRecursive2(node,-1);
	  nodeLevel=node->getLevel();
	  if(nodeLevel>maxLevel)
	    maxLevel = nodeLevel;
	}
	else {
	  if(nodeLevel>maxLevel)
	    maxLevel = nodeLevel;
	}
      }
      tp->setLevel(++maxLevel);
      //TimingQ.push(tp);
      //cout<<tp->getPin()->getName()<<"   "<<tp->getLevel()<<endl;
    }

    theArcs = tp->getFromArcs();
    for(unsigned int i=0; i<theArcs.size(); i++) {
      node = theArcs[i]->getToNode();
      calcLevelRecursive(node,tp->getLevel());
    }
  }

  void setClockWaveform(double lEdge, double tEdge, double period) {
    clk_period = period;
    leading_edge = lEdge;
    trailing_edge = tEdge;
    //cout<< clk_period<<endl;
  }

  void propagateClockFlag() {

  }

  /*
  void levelizeGraph2() {
    TimingPoint* tp;
    for(unsigned int i=0; i<inputNodes.size(); i++) {
      tp = inputNodes[i];
      tp->setLevel(0);
      calcLevelRecursive(tp);
    }
    //Check
    for(unsigned int i=0; i<outputNodes.size(); i++) {
      tp = outputNodes[i];
      cout<<tp->getLevel()<<endl;
    }
  }
  */

  void levelizeGraph() {
    TimingPoint* tp;
    for(unsigned int i=0; i<inputNodes.size(); i++) {
      tp = inputNodes[i];
      calcLevelRecursive(tp,-1);
    }
    //Check
    for(unsigned int i=0; i<endPoints.size(); i++) {
      tp = endPoints[i];
      //cout<<tp->getLevel()<<endl;
    }
    /* while(!TimingQ.empty()) {
      tp = TimingQ.top();
      TimingQ.pop();
      cout<<tp->getPin()->getName()<<" "<<tp->getLevel()<<endl;
      } */
  }


  void analyzeTiming() {

    map<string,TimingPoint*>::iterator it;
    for(it=graphNodes.begin(); it!=graphNodes.end(); it++) {
      TimingQ.push(it->second);
      RatQ.push(it->second);
    }

    calcArrival();
    calcRequired();
    sortEndPointsBySlack();
    printPathsForPlot(100);
  }

  void calcArrival() {
    TimingPoint* tp;
    vector<TimingArc*> arcs;
    double atR, atF, slopeR, slopeF, tmpSlope, tmpAt;
    while(!TimingQ.empty()) {
      tp = TimingQ.top();
      //cout<<tp->getPin()->getName()<<endl;
      TimingQ.pop();
      atR = atF = MIN_VALUE;
      slopeR = slopeF = MIN_VALUE;
      arcs = tp->getToArcs();
      if(arcs.size()==0) {
	if(tp->getPin()->isPort() && tp->getPin()->getDirection()=="in") {
	  //Default AT and Slope at input ports
	  atR = atF = 0;
	  slopeR = slopeF = 0.1;
	}
	else {
	  //cout<<"Dangling input found: "<<tp->getPin()->getName()<<endl;
	}
      }
      TimingPoint* tp2;
      TimingArc* tarc;
      for(unsigned int i=0; i<arcs.size(); i++) {
	tarc = arcs[i];
	tp2 = tarc->getFromNode();
	//cout<<"   "<<tp2->getPin()->getName()<<endl;
	if(tarc->getTimingSense()=="positive_unate") {
	  tmpAt = tp2->getArrival(RISE) + tarc->getDelay(RISE,tp2->getSlope(RISE));
	  if(tmpAt>atR) {
	    atR = tmpAt;
	    tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(RISE));
	    slopeR = tmpSlope;
	  }

	  tmpAt = tp2->getArrival(FALL) + tarc->getDelay(FALL,tp2->getSlope(FALL));
	  if(tmpAt>atF) {
	    atF = tmpAt;
	    tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(FALL));
	    slopeF = tmpSlope;
	  }
	}
	else if(tarc->getTimingSense()=="negative_unate") {
	  tmpAt = tp2->getArrival(FALL) + tarc->getDelay(RISE,tp2->getSlope(FALL));
	  if(tmpAt>atR) {
	    atR = tmpAt;
	    tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(FALL));
	    slopeR = tmpSlope;
	  }

	  tmpAt = tp2->getArrival(RISE) + tarc->getDelay(FALL,tp2->getSlope(RISE));
	  if(tmpAt>atF) {
	    atF = tmpAt;
	    tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(RISE));
	    slopeF = tmpSlope;
	  }
	} else if(tarc->getTimingSense()=="non_unate") {

	  if(tarc->getTimingType()!="rising_edge") {
	    tmpAt = tp2->getArrival(FALL) + tarc->getDelay(RISE,tp2->getSlope(FALL));
	    if(tmpAt>atR) {
	      atR = tmpAt;
	      tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(FALL));
	      slopeR = tmpSlope;
	    }

	    tmpAt = tp2->getArrival(FALL) + tarc->getDelay(FALL,tp2->getSlope(FALL));
	    if(tmpAt>atF) {
	      atF = tmpAt;
	      tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(FALL));
	      slopeF = tmpSlope;
	    }
	  }
	  if(tarc->getTimingType()!="falling_edge") {
	    tmpAt = tp2->getArrival(RISE) + tarc->getDelay(RISE,tp2->getSlope(RISE));
	    if(tmpAt>atR) {
	      atR = tmpAt;
	      tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(RISE));
	      slopeR = tmpSlope;
	    }
	    tmpAt = tp2->getArrival(RISE) + tarc->getDelay(FALL,tp2->getSlope(RISE));
	    if(tmpAt>atF) {
	      atF = tmpAt;
	      tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(RISE));
	      slopeF = tmpSlope;
	    }
	  }
	}
	else {
	  cout<<"Unkown timing sense for arc"<<endl;
	}
      }

      tp->setArrival(RISE,atR);
      tp->setArrival(FALL,atF);
      tp->setSlope(RISE,slopeR);
      tp->setSlope(FALL,slopeF);
      tp->setATValid();

      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(1)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlope(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlope(1)<<endl; */
      /* cout<<endl; */
    }
  }


  void calcRequired() {
    TimingPoint* tp;
    vector<TimingArc*> arcs;
    double ratR, ratF, tmpRat;

    while(!RatQ.empty()) {
      tp = RatQ.top();
      //cout<<tp->getPin()->getName()<<endl;
      RatQ.pop();
      ratR = ratF = MAX_VALUE;
      arcs = tp->getFromArcs();
      if(arcs.size()==0 && !tp->isEndPoint()) {
	//cout<<"Dangling output found: "<<tp->getPin()->getName()<<endl;
      }

      TimingPoint* tp2;
      TimingArc* tarc;
      for(unsigned int i=0; i<arcs.size(); i++) {
	tarc = arcs[i];
	tp2 = tarc->getToNode();

	if(tarc->getTimingSense()=="positive_unate") {
	  tmpRat = tp2->getRequired(RISE) - tarc->getDelay(RISE);
	  if(tmpRat<ratR)
	    ratR = tmpRat;
	  tmpRat = tp2->getRequired(FALL) - tarc->getDelay(FALL);
	  if(tmpRat<ratF)
	    ratF = tmpRat;
	}
	else if(tarc->getTimingSense()=="negative_unate") {
	  tmpRat = tp2->getRequired(FALL) - tarc->getDelay(FALL);
	  if(tmpRat<ratR)
	    ratR = tmpRat;
	  tmpRat = tp2->getRequired(RISE) - tarc->getDelay(RISE);
	  if(tmpRat<ratF)
	    ratF = tmpRat;
	} else if(tarc->getTimingSense()=="non_unate") {
	  double tmpRat2;
	  tmpRat = tp2->getRequired(FALL) - tarc->getDelay(FALL);
	  tmpRat2 = tp2->getRequired(RISE) - tarc->getDelay(RISE);
	  if(tmpRat2<tmpRat)
	    tmpRat = tmpRat2;
	  if(tarc->getTimingType()!="rising_edge") {
	    if(tmpRat<ratF)
	      ratF = tmpRat;
	  }
	  if(tarc->getTimingType()!="falling_edge") {
	    if(tmpRat<ratR)
	      ratR = tmpRat;
	  }
	}
	else {
	  cout<<"Unkown timing sense for arc"<<endl;
	}
      }

      double slackR, slackF;
      vector<TestData*> testsList;
      if(tp->isEndPoint()) { //Only for actual test nodes, not reference nodes
	testsList = tp->getTests();
      }
      TestData* td;

      for(unsigned int i=0; i<testsList.size(); i++) {
      	if(i>0)
      	  cout<<"More than one test found"<<endl;
      	td = testsList[i];
      	ratR = td->getTestRAT(RISE);
      	ratF = td->getTestRAT(FALL);
      	string testType=td->getTestType();
      	if(testType.find("setup_rising")!=string::npos) {
      	  ratR += (clk_period-leading_edge);
      	  ratF += (clk_period-leading_edge);
      	}
	else if(testType.find("setup_falling")!=string::npos) {
	  ratR += trailing_edge;
	  ratF += trailing_edge;
	}
	//cout<<ratR<<" "<<ratF<<endl;
      }


      tp->setRequired(RISE,ratR);
      tp->setRequired(FALL,ratF);
      tp->setRATValid();
      slackR = tp->getRequired(RISE)-tp->getArrival(RISE);
      slackF = tp->getRequired(FALL)-tp->getArrival(FALL);
      tp->setSlack(RISE,slackR);
      tp->setSlack(FALL,slackF);

      /* cout<<tp->getPin()->getName()<<" "<<tp->getRequired(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getRequired(1)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(1)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlack(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlack(1)<<endl; */
      /* cout<<endl; */

    }
  }

  void initializeTimingQ() {
    for(unsigned i=0; i<inputNodes.size(); i++)
      TimingQ.push(inputNodes[i]);
  }


  void populateRatQ() {

    /* bool anyChange; */
    /* bool hasChanged; */
    /* vector<TestData*> tmpTests; */
    /* TestData* tmpTd; */
    /* double tmpVal; */
    /* TimingPoint* tp; */
    /* for(unsigned i=0; i<endPoints.size(); i++) { */
    /*   anyChange = false; */
    /*   tp = endPoints[i]; */
    /*   tmpTests = tp->getTests(); */
    /*   for(unsigned j=0; i<tmpTests.size(); i++) { */
    /* 	tmpTd = tmpTests[j]; */
    /* 	tmpVal = tmpTd->getTestRAT(RISE, hasChanged); */
    /* 	anyChange = anyChange || hasChanged; */
    /* 	tmpVal = tmpTd->getTestRAT(FALL, hasChanged); */
    /* 	anyChange = anyChange || hasChanged; */
    /*   } */
    /*   if(anyChange) { */
    /* 	tp->invalidateRAT(); */
    /* 	RatQ.push(tp); */
    /*   } */
    /* } */

    TimingPoint* tp;
    for(unsigned i=0; i<endPoints.size(); i++) {
      tp = endPoints[i];
      if(tp->getPin()->isPort()) {
	tp->invalidateRAT();
	RatQ.push(tp);
      }
    }

  }

  void calcArrivalIncr() {
    TimingPoint* tp;
    vector<TimingArc*> arcs;
    double atR, atF, slopeR, slopeF, tmpSlope, tmpAt;
    while(!TimingQ.empty()) {
      tp = TimingQ.top();
      //cout<<tp->getPin()->getName()<<endl;
      TimingQ.pop();
      if(tp->isATValid())
	continue;
      atR = atF = MIN_VALUE;
      slopeR = slopeF = MIN_VALUE;
      arcs = tp->getToArcs();
      if(arcs.size()==0) {
	if(tp->getPin()->isPort() && tp->getPin()->getDirection()=="in") {
	  //Default AT and Slope at input ports
	  atR = atF = 0;
	  slopeR = slopeF = 0.1;
	}
	else
	  cout<<"Dangling input found: "<<tp->getPin()->getName()<<endl;
      }
      TimingPoint* tp2;
      TimingArc* tarc;
      bool hasChanged;
      bool anyChange;
      for(unsigned int i=0; i<arcs.size(); i++) {
	tarc = arcs[i];
	tp2 = tarc->getFromNode();
	anyChange = false;
	//cout<<"   "<<tp2->getPin()->getName()<<endl;
	if(tarc->getTimingSense()=="positive_unate") {
	  tmpAt = tp2->getArrival(RISE) + tarc->getDelay(RISE,tp2->getSlope(RISE),hasChanged);
	  if(tmpAt>atR)
	    atR = tmpAt;
	  anyChange = anyChange || hasChanged;
	  tmpAt = tp2->getArrival(FALL) + tarc->getDelay(FALL,tp2->getSlope(FALL),hasChanged);
	  if(tmpAt>atF)
	    atF = tmpAt;
	  anyChange = anyChange || hasChanged;
	  tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(RISE));
	  if(tmpSlope>slopeR)
	    slopeR = tmpSlope;
	  tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(FALL));
	  if(tmpSlope>slopeF)
	    slopeF = tmpSlope;
	}
	else if(tarc->getTimingSense()=="negative_unate") {
	  tmpAt = tp2->getArrival(FALL) + tarc->getDelay(RISE,tp2->getSlope(FALL),hasChanged);
	  if(tmpAt>atR)
	    atR = tmpAt;
	  anyChange = anyChange || hasChanged;
	  tmpAt = tp2->getArrival(RISE) + tarc->getDelay(FALL,tp2->getSlope(RISE),hasChanged);
	  if(tmpAt>atF)
	    atF = tmpAt;
	  anyChange = anyChange || hasChanged;
	  tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(FALL));
	  if(tmpSlope>slopeR)
	    slopeR = tmpSlope;
	  tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(RISE));
	  if(tmpSlope>slopeF)
	    slopeF = tmpSlope;
	} else if(tarc->getTimingSense()=="non_unate") {

	  if(tarc->getTimingType()!="rising_edge") {
	    tmpAt = tp2->getArrival(FALL) + tarc->getDelay(RISE,tp2->getSlope(FALL),hasChanged);
	    if(tmpAt>atR)
	      atR = tmpAt;
	    anyChange = anyChange || hasChanged;
	    tmpAt = tp2->getArrival(FALL) + tarc->getDelay(FALL,tp2->getSlope(FALL),hasChanged);
	    if(tmpAt>atF)
	      atF = tmpAt;
	    anyChange = anyChange || hasChanged;
	    tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(FALL));
	    if(tmpSlope>slopeR)
	      slopeR = tmpSlope;
	    tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(FALL));
	    if(tmpSlope>slopeF)
	      slopeF = tmpSlope;
	  }
	  if(tarc->getTimingType()!="falling_edge") {
	    tmpAt = tp2->getArrival(RISE) + tarc->getDelay(RISE,tp2->getSlope(RISE),hasChanged);
	    if(tmpAt>atR)
	      atR = tmpAt;
	    anyChange = anyChange || hasChanged;
	    tmpAt = tp2->getArrival(RISE) + tarc->getDelay(FALL,tp2->getSlope(RISE),hasChanged);
	    if(tmpAt>atF)
	      atF = tmpAt;
	    anyChange = anyChange || hasChanged;
	    tmpSlope = tarc->getOutSlope(RISE,tp2->getSlope(RISE));
	    if(tmpSlope>slopeR)
	      slopeR = tmpSlope;
	    tmpSlope = tarc->getOutSlope(FALL,tp2->getSlope(RISE));
	    if(tmpSlope>slopeF)
	      slopeF = tmpSlope;
	  }
	}
	else {
	  cout<<"Unkown timing sense for arc"<<endl;
	}
	if(anyChange) {
	  tp2->invalidateRAT();
	  RatQ.push(tp2);
	}
      }

      if(fabs(tp->getArrival(RISE)-atR) < 0.0001
	 && fabs(tp->getArrival(FALL)-atF) < 0.0001
	 && fabs(tp->getSlope(RISE)-slopeR) < 0.0001
	 && fabs(tp->getSlope(FALL)-slopeF) < 0.0001) {
	tp->setATValid();
      } else {
	tp->setArrival(RISE,atR);
	tp->setArrival(FALL,atF);
	tp->setSlope(RISE,slopeR);
	tp->setSlope(FALL,slopeF);
	tp->setATValid();
	TimingPoint* tp2;
	vector<TimingArc*> tarcs = tp->getFromArcs();
	for(unsigned i=0; i<tarcs.size(); i++) {
	  tp2 = tarcs[i]->getToNode();
	  tp2->invalidateAT();
	  TimingQ.push(tp2);
	}
	if(tp->getPin()->isClockPin()) {
	  vector<TestData*> tests = tp->getTests();
	  for(unsigned i=0; i<tests.size(); i++) {
	    tp2 = tests[i]->getTestNode();
	    tp2->invalidateRAT();
	    RatQ.push(tp2);
	  }
	}
      }
       
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(1)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlope(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlope(1)<<endl; */
      /* cout<<endl; */
    }
  }

  void calcRequiredIncr() {
    TimingPoint* tp;
    vector<TimingArc*> arcs;
    double ratR, ratF, tmpRat;

    while(!RatQ.empty()) {
      tp = RatQ.top();
      //cout<<tp->getPin()->getName()<<endl;
      RatQ.pop();
      if(tp->isRATValid())
	continue;
      ratR = ratF = MAX_VALUE;
      arcs = tp->getFromArcs();
      if(arcs.size()==0 && !tp->isEndPoint()) {
	cout<<"Dangling output found: "<<tp->getPin()->getName()<<endl;
      }

      TimingPoint* tp2;
      TimingArc* tarc;
      for(unsigned int i=0; i<arcs.size(); i++) {
	tarc = arcs[i];
	tp2 = tarc->getToNode();

	if(tarc->getTimingSense()=="positive_unate") {
	  tmpRat = tp2->getRequired(RISE) - tarc->getDelay(RISE);
	  if(tmpRat<ratR)
	    ratR = tmpRat;
	  tmpRat = tp2->getRequired(FALL) - tarc->getDelay(FALL);
	  if(tmpRat<ratF)
	    ratF = tmpRat;
	}
	else if(tarc->getTimingSense()=="negative_unate") {
	  tmpRat = tp2->getRequired(FALL) - tarc->getDelay(FALL);
	  if(tmpRat<ratR)
	    ratR = tmpRat;
	  tmpRat = tp2->getRequired(RISE) - tarc->getDelay(RISE);
	  if(tmpRat<ratF)
	    ratF = tmpRat;
	} else if(tarc->getTimingSense()=="non_unate") {
	  double tmpRat2;
	  tmpRat = tp2->getRequired(FALL) - tarc->getDelay(FALL);
	  tmpRat2 = tp2->getRequired(RISE) - tarc->getDelay(RISE);
	  if(tmpRat2<tmpRat)
	    tmpRat = tmpRat2;
	  if(tarc->getTimingType()!="rising_edge") {
	    if(tmpRat<ratF)
	      ratF = tmpRat;
	  }
	  if(tarc->getTimingType()!="falling_edge") {
	    if(tmpRat<ratR)
	      ratR = tmpRat;
	  }
	}
	else {
	  cout<<"Unkown timing sense for arc"<<endl;
	}
      }

      double slackR, slackF;
      vector<TestData*> testsList;
      if(tp->isEndPoint()) { //Only for actual test nodes, not reference nodes
	testsList = tp->getTests();
      }
      TestData* td;

      for(unsigned int i=0; i<testsList.size(); i++) {
      	if(i>0)
      	  cout<<"More than one test found"<<endl;
      	td = testsList[i];
      	ratR = td->getTestRAT(RISE);
      	ratF = td->getTestRAT(FALL);
      	string testType=td->getTestType();
      	if(testType.find("setup")!=string::npos) {
      	  ratR += clk_period;
      	  ratF += clk_period;
      	}
	//cout<<ratR<<" "<<ratF<<endl;
      }


      tp->setRequired(RISE,ratR);
      tp->setRequired(FALL,ratF);
      tp->setRATValid();
      slackR = tp->getRequired(RISE)-tp->getArrival(RISE);
      slackF = tp->getRequired(FALL)-tp->getArrival(FALL);
      tp->setSlack(RISE,slackR);
      tp->setSlack(FALL,slackF);

      vector<TimingArc*> tarcs = tp->getToArcs();
      for(unsigned i=0; i<tarcs.size(); i++) {
	tp2 = tarcs[i]->getFromNode();
	  tp2->invalidateRAT();
	  RatQ.push(tp2);
      }

      /* cout<<tp->getPin()->getName()<<" "<<tp->getRequired(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getRequired(1)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getArrival(1)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlack(0)<<endl; */
      /* cout<<tp->getPin()->getName()<<" "<<tp->getSlack(1)<<endl; */
      /* cout<<endl; */

    }
  }

  void updateTiming() {
    initializeTimingQ();
    calcArrivalIncr();
    populateRatQ();
    calcRequiredIncr();
    sortEndPointsBySlack();
  }

  void sortEndPointsBySlack(bool traceFullPath=false) {
    TimingPoint* endpt;
    for(unsigned int i=0; i<endPoints.size(); i++) {
      endpt = endPoints[i];
      sortedEndPoints.push(endpt);
    }
    int counter=0;
    Path* tempPath;
    PathElem* tempPathElem;
    while(!sortedEndPoints.empty()) {
      counter++;
      //cout<<"Path "<<counter<<endl;
      endpt = sortedEndPoints.top();
      sortedEndPoints.pop();
      int edge;
      double worstSlack=endpt->getWorstSlack(edge);
      tempPath = new Path();
      timingPaths.push_back(tempPath);
      tempPathElem = new PathElem(endpt,edge);
      tempPath->insert(tempPathElem);
      //cout<<endpt->getPin()->getName()<<" "<<worstSlack<<" "<<edge<<"\t"<<endpt->getArrival(edge)<<"\t"<<endpt->getSlope(edge)<<endl;

      TimingPoint* tmpTp;
      TimingArc* tmpArc;
      vector<TimingArc*> tarcs = endpt->getToArcs();
      for(unsigned int i=0; i<tarcs.size(); i++) {

	tmpArc = tarcs[i];
	tmpTp = tmpArc->getFromNode();
	string tSense = tmpArc->getTimingSense();

	if(tSense == "positive_unate") {
	  if( fabs(tmpTp->getArrival(edge)+tmpArc->getDelay(edge)-endpt->getArrival(edge)) < 0.0001 ) {
	    //if( fabs(tmpTp->getSlack(edge)-endpt->getSlack(edge)) < 0.0001 ) {
	    //cout<<tmpTp->getPin()->getName()<<" "<<tmpTp->getSlack(edge)<<" "<<edge<<"\t"<<tmpTp->getArrival(edge)<<"\t"<<tmpTp->getSlope(edge)<<endl;
	    endpt = tmpTp;
	    tempPathElem = new PathElem(endpt,edge);
	    tempPath->insert(tempPathElem);
	    tarcs = endpt->getToArcs();
	    i=-1;
	  }
	}
	else if(tSense == "negative_unate") {
	  if( fabs(tmpTp->getArrival(1-edge)+tmpArc->getDelay(edge)-endpt->getArrival(edge)) < 0.0001 ) {
	    //if( fabs(tmpTp->getSlack(1-edge)-endpt->getSlack(edge)) < 0.0001 ) {
	    edge = 1-edge;
	    //cout<<tmpTp->getPin()->getName()<<" "<<tmpTp->getSlack(edge)<<" "<<edge<<"\t"<<tmpTp->getArrival(edge)<<"\t"<<tmpTp->getSlope(edge)<<endl;
	    endpt = tmpTp;
	    tempPathElem = new PathElem(endpt,edge);
	    tempPath->insert(tempPathElem);
	    tarcs = endpt->getToArcs();
	    i=-1;
	  }
	}
	else {
	  if(tmpArc->getTimingType()=="rising_edge") {
	    if( fabs(tmpTp->getArrival(RISE)+tmpArc->getDelay(edge)-endpt->getArrival(edge)) < 0.0001 ) {
	      edge = RISE;
	      //cout<<tmpTp->getPin()->getName()<<" "<<tmpTp->getSlack(edge)<<" "<<edge<<"\t"<<tmpTp->getArrival(edge)<<"\t"<<tmpTp->getSlope(edge)<<endl;
	      endpt = tmpTp;
	      tempPathElem = new PathElem(endpt,edge);
	      tempPath->insert(tempPathElem);
	      tarcs = endpt->getToArcs();
	      if(!traceFullPath)
		break;

	      i=-1;
	    }
	  }
	  else if(tmpArc->getTimingType()=="falling_edge") {
	    if( fabs(tmpTp->getArrival(FALL)+tmpArc->getDelay(edge)-endpt->getArrival(edge)) < 0.0001 ) {
	      edge = FALL;
	      //cout<<tmpTp->getPin()->getName()<<" "<<tmpTp->getSlack(edge)<<" "<<edge<<"\t"<<tmpTp->getArrival(edge)<<"\t"<<tmpTp->getSlope(edge)<<endl;
	      endpt = tmpTp;
	      tempPathElem = new PathElem(endpt,edge);
	      tempPath->insert(tempPathElem);
	      tarcs = endpt->getToArcs();

	      if(!traceFullPath)
		break;

	      i=-1;
	    }
	  }
	  else {
	    if( fabs(tmpTp->getArrival(edge)+tmpArc->getDelay(edge)-endpt->getArrival(edge)) < 0.0001 ) {
	      //if( fabs(tmpTp->getSlack(edge)-endpt->getSlack(edge)) < 0.0001 ) {
	      //cout<<tmpTp->getPin()->getName()<<" "<<tmpTp->getSlack(edge)<<" "<<edge<<"\t"<<tmpTp->getArrival(edge)<<"\t"<<tmpTp->getSlope(edge)<<endl;
	      endpt = tmpTp;
	      tempPathElem = new PathElem(endpt,edge);
	      tempPath->insert(tempPathElem);
	      tarcs = endpt->getToArcs();
	      i=-1;
	    }
	    else if( fabs(tmpTp->getArrival(1-edge)+tmpArc->getDelay(edge)-endpt->getArrival(edge)) < 0.0001 ) {
	      //else if( fabs(tmpTp->getSlack(1-edge)-endpt->getSlack(edge)) < 0.0001 ) {
	      edge = 1-edge;
	      //cout<<tmpTp->getPin()->getName()<<" "<<tmpTp->getSlack(edge)<<" "<<edge<<"\t"<<tmpTp->getArrival(edge)<<"\t"<<tmpTp->getSlope(edge)<<endl;
	      endpt = tmpTp;
	      tempPathElem = new PathElem(endpt,edge);
	      tempPath->insert(tempPathElem);
	      tarcs = endpt->getToArcs();
	      i=-1;
	    }
	  }
	}
      }
      //cout<<endl;
    }
  }

  void printPathsForPlot(int numPaths) {

    ofstream ofs;
    ofs.open("myTimer.dat");
    PathElem* pe;
    int count;
    if(numPaths<timingPaths.size())
      count = numPaths;
    else
      count = timingPaths.size();
    for(int i=0; i<count; i++) {
      pe = timingPaths[i]->getStartPoint();
      ofs<<"#"<<pe->getNode()->getPin()->getName()<<" ";
      pe = timingPaths[i]->getEndPoint();
      ofs<<pe->getNode()->getPin()->getName()<<endl;
      ofs<<pe->getNode()->getSlack(pe->getNodeTrans())<<endl;
    }
    ofs.close();
  }

};
