# include <Edge.h>

void Edge::EdgeAddNode(Node *newNode)
{
  nodeSet.insert(nodeSet.end(), newNode);
}

void* Edge::EdgeGetObject(void)
{
  return (object);
}

void Edge::EdgeSetObject(void *object)
{
  this->object = object;
}

void Edge::EdgeSetIdx(unsigned int edgeIdx)
{
  this->edgeIdx = edgeIdx;
}

vector<Node*>& Edge::EdgeGetNodes(void)
{

}

Edge::Edge() 
{

}

Edge::Edge(void *object)
{
  EdgeSetObject(object);
}

