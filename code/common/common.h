# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <utility>
# include <commonIter.h>
# include <assert.h>
# include <math.h>

# define NIL(type) (type)0
# define COMMON_DEBUG 1
# define _setNULL(arg, type) \
  arg = NIL(type)

# define _ASSERT(txt, expr) \
  if (expr)  {		    \
    cout << txt << endl;    \
    assert(1);		    \
  }
# define _ASSERT_TRUE(txt)  \
    cout << txt << endl;    \
    assert(1);		    

using namespace std;

void common_error(string);
void common_message(string);

string getStrFromInt(int);

double dround(double);
