# include <HyperGraph.h>
# include <Cell.h>

void 
Node::NodeAddEdge(unsigned int edgeIdx, edgeType type)
{
  if (type == IN_EDGE) {
    inEdges.insert(inEdges.end(), edgeIdx);
    numInEdges++;
  } else if (type == OUT_EDGE) {
    outEdges.insert(outEdges.end(), edgeIdx);
    numOutEdges++;
  }
}

void 
Node::NodeAddEdge(unsigned int edgeIdx)
{
  NodeAddEdge(edgeIdx, IN_EDGE);
}

unsigned int 
Node::NodeGetFlags(void)
{
  return (this->flags);
}

void* 
Node::NodeGetData(void)
{
  return (void *)data;
}

unsigned int 
Node::NodeGetIdx(void)
{
  return (nodeIdx);
}

unsigned int 
Node::NodeGetAnnIdx1(void)
{
  this->annIdx1 = annIdx1;
}

unsigned int 
Node::NodeGetAnnIdx2(void)
{
  this->annIdx2 = annIdx3;
}

unsigned int 
Node::NodeGetAnnIdx3(void)
{
  this->annIdx3 = annIdx3;
}

unsigned int 
Node::NodeGetNumInEdges(void)
{
  return (numInEdges);
}

unsigned int 
Node::NodeGetNumOutEdges(void)
{
  return (numOutEdges);
}

unsigned int 
Node::NodeGetNumEdges(void)
{
  return (numInEdges + numOutEdges);
}

void 
Node::NodeSetIdx(unsigned int nodeIdx)
{
  this->nodeIdx = nodeIdx;
}

void
Node::NodeSetFlags(unsigned int flags)
{
  this->flags = flags;
}

void 
Node::NodeSetAnnIdx1(unsigned int annIdx1)
{
  this->annIdx1 = annIdx1;
}

void 
Node::NodeSetAnnIdx2(unsigned int annIdx2)
{
  this->annIdx2 = annIdx3;
}

void 
Node::NodeSetAnnIdx3(unsigned int annIdx3)
{
  this->annIdx3 = annIdx3;
}

void
Node::NodeSetData(void *object)
{
  data = object;
}

void 
Node::NodeSetNumInEdges(unsigned int numInEdges)
{
  this->numInEdges = numInEdges;
}

void
Node::NodeSetNumOutEdges(unsigned int numOutEdges)
{
  this->numOutEdges = numOutEdges;
}

Node::Node() 
{
  NodeSetNumInEdges(0);
  NodeSetNumOutEdges(0);
  NodeSetIdx(0);
  NodeSetFlags(0);
  NodeSetAnnIdx1(0);
  NodeSetAnnIdx2(0);
  NodeSetAnnIdx3(0);
  NodeSetData(NIL(void*));
}

Node::Node(void *object)
{
  NodeSetNumInEdges(0);
  NodeSetNumOutEdges(0);
  NodeSetIdx(0);
  NodeSetFlags(0);
  NodeSetAnnIdx1(0);
  NodeSetAnnIdx2(0);
  NodeSetAnnIdx3(0);
  NodeSetData(object);
}
