# include <common.h>
# include <NetIter.h>
# include <Pin.h>

# ifndef NET_H
# define NET_H

using namespace std;

class Net {
 private:
  int Id;
  unsigned int weight;
  unsigned int pinCount;
  unsigned int driverCount;
  unsigned int loadCount;

 public:
  map<string, Pin*> Pins;
  string name;
  Net();
  Net(int);
  Net(int, const string&);
  
  void NetSetName(const string&);
  void NetSetId(int);
  void NetSetWeight(unsigned int);
  void NetSetPinCount(unsigned int);
  void NetSetDriverCount(unsigned int);
  void NetSetLoadCount(unsigned int);
  void NetAddPin(const Pin&);

  int NetGetId(void);
  unsigned int NetGetPinCount(void);
  unsigned int NetGetDriverCount(void);
  unsigned int NetGetLoadCount(void);
  unsigned int NetGetWeight(void);
  string NetGetName(void);
  
  map<string, Pin*>& NetGetPins(void);
  ~Net();
};

# endif
