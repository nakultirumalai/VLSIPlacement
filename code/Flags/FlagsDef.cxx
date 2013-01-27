/* Functions to define all flags that can be set on objects */
/* All maps are of type map<void *, bool> and therefore can be */
/* created in an array of vectors */
# include <Flags.h>

vector<map<void *, bool> > FlagsDefInt;
vector<map<void *, void*> > FlagsDefPtr;

void FlagsInit()
{
  for (int i=0; i < TOTAL_NUM_INT_FLAGS; i++) {
    FlagsDefInt.push_back(map<void *, bool>());
  }
  for (int i=0; i < TOTAL_NUM_PTR_FLAGS; i++) {
    FlagsDefPtr.push_back(map<void *, void *>());
  }
}

bool FlagsDefGetFlag(flagIntType fType, void *obj)
{
  map<void *, bool> &objMap = FlagsDefInt[fType];
  bool rtv;
  
  std::map<void *, bool>::iterator mapIter = objMap.find(obj);
  rtv = false;
  if (mapIter != objMap.end()) {
    rtv = mapIter->second;
  }

  return rtv;
}

void FlagsDefSetFlag(flagIntType fType, void* obj)
{
  (FlagsDefInt[fType])[obj] = true;
}

void FlagsDefClearFlag(flagIntType fType, void* obj)
{
  map<void *, bool> &objMap = FlagsDefInt[fType];
  
  std::map<void *, bool>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}

void FlagsDefClearAllObjs(flagIntType fType)
{
  map<void *, bool> &objMap = FlagsDefInt[fType];
  
  objMap.clear();
}

void FlagsDefClearAllObjs(flagPtrType fType)
{
  map<void *, void *> &objMap = FlagsDefPtr[fType];
  
  objMap.clear();
}

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

void FlagsDefClearPtr(flagPtrType fType, void *obj)
{
  map<void *, void *> &objMap = FlagsDefPtr[fType];
  
  std::map<void *, void *>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}
