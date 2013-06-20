# include <Node.h>

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

bool
Node::NodeIsCluster(void)
{
  return (this->isCluster);
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

bool
Node::NodeIsTop(void)
{
  unsigned int flags;
  bool result;
  
  flags = NodeGetFlags();
  result = (flags & NODE_FLAG_IS_TOP);

  return (result);
}

bool
Node::NodeIsClusterChild(void)
{
  unsigned int flags;
  bool result;
  
  flags = NodeGetFlags();
  result = (flags & NODE_FLAG_IS_CLUSTER_CHILD);

  return (result);
}

bool
Node::NodeIsClusterParent(void)
{
  unsigned int flags;
  bool result;
  
  flags = NodeGetFlags();
  result = (flags & NODE_FLAG_IS_CLUSTER_PARENT);

  return (result);
}

bool
Node::NodeIsInvalid(void)
{
  unsigned int flags;
  bool result;

  flags = NodeGetFlags();
  result = (flags & NODE_FLAG_IS_INVALID);

  return (result);
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
Node::NodeSetIsCluster(bool isCluster)
{
  this->isCluster = isCluster;
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

void
Node::NodeSetIsTop(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_TOP;
  NodeSetFlags(flags);
}

void
Node::NodeClearIsTop(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_TOP;
  flags = flags ^ NODE_FLAG_IS_TOP;
  NodeSetFlags(flags);
}

void
Node::NodeSetIsInvalid(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_INVALID;
  NodeSetFlags(flags);
}

void 
Node::NodeClearIsInvalid(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_INVALID;
  flags = flags ^ NODE_FLAG_IS_INVALID;
  NodeSetFlags(flags);
}

void
Node::NodeSetIsClusterChild(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_CLUSTER_CHILD;
  NodeSetFlags(flags);
}

void
Node::NodeClearIsClusterChild(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_CLUSTER_CHILD;
  flags = flags ^ NODE_FLAG_IS_CLUSTER_CHILD;
  NodeSetFlags(flags);
}

void
Node::NodeSetIsClusterParent(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_CLUSTER_PARENT;
  NodeSetFlags(flags);
}

void
Node::NodeClearIsClusterParent(void)
{
  unsigned int flags;
  
  flags = NodeGetFlags();
  flags = flags | NODE_FLAG_IS_CLUSTER_PARENT;
  flags = flags ^ NODE_FLAG_IS_CLUSTER_PARENT;
  NodeSetFlags(flags);
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

Node::~Node()
{

}
