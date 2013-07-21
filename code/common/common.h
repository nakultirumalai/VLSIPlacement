# ifndef COMMON_H
# define COMMON_H
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <queue>
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
# include <string.h>
# include <ctime>
# include <iomanip>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/utsname.h>
# include <unistd.h>
# include <signal.h>
# include <google/profiler.h>

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

# define DBL_MAX 1e+37
# define GRID_COMPACTION_RATIO 1000
# define GRID_COMPACTION_RATIO_SQUARED 1000000

using namespace std;

typedef unsigned int uint;
typedef unsigned long ulong;

/* Import variable definitions to all modules */
extern unsigned int stepDepth;
extern bool debug;
extern bool performNetHidingConsistency;
extern unsigned int traceDepth;
extern bool performAnalysis;
extern bool varOption;
extern uint starModelUpgradeNumPins;

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

# define _KEY_EXISTS(mapName, mapKey)	     \
  if (mapName.find(mapKey) != mapName.end()) \

# define _KEY_DOES_NOT_EXIST(mapName, mapKey)	\
  if (mapName.find(mapKey) == mapName.end())	\

# define _KEY_EXISTS_WITH_VAL(mapName, mapKey, mapItrVal) \
  if ((mapItrVal = mapName.find(mapKey)) != mapName.end()) \

/*******************************************************************************
  Type definitions common for all objects
*******************************************************************************/
typedef enum {N, E, S, W, FN, FE, FS, FW} objOrient;

/*******************************************************************************
  Type definition to specify solver types
*******************************************************************************/
typedef enum {NO_SEQ_SOLVE, 
	      SEQSOLVE_QO_WL, 
	      SEQSOLVE_QCQO_WL, 
	      SEQSOLVE_QO_TD, 
	      SEQSOLVE_QCQO_TD} seqSolverType;
typedef enum {ALL_QO_WL, 
	      ALL_QCQO_WL, 
	      ALL_QO_TD, 
	      ALL_QCQO_TD} allSolverType;

void common_error(string);
void common_message(string);

string getStrFromInt(int);

double dround(double);
int dtoi(double);

objOrient getOrientationFromStr(string);
string getStrForOrientation(objOrient);

void vectorRemoveDuplicates(vector<unsigned int>& vec);
bool strIsNumber(const string& s);
int strToInt(const string &s);
double strToDouble(const string& s);
string strToLower(const char *s);
string strToLower(const string& s);
bool fileExists(string& fullFileName);
bool fileExists(string& filePath, string &fileName);
string getCurrentTime(void);

void swap(uint& a, uint &b);
bool fileExists(const string &);
bool dirExists(const string &);
void displayHeartBeat(void);
bool isDouble(string);

void makeDir(string dirName);
void changeDir(string dirName);

string getPlatformString(void);
string getUserName(void);
string getHostName(void);
int executeCommand(string);

#endif
