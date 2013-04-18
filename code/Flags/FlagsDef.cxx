/* Functions to define all flags that can be set on objects */
/* All maps are of type map<void *, bool> and therefore can be */
/* created in an array of vectors */
# include <Flags.h>

vector<map<void *, unsigned int> > FlagsDefInt;
vector<map<void *, void*> > FlagsDefPtr;
vector<map<void *, double> > FlagsDefDouble;

/* Init functions */
void FlagsInit()
{
  for (int i=0; i < TOTAL_NUM_INT_FLAGS; i++) {
    FlagsDefInt.push_back(map<void *, unsigned int>());
  }
  for (int i=0; i < TOTAL_NUM_PTR_FLAGS; i++) {
    FlagsDefPtr.push_back(map<void *, void *>());
  }
  for (int i=0; i < TOTAL_NUM_DOUBLE_FLAGS; i++) {
    FlagsDefDouble.push_back(map<void *, double>());
  }
}

/* Get and set functions for boolean type flag */
bool FlagsDefGetFlag(flagIntType fType, void *obj)
{
  map<void *, unsigned int> &objMap = FlagsDefInt[fType];
  bool rtv;
  
  std::map<void *, unsigned int>::iterator mapIter = objMap.find(obj);
  rtv = 0;
  if (mapIter != objMap.end()) {
    rtv = mapIter->second;
  }

  return (rtv == 1);
}

void FlagsDefSetFlag(flagIntType fType, void* obj)
{
  (FlagsDefInt[fType])[obj] = 1;
}

/* Get and set functions for integer type flag */
unsigned int FlagsDefGetInt(flagIntType fType, void* obj)
{
  map<void *, unsigned int> &objMap = FlagsDefInt[fType];
  unsigned int rtv;
  
  std::map<void *, unsigned int>::iterator mapIter = objMap.find(obj);
  rtv = 0;
  if (mapIter != objMap.end()) {
    rtv = mapIter->second;
  }

  return (rtv);
}

void FlagsDefSetInt(flagIntType fType, void* obj, unsigned int val)
{
  (FlagsDefInt[fType])[obj] = val;
}

/* Get and set functions for pointer type flag */
void* FlagsDefGetPtr(flagPtrType fType, void *obj)
{
  map<void *, void*> &objMap = FlagsDefPtr[fType];
  void* rtv;
  
  std::map<void *, void*>::iterator mapIter = 
    objMap.find(obj);

  rtv = NIL(void *);
  if (mapIter != objMap.end()) {
    rtv = mapIter->second;
  }

  return rtv;
}

void FlagsDefSetPtr(flagPtrType fType, void *obj, void *ptr)
{
  (FlagsDefPtr[fType])[obj] = ptr;
}

/* Get and set functions for double type flag */
double FlagsDefGetDouble(flagDoubleType fType, void *obj)
{
  map<void *, double> &objMap = FlagsDefDouble[fType];
  double rtv;
  
  std::map<void *, double>::iterator mapIter = 
    objMap.find(obj);

  rtv = 0.0;
  if (mapIter != objMap.end()) {
    rtv = mapIter->second;
  }

  return rtv;
}

void FlagsDefSetDouble(flagDoubleType fType, void *obj, double val)
{
  (FlagsDefDouble[fType])[obj] = val;
}


/* Clear function for boolean flag */
void FlagsDefClearFlag(flagIntType fType, void* obj)
{
  map<void *, unsigned int> &objMap = FlagsDefInt[fType];
  
  std::map<void *, unsigned int>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}

/* Clear function for boolean int */
void FlagsDefClearInt(flagIntType fType, void* obj)
{
  map<void *, unsigned int> &objMap = FlagsDefInt[fType];
  
  std::map<void *, unsigned int>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}

/* Clear function for pointer  */
void FlagsDefClearPtr(flagPtrType fType, void *obj)
{
  map<void *, void *> &objMap = FlagsDefPtr[fType];
  
  std::map<void *, void *>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}

/* Clear function for double  */
void FlagsDefClearDouble(flagDoubleType fType, void *obj)
{
  map<void *, double> &objMap = FlagsDefDouble[fType];
  
  std::map<void *, double>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}

/* Clear all function for boolean type  */
void FlagsDefClearAllObjs(flagIntType fType)
{
  map<void *, unsigned int> &objMap = FlagsDefInt[fType];
  
  objMap.clear();
}

/* Clear all function for ptr type  */
void FlagsDefClearAllObjs(flagPtrType fType)
{
  map<void *, void *> &objMap = FlagsDefPtr[fType];
  
  objMap.clear();
}

/* Clear all function for double type  */
void FlagsDefClearAllObjs(flagDoubleType fType)
{
  map<void *, double> &objMap = FlagsDefDouble[fType];
  
  objMap.clear();
}
