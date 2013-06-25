# include <common.h>
# include <NetIter.h>
# include <Pin.h>

# ifndef NET_H
# define NET_H

using namespace std;

class Net {
 private:
  double weight;
  double hpwl;
  uint maxx, maxy;
  uint minx, miny;
  uint pinCount;
  uint driverCount;
  uint loadCount;
  int Id;
  bool isUnderCluster;
  bool isHidden;

 public:
  map<string, Pin*> inPins;
  map<string, Pin*> outPins;
  map<string, Pin*> Pins;
  string name;
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
  void NetInitHPWL(void);
  void NetAddPin(const Pin&);
  void NetRemovePin(const Pin&);

  int NetGetId(void);
  uint NetGetPinCount(void);
  uint NetGetDriverCount(void);
  uint NetGetLoadCount(void);
  double NetGetWeight(void);
  string NetGetName(void);
  bool NetIsUnderCluster(void);
  double NetComputeHPWL(uint &, uint &);
  bool NetIsHidden(void);
  
  map<string, Pin*>& NetGetPins(void);
  map<string, Pin*>& NetGetPins(char);
  ~Net();
};

# endif
