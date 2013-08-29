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
  PinList.push_back(pinPtr);
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
  Pin *pinPtr;
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
  uint idx = 0;
  VECTOR_FOR_ALL_ELEMS(PinList, Pin*, pinPtr) {
    if (pinPtr == &pinToRemove) {
      PinList.erase(PinList.begin() + idx);
      break;
    }
    idx++;
  } END_FOR;
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
Net::NetComputeHPWL(uint &xHPWL, uint &yHPWL)
{
  Pin *pinPtr;
  Cell *cellPtr;
  uint cellXpos, cellYpos;
  uint pinXpos, pinYpos;
  uint idx;
  
  maxx = 0; maxy = 0;
  minx = INT_MAX; miny = INT_MAX;

  //  cout << "HPWL: Net: " << name << endl;
  for (idx = 0; idx < pinCount; idx++) {
    pinPtr = PinsVecX[idx];
    if ((*pinPtr).isHidden) {
      continue;
    }
    cellPtr = (*pinPtr).PinGetParentCellPtr();
    pinXpos = pinPtr->xOffset + cellPtr->x;
    pinYpos = pinPtr->yOffset + cellPtr->y;
    /*
    if ((*cellPtr).CellIsHidden()) {
      cout << "Cell: " << (*cellPtr).CellGetName() << " X: " << cellPtr->x << " Y: " << cellPtr->y 
	   << " Abs: X: " << pinXpos << " Y: " << pinYpos << endl;
	   }*/
    if (maxx < pinXpos) {
      maxx = pinXpos;
      pinMaxx = pinPtr;
    }
    if (maxy < pinYpos) {
      maxy = pinYpos;
      pinMaxy = pinPtr;
    }
    if (minx > pinXpos) {
      minx = pinXpos;
      pinMinx = pinPtr;
    }
    if (miny > pinYpos) {
      miny = pinYpos;
      pinMiny = pinPtr;
    }
  }
  //  cout << "     Maxx : " << maxx << "   Maxy : " << maxy << endl;
  //  cout << "     Minx : " << minx << "   Miny : " << miny << endl;
  xHPWL = maxx - minx; 
  yHPWL = maxy - miny; 
}

void
Net::NetGetHPWL(uint &xHPWL, uint &yHPWL)
{
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

vector<Pin *> & Net::NetGetAllPinsVector(void)
{
  return (this->PinList);
}
