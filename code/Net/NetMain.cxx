# include <Net.h>

Net::Net()
{
  NetSetId(0);
  NetSetWeight(0.0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
  NetSetIsUnderCluster(false);
}

Net::Net(int id)
{
  NetSetId(id);
  NetSetWeight(0.0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
  NetSetIsUnderCluster(false);
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
Net::NetSetIsUnderCluster(const bool & isUnderCluster)
{
  this->isUnderCluster = isUnderCluster;
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
