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

vector<Node*>& Edge::EdgeGetNodes(void)
{

}

Edge::Edge() 
{

}

