# include <Edge.h>

void Edge::EdgeAddNode(unsigned int nodeIdx)
{
  nodeSet.insert(nodeSet.end(), nodeIdx);
}

void* Edge::EdgeGetObject(void)
{
  return (object);
}

double Edge::EdgeGetWeight(void)
{
  return (this->weight);
}

void Edge::EdgeSetObject(void *object)
{
  this->object = object;
}

void Edge::EdgeSetIdx(unsigned int edgeIdx)
{
  this->edgeIdx = edgeIdx;
}

void Edge::EdgeSetWeight(double weight)
{
  this->weight = weight;
}

void Edge::EdgeSetIsHidden(bool hidden)
{
  this->isHidden = hidden;
}

vector<unsigned int>& Edge::EdgeGetNodes(void)
{
  return nodeSet;
}

bool Edge::EdgeIsHidden(void)
{
  return (this->isHidden);
}

Edge::Edge() 
{
  EdgeSetIdx(0);
  EdgeSetObject(NIL(void*));
  EdgeSetWeight(0.0);
  EdgeSetIsHidden(false);
}

Edge::Edge(void *object)
{
  EdgeSetIdx(0);
  EdgeSetObject(object);
  EdgeSetWeight(0.0);
  EdgeSetIsHidden(false);
}

Edge::~Edge()
{

}
