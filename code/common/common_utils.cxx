# include <common.h>
# include <stdlib.h>

void 
common_error(std::string ErrorString)
{
# ifdef COMMON_DEBUG  
  std::cout << std::endl << "Error: " << ErrorString << std::endl << "Quitting.." << std::endl;
  exit(0);
# endif
}

void 
common_message(std::string MessageString)
{
# ifdef COMMON_DEBUG
  //  std::cout << std::endl << "Message from program:" << MessageString << std::endl;
# endif 
}

string 
getStrFromInt(int thisInt)
{
  stringstream oss;
  
  oss << thisInt;
  
  return oss.str();
}

string 
getStrFromDouble(double thisDouble)
{
  stringstream oss;
  
  oss << thisDouble;
  
  return oss.str();
}

objOrient
getOrientationFromStr(string orient)
{
  objOrient retVal;

  retVal = NIL(objOrient);
  if (orient == "N" || orient == "1") retVal = N;
  else if (orient == "E") retVal = E;
  else if (orient == "S") retVal = S;
  else if (orient == "W") retVal = W;
  else if (orient == "FN") retVal = FN;
  else if (orient == "FE") retVal = FE;
  else if (orient == "FS") retVal = FS;
  else if (orient == "FW") retVal = FW;

  return (retVal);
}

string
getStrForOrientation(objOrient orient)
{
  string retVal;;

  if (orient == N || orient == 1) retVal = "N";
  else if (orient == E) retVal = "E";
  else if (orient == S) retVal = "S";
  else if (orient == W) retVal = "W";
  else if (orient == FN) retVal = "FN";
  else if (orient == FE) retVal = "FE";
  else if (orient == FS) retVal = "FS";
  else if (orient == FW) retVal = "FW";

  return (retVal);
}

/* Rounds a double/float to the nearest integer */
int dtoi(double thisDouble)
{
  int upper, lower;
  int rtv;
  
  lower = floor(thisDouble);
  upper = lower + 1;
  
  rtv = thisDouble;
  if ((((double)upper) - thisDouble) >= (((double)thisDouble) - lower)) {
    rtv = lower;
  } else {
    rtv = upper;
  }
  return rtv;
}

/* Rounds a double/float to a single decimal point */
double dround(double thisDouble)
{
  long int thisInt;
  double absDouble;

  /* Get the integer part of thisDouble */
  thisInt = (long int)trunc(thisDouble);
  thisDouble = thisDouble - thisInt;
  
  absDouble = fabs(thisDouble);
  if (absDouble > 0 && absDouble <= 0.05) thisDouble = (double)thisInt;
  else if (absDouble > 0.05 && absDouble <= 0.1) (thisDouble < 0)?(thisDouble = (double)thisInt-0.1):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.1 && absDouble <= 0.15) (thisDouble < 0)?(thisDouble = (double)thisInt-0.1):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.15 && absDouble <= 0.2) (thisDouble < 0)?(thisDouble = (double)thisInt-0.2):(thisDouble = (double)thisInt+0.2);
  else if (absDouble > 0.2 && absDouble <= 0.25) (thisDouble < 0)?(thisDouble = (double)thisInt-0.2):(thisDouble = (double)thisInt+0.2);
  else if (absDouble > 0.25 && absDouble <= 0.3) (thisDouble < 0)?(thisDouble = (double)thisInt-0.3):(thisDouble = (double)thisInt+0.3);
  else if (absDouble > 0.3 && absDouble <= 0.35) (thisDouble < 0)?(thisDouble = (double)thisInt-0.3):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.35 && absDouble <= 0.4) (thisDouble < 0)?(thisDouble = (double)thisInt-0.4):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.4 && absDouble <= 0.45) (thisDouble < 0)?(thisDouble = (double)thisInt-0.4):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.45 && absDouble <= 0.5) (thisDouble < 0)?(thisDouble = (double)thisInt-0.5):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.5 && absDouble <= 0.55) (thisDouble < 0)?(thisDouble = (double)thisInt-0.5):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.55 && absDouble <= 0.6) (thisDouble < 0)?(thisDouble = (double)thisInt-0.6):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.6 && absDouble <= 0.65) (thisDouble < 0)?(thisDouble = (double)thisInt-0.6):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.65 && absDouble <= 0.7) (thisDouble < 0)?(thisDouble = (double)thisInt-0.7):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.7 && absDouble <= 0.75) (thisDouble < 0)?(thisDouble = (double)thisInt-0.7):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.75 && absDouble <= 0.8) (thisDouble < 0)?(thisDouble = (double)thisInt-0.8):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.8 && absDouble <= 0.85) (thisDouble < 0)?(thisDouble = (double)thisInt-0.8):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.85 && absDouble <= 0.9) (thisDouble < 0)?(thisDouble = (double)thisInt-0.9):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.9 && absDouble <= 0.95) (thisDouble < 0)?(thisDouble = (double)thisInt-0.9):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.95 && absDouble <= 1.0) (thisDouble < 0)?(thisDouble = (double)thisInt):(thisDouble = (double)thisInt);
  
  return (thisDouble);
}

