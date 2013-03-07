# ifndef COMMON_H
# define COMMON_H
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <utility>
# include <commonIter.h>
# include <assert.h>
# include <math.h>
# include <limits.h>
# include <time.h>
# include <Stat.h>
# include <algorithm>
# include <stdlib.h>

# define NIL(type) (type)0
# define COMMON_DEBUG 1
# define _setNULL(arg, type) \
  arg = NIL(type)

# define _ASSERT(txt, expr) \
  if (expr)  {		    \
    cout << txt << endl;    \
    exit(0);		    \
  }
# define _ASSERT_TRUE(txt)  \
    cout << txt << endl;    \
    exit(0);		    

using namespace std;

/* Import variable definitions to all modules */
extern unsigned int stepDepth;
extern bool debug;
extern bool performNetHidingConsistency;
extern unsigned int traceDepth;
extern bool performAnalysis;

# define _STEP_BEGIN(stepName)  {					\
    stepDepth++;							\
    if (traceDepth >= stepDepth) {					\
      for (int i=0; i < (stepDepth - 1); i++) cout << " ";		\
      cout << "BEGIN STEP: " << stepName				\
	   <<  "   CPU TIME:" << getCPUTime() << CPU_TIME_UNIT		\
	   <<  "   MEM USAGE:" << getMemUsage() << MEM_USAGE_UNIT << endl; \
    }									\
  }


# define _STEP_END(stepName)  {						\
    if (traceDepth >= stepDepth) {					\
      for (int i=0; i < (stepDepth - 1); i++) cout << " ";		\
      cout << "END STEP: " << stepName					\
	   <<  "   CPU TIME:" << getCPUTime() << CPU_TIME_UNIT		\
	   <<  "   MEM USAGE:" << getMemUsage() << MEM_USAGE_UNIT << endl; \
    }									\
    stepDepth--;							\
  }
  

# define _DEBUG_MSG(msg) \
  if (debug) cout << msg << endl;

# define _ERROR(msg) \
  cout << "Error:";  \
  cout << msg << endl;

# define MCOMMA ,

/*******************************************************************************
  Type definitions common for all objects
*******************************************************************************/
typedef enum {N, E, S, W, FN, FE, FS, FW} objOrient;

void common_error(string);
void common_message(string);

string getStrFromInt(int);

double dround(double);

objOrient getOrientationFromStr(string);
void vectorRemoveDuplicates(vector<unsigned int>& vec);
bool strIsNumber(const string& s);
string strToLower(const char *s);
string strToLower(const string& s);

#endif
