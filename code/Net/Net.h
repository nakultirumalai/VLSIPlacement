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
 public:
  map<string, Pin*> Pins;
  string name;
  Net();
  Net(int);
  Net(int, const string&);
  
  void NetSetName(const string&);
  void NetSetId(int);
  void NetSetWeight(unsigned int);
  void NetAddPin(const Pin&);

  int NetGetId(void);
  unsigned int NetGetWeight(void);
  string NetGetName(void);

  map<string, Pin*>& NetGetPins(void);
  ~Net();
};

# endif