void vectorRemoveDuplicates(vector<unsigned int>& vec)
{
  sort(vec.begin(), vec.end());
  vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

bool strIsNumber(const string& s)
{
  string::const_iterator it = s.begin();
  while (it != s.end() && isdigit(*it)) ++it;
  
  return !s.empty() && it == s.end();
}

int strToInt(const string &s) 
{
  int rtv;

  istringstream stream(s, istringstream::in);
  
  stream >> rtv;

  return rtv;
}

double strToDouble(const string &s) 
{
  double rtv;

  istringstream stream(s, istringstream::in);
  
  stream >> rtv;

  return rtv;
}

string strToLower(const string& s) 
{
  string retVal = s;
  int length = retVal.length();
  
  for (int i = 0;i < length;i++) {
    if (isupper(retVal[i])) {
      retVal[i] = tolower(retVal[i]);
    }
  }

  return retVal;
}

string strToLower(const char *s) 
{
  string retVal = s;
  int length = retVal.length();
  
  for (int i = 0;i < length;i++) {
    if (isupper(retVal[i])) {
      retVal[i] = tolower(retVal[i]);
    }
  }

  return retVal;
}

bool fileExists(string& fullFileName) 
{
  ifstream ifile(fullFileName.data());
  bool retVal;
  
  retVal = false;
  if (ifile) {
    retVal = true;
    ifile.close();
  }
  
  return (retVal);
} 

bool fileExists(string& fileName, string& filePath) 
{
  string fullFileName = fileName + filePath;
  bool retVal = fileExists(fullFileName);
  
  return (retVal);
} 

string getCurrentTime(void)
{
  string returnStr;

  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t );
  returnStr = " " + getStrFromInt(now->tm_year + 1900) + "-" + getStrFromInt(now->tm_mon + 1) + "-" + getStrFromInt(now->tm_mon + 1);
    //  cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mda << endl;

  return (returnStr);
}

void swap(uint &a, uint &b)
{
  a = a + b;
  b = a - b;
  a = a - b;
}

bool fileExists(const string &filename)
{
  ifstream ifile(filename.data());
  return ifile;
} 

bool dirExists(const string &dirname)
{
  bool rtv;

  rtv = false;
  const char *myDir = dirname.data();
  struct stat myStat;
  if ((stat(myDir, &myStat) == 0) && 
      (((myStat.st_mode) & S_IFMT) == S_IFDIR)) {
    rtv = true;
  }
  
  return rtv;
}

void 
displayHeartBeat(void)
{
  cout << ".";
}

/* Returns true if a string is a double */
bool
isDouble(string str)
{
  double dbl;
  bool rtv;

  rtv = false;
  istringstream stream(str, istringstream::in);
  if (stream >> dbl) {
    rtv = true;
  }

  return (rtv);
}

/* Creates a directory if it does not exist */
void makeDir(string dirName)
{
  if (!dirExists(dirName)) {
    if (!(0 == mkdir(dirName.data(), S_IRWXU | S_IRWXG | S_IRWXO))) {
      cout << "Error: Directory does not exist. Cannot create directory!!" << endl;
      exit(0);
    }
  }
}

/* Creates a directory if it does not exist */
void changeDir(string dirName)
{
  if (chdir(dirName.data()) != 0) {
    _ASSERT_TRUE(("Error: Could not change the directory name to " + dirName));
  }
}

/* Gets the host name */
string getHostName(void)
{
  char host[512];
  gethostname(host, sizeof host);

  string hostName(host);
  
  return (hostName);
}

/* Gets the user name */
string getUserName(void)
{
  char *username; 
  username = getenv("USER");
  
  string userName(username);
  
  return (userName);
}

/* Get platform string */
string getPlatformString(void)
{
  struct utsname buf;
  uname(&buf);

  string platInfo(buf.sysname);
  
  return platInfo;
}

/* Print an integer map */
void
printMap(map<uint,uint> mapToPrint)
{
  uint key;
  MAP_FOR_ALL_KEYS(mapToPrint, uint, uint, key) {
    cout << key << " " << endl;
  } END_FOR;
}

/* Execute a shell command. This is done after 
   blocking the profiler signal SIGPROF */
/* Internally calls system() */
int
executeCommand(string command)
{
  int status;
  struct sigaction new_action, old_action;
  
  new_action.sa_handler = SIG_IGN;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;
  
  sigaction (SIGPROF, &new_action, &old_action);
  status = system(command.data());
  sigaction (SIGPROF, &old_action, NULL);
  
  return status;
}

/* Compute x / y * 100 */
double
getPercent(double x, double y)
{
  double rtv;
  
  rtv = (x / y) * 100;
  
  return rtv;
}
