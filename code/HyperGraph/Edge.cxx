# include <Edge.h>

void Edge::EdgeAddNode(unsigned int nodeIdx)
{
  nodeSet.insert(nodeSet.end(), nodeIdx);
}

void* Edge::EdgeGetObject(void)
{
  return (object);
}

unsigned int Edge::EdgeGetWeight(void)
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

void Edge::EdgeSetWeight(unsigned int weight)
{
  this->weight = weight;
}

vector<unsigned int>& Edge::EdgeGetNodes(void)
{
  return nodeSet;
}

Edge::Edge() 
{
  EdgeSetIdx(0);
  EdgeSetObject(NIL(void*));
  EdgeSetWeight(0);
}

Edge::Edge(void *object)
{
  EdgeSetIdx(0);
  EdgeSetObject(object);
  EdgeSetWeight(0);
}

