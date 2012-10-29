# include <HyperGraph.h>
# include <Cell.h>

void Node::NodeAddEdge(Edge *newEdge)
{
  inEdges.insert(inEdges.end(), newEdge);
  numInEdges++;
}

void Node::NodeAddEdge(Edge *newEdge, edgeType type)
{
  if (type == IN_EDGE) {
    inEdges.insert(inEdges.end(), newEdge);
    numInEdges++;
  } else if (type == OUT_EDGE) {
    outEdges.insert(outEdges.end(), newEdge);
    numOutEdges++;
  }
}

void* Node::NodeGetData(void)
{
  return (void *)data;
}

unsigned int Node::NodeGetIdx(void)
{
  return (nodeIdx);
}

unsigned char Node::NodeGetNumInEdges(void)
{
  return (numInEdges);
}

unsigned char Node::NodeGetNumOutEdges(void)
{
  return (numOutEdges);
}

unsigned char Node::NodeGetNumEdges(void)
{
  return (numInEdges + numOutEdges);
}

vector<Node *>& Node::NodeGetAdjacentNodes(void)
{

}

void Node::NodeSetData(void *object)
{
  data = object;
}

Node::Node(void *object)
{
  this->data = object;
}
