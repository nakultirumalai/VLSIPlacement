# include <Net.h>

Net::Net()
{
  NetSetId(0);
  NetSetWeight(0.0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
  NetSetIsUnderCluster(false);
  NetSetIsHidden(false);
  NetSetDirtyHPWL(true);
  NetInitMinMaxPositions();
}

Net::Net(int id)
{
  NetSetId(id);
  NetSetWeight(0.0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
  NetSetIsUnderCluster(false);
  NetSetIsHidden(false);
  NetSetDirtyHPWL(true);
  NetInitMinMaxPositions();
}

Net::Net(int id, const string& Name)
{
  NetSetId(id);
  NetSetName(Name);
  NetSetWeight(0.0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
  NetSetIsUnderCluster(false);
  NetSetIsHidden(false);
  NetSetDirtyHPWL(true);
  NetInitMinMaxPositions();
}

void 
Net::NetSetName(const string& Name)
{
  name = Name;
}

void 
Net::NetSetId(int id)
{
  Id = id;
}

void
Net::NetSetWeight(double weight)
{
  this->weight = weight;
}

void
Net::NetSetPinCount(uint pinCount)
{
  this->pinCount = pinCount;
}

void
Net::NetSetDriverCount(uint driverCount)
{
  this->driverCount = driverCount;
}

void
Net::NetSetLoadCount(uint loadCount)
{
  this->loadCount = loadCount;
}

void
Net::NetSetIsUnderCluster(const bool &isUnderCluster)
{
  this->isUnderCluster = isUnderCluster;
}

void
Net::NetSetIsHidden(const bool &isHidden)
{
  this->isHidden = isHidden;
}

void
Net::NetInitMinMaxPositions(void) 
{
  minx = INT_MAX;
  miny = INT_MAX;
  maxx = 0;
  maxy = 0;
  xhpwl = 0;
  yhpwl = 0;
}

void
Net::NetSetMinMaxPositions(uint xPos, uint yPos)
{
  if (xPos < minx) {
    minx = xPos;
  } else if (xPos > maxx) {
    maxx = xPos;
  }
  if (yPos < miny) {
    miny = yPos;
  } else if (yPos > maxy) {
    maxy = yPos;
  }
}

void
Net::NetSetDirtyHPWL(bool dirtyHPWL) 
{
  this->dirtyHPWL = dirtyHPWL;
}

void
Net::NetAddPin(Pin& pinToAdd)
{
  string Name = pinToAdd.PinGetName();
  Pin *pinPtr;

  pinPtr = &pinToAdd;
  Pins[Name] = (Pin *) pinPtr;
  PinsVecX.push_back(pinPtr);
  PinsVecY.push_back(pinPtr);
  this->pinCount++;
  if (pinToAdd.PinGetDirection() == PIN_DIR_INPUT) {
    inPins[Name] = (Pin *) &pinToAdd;
    this->loadCount++;
  } else if (pinToAdd.PinGetDirection() == PIN_DIR_OUTPUT) {
    outPins[Name] = (Pin *) &pinToAdd;
    this->driverCount++;
  }
}

void
Net::NetRemovePin(Pin& pinToRemove)
{
  string Name = pinToRemove.PinGetName();
  
  _KEY_EXISTS(Pins, Name) {
    Pins.erase(Name);
    this->pinCount--;
  } else {
    string assertString = "Cannot find pin " + Name 
      + " on net " + this->name;
    _ASSERT_TRUE(assertString);
  }
  if (pinToRemove.PinGetDirection() == PIN_DIR_INPUT) {
    _KEY_EXISTS(inPins, Name) {
      inPins.erase(Name);
    } else {
      string assertString = "Cannot find pin " + Name 
	+ " on net " + this->name;
      _ASSERT_TRUE(assertString);
    }
    this->loadCount--;
  } else if (pinToRemove.PinGetDirection() == PIN_DIR_OUTPUT) {
    _KEY_EXISTS(outPins, Name) {
      outPins.erase(Name);
    } else {
      string assertString = "Cannot find output pin " + Name 
	+ " on net " + this->name;
      _ASSERT_TRUE(assertString);
    }
    this->driverCount--;
  }
}

int
Net::NetGetId(void)
{
  return Id;
}

double
Net::NetGetWeight(void)
{
  return weight;
}

string
Net::NetGetName(void)
{
  return name;
}

bool
Net::NetGetDirtyHPWL(void) 
{
  return (this->dirtyHPWL);
}

uint
Net::NetGetPinCount(void)
{
  return pinCount;
}

uint
Net::NetGetDriverCount(void)
{
  return this->driverCount;
}

uint
Net::NetGetLoadCount(void)
{
  return this->loadCount;
}

bool
Net::NetIsUnderCluster(void)
{
  return (this->isUnderCluster);
}


void
Net::NetComputeHPWL(uint &xOldHPWL, uint &yOldHPWL, uint &xNewHPWL, uint &yNewHPWL)
{
  Pin *pinPtr;
  struct cmpPinXposStruct cmpPinXpos;
  struct cmpPinYposStruct cmpPinYpos;

  xOldHPWL = xhpwl;
  yOldHPWL = yhpwl;
  
  if (NetGetDirtyHPWL()) {
    //    cout << name << " is dirty" << endl;
    std::sort(PinsVecX.begin(), PinsVecX.end(), cmpPinXpos);
    std::sort(PinsVecY.begin(), PinsVecY.end(), cmpPinYpos);
    maxx = (*PinsVecX[0]).PinGetAbsXPos();
    maxy = (*PinsVecY[0]).PinGetAbsYPos();
    minx = (*PinsVecX[pinCount - 1]).PinGetAbsXPos();
    miny = (*PinsVecY[pinCount - 1]).PinGetAbsYPos();
    NetSetDirtyHPWL(false);
  }
  xNewHPWL = maxx - minx; xhpwl = xNewHPWL;
  yNewHPWL = maxy - miny; yhpwl = yNewHPWL;
}

void
Net::NetComputeHPWL(uint &xHPWL, uint &yHPWL)
{
  Pin *pinPtr;
  Cell *cellPtr;
  uint cellXpos, cellYpos;
  uint pinXPos, pinYPos;
  struct cmpPinXposStruct cmpPinXpos;
  struct cmpPinYposStruct cmpPinYpos;
  uint idx;

  if (NetGetDirtyHPWL()) {
    maxx = 0;
    maxy = 0;
    minx = INT_MAX;
    miny = INT_MAX;
    for (idx = 0; idx < pinCount; idx++) {
      pinPtr = PinsVecX[idx];
      pinXPos = (*pinPtr).PinGetAbsXPos();
      pinYPos = (*pinPtr).PinGetAbsYPos();
      if (maxx < pinXPos) maxx = pinXPos;
      if (minx > pinXPos) minx = pinXPos;
      if (maxy < pinYPos) maxy = pinYPos;
      if (miny > pinYPos) miny = pinYPos;
      idx++;
      if (idx == pinCount) continue;
      pinPtr = PinsVecX[idx];
      pinXPos = (*pinPtr).PinGetAbsXPos();
      pinYPos = (*pinPtr).PinGetAbsYPos();
      if (maxx < pinXPos) maxx = pinXPos;
      if (minx > pinXPos) minx = pinXPos;
      if (maxy < pinYPos) maxy = pinYPos;
      if (miny > pinYPos) miny = pinYPos;
    }
    /*    maxx = (*PinsVecX[0]).PinGetAbsXPos();
    maxy = (*PinsVecY[0]).PinGetAbsYPos();
    minx = (*PinsVecX[pinCount - 1]).PinGetAbsXPos();
    miny = (*PinsVecY[pinCount - 1]).PinGetAbsYPos();
    */
    NetSetDirtyHPWL(false);
  }

  xHPWL = maxx - minx; 
  yHPWL = maxy - miny; 
}

bool
Net::NetIsHidden(void)
{
  return (this->isHidden);
}

map<string, Pin*>& Net::NetGetPins(void)
{
  return this->Pins;
}

map<string, Pin*>& Net::NetGetPins(char direction)
{
  if (direction == PIN_DIR_INPUT) {
    return this->inPins;
  }
  if (direction == PIN_DIR_OUTPUT) {
    return this->outPins;
  }
}
