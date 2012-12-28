# include <Net.h>

Net::Net()
{
  NetSetId(0);
  NetSetWeight(0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
}

Net::Net(int id)
{
  NetSetId(id);
  NetSetWeight(0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
}

Net::Net(int id, const string& Name)
{
  NetSetId(id);
  NetSetName(Name);
  NetSetWeight(0);
  NetSetPinCount(0);
  NetSetDriverCount(0);
  NetSetLoadCount(0);
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
Net::NetSetWeight(unsigned int weight)
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
Net::NetAddPin(const Pin& pinToAdd)
{
  string Name = pinToAdd.PinGetName();

  Pins[Name] = (Pin *) &pinToAdd;
  this->pinCount++;
  if (pinToAdd.PinGetDirection() == PIN_DIR_INPUT) {
    this->loadCount++;
  } else if (pinToAdd.PinGetDirection() == PIN_DIR_OUTPUT) {
    this->driverCount++;
  }
}

int
Net::NetGetId(void)
{
  return Id;
}

unsigned int
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

map<string, Pin*>& Net::NetGetPins(void)
{
  return this->Pins;
}
