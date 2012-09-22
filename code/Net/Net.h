# include <common.h>
# include <Pin.h>

# ifndef NET_H
# define NET_H

using namespace std;

class Net {
 private:
  int Id;
  map <string, Pin*> Pins;

 public:
  string name;
  Net();
  Net(int);
  Net(int, const string&);
  
  void NetSetName(const string&);
  void NetSetId(int);
  
  void NetAddPin(const Pin&);
  
  int NetGetId(void);
  string NetGetName(void);
  
  ~Net();
};

# endif
