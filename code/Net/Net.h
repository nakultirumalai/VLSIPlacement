# include <common.h>
# include <NetIter.h>
# include <Pin.h>

# ifndef NET_H
# define NET_H

using namespace std;

class Net {
 private:
  int Id;
  unsigned int pinCount;
  unsigned int driverCount;
  unsigned int loadCount;
  bool isUnderCluster;
  bool isHidden;
  double weight;

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
  void NetSetPinCount(unsigned int);
  void NetSetDriverCount(unsigned int);
  void NetSetLoadCount(unsigned int);
  void NetSetIsUnderCluster(const bool&);
  void NetSetIsHidden(const bool &);
  void NetAddPin(const Pin&);
  void NetRemovePin(const Pin&);

  int NetGetId(void);
  unsigned int NetGetPinCount(void);
  unsigned int NetGetDriverCount(void);
  unsigned int NetGetLoadCount(void);
  double NetGetWeight(void);
  string NetGetName(void);
  bool NetIsUnderCluster(void);
  bool NetIsHidden(void);
  
  map<string, Pin*>& NetGetPins(void);
  map<string, Pin*>& NetGetPins(char);
  ~Net();
};

# endif
