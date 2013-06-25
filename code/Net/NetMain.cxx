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
Net::NetSetPinCount(unsigned int pinCount)
{
  this->pinCount = pinCount;
}

void
Net::NetSetDriverCount(unsigned int driverCount)
{
  this->driverCount = driverCount;
}

void
Net::NetSetLoadCount(unsigned int loadCount)
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
  minx = 0;
  miny = 0;
  maxx = 0;
  maxy = 0;
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
Net::NetAddPin(const Pin& pinToAdd)
{
  string Name = pinToAdd.PinGetName();

  Pins[Name] = (Pin *) &pinToAdd;
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
Net::NetRemovePin(const Pin& pinToRemove)
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

unsigned int
Net::NetGetPinCount(void)
{
  return pinCount;
}

unsigned int
Net::NetGetDriverCount(void)
{
  return this->driverCount;
}

unsigned int
Net::NetGetLoadCount(void)
{
  return this->loadCount;
}

bool
Net::NetIsUnderCluster(void)
{
  return (this->isUnderCluster);
}

double
Net::NetComputeHPWL(uint &xHPWL, uint &yHPWL)
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
