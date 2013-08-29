# include <common.h>
# include <NetIter.h>
# include <Pin.h>
# include <Cell.h>

# ifndef NET_H
# define NET_H

using namespace std;

class Net {
 private:
  uint pinCount, driverCount,loadCount;
  int Id;
  bool isUnderCluster;
  bool isHidden;
  bool dirtyHPWL;

 public:
  map<string, Pin*> inPins;
  map<string, Pin*> outPins;
  map<string, Pin*> Pins;
  vector<Pin *> PinsVecX, PinsVecY;
  vector<Pin *> PinList;
  uint maxx, maxy, minx, miny;
  uint xhpwl, yhpwl;
  Pin *pinMaxx, *pinMaxy, *pinMinx, *pinMiny;
  string name;
  double weight;

  Net();
  Net(int);
  Net(int, const string&);
  
  void NetSetName(const string&);
  void NetSetId(int);
  void NetSetWeight(double);
  void NetSetPinCount(uint);
  void NetSetDriverCount(uint);
  void NetSetLoadCount(uint);
  void NetSetIsUnderCluster(const bool&);
  void NetSetIsHidden(const bool &);
  void NetInitMinMaxPositions(void);
  void NetSetMinMaxPositions(uint, uint);
  void NetSetDirtyHPWL(bool);
  void NetInitHPWL(void);
  void NetAddPin(Pin&);
  void NetRemovePin(Pin&);
  void NetUpdateMinMaxX(uint &, uint &);
  void NetUpdateMinMaxY(uint &, uint &);

  int NetGetId(void);
  uint NetGetPinCount(void);
  uint NetGetDriverCount(void);
  uint NetGetLoadCount(void);
  double NetGetWeight(void);
  string NetGetName(void);
  bool NetGetDirtyHPWL(void);
  bool NetIsUnderCluster(void);
  void NetGetHPWL(uint &, uint &);
  void NetComputeHPWL(uint &, uint &);
  bool NetIsHidden(void);
  
  map<string, Pin*>& NetGetPins(void);
  map<string, Pin*>& NetGetPins(char);
  vector<Pin *> &NetGetAllPinsVector(void);
  ~Net();
};

# endif
