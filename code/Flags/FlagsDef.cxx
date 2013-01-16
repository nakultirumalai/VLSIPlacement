/* Functions to define all flags that can be set on objects */
/* All maps are of type map<void *, bool> and therefore can be */
/* created in an array of vectors */
# include <Flags.h>

vector<map<void *, bool> > FlagsDef;

void FlagsInit()
{
  FlagsDef.reserve(TOTAL_NUM_FLAGS);
}

bool FlagsDefGetFlag(flagType fType, void *obj)
{
  map<void *, bool> &objMap = FlagsDef[fType];
  bool rtv;
  
  std::map<void *, bool>::iterator mapIter = objMap.find(obj);
  rtv = false;
  if (mapIter != objMap.end()) {
    rtv = mapIter->second;
  }

  return rtv;
}

void FlagsDefSetFlag(flagType fType, void* obj)
{
  (FlagsDef[fType])[obj] = true;
}

void FlagsDefClearFlag(flagType fType, void* obj)
{
  map<void *, bool> &objMap = FlagsDef[fType];
  
  std::map<void *, bool>::iterator mapIter = objMap.find(obj);

  if (mapIter != objMap.end()) {
    objMap.erase(mapIter);
  }
}

void FlagsDefClearFlagAllObjs(flagType fType)
{
  map<void *, bool> &objMap = FlagsDef[fType];
  
  objMap.clear();
}

