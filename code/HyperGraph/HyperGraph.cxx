# include <HyperGraph.h>
# include <Cell.h>

void HyperGraph::AddEdge(vector<void *> &cellList, void *EdgeObject)
{
  void *object;
  Edge *newEdge = new Edge();
  Node *ExistingNode;

  VECTOR_FOR_ALL_CELLS(cellList, void*, object) {
    ExistingNode = nodeList[(void*)object];
    if (ExistingNode == NULL) {
      ExistingNode = new Node(object);
      AddNodeInt(object, ExistingNode);
    }
    (*newEdge).EdgeAddNode(ExistingNode);
    (*newEdge).EdgeSetObject(EdgeObject);
    (*ExistingNode).NodeAddEdge(newEdge);
  } END_FOR;
  numEdges++;
}

void HyperGraph::AddNode(void *object)
{
  Node *newNode;

  newNode = nodeList[(void*)object];
  if (newNode == NULL) {
    newNode = new Node(object);
    AddNodeInt(object, newNode);
  }
}

void HyperGraph::AddNodeInt(void *object, Node *newNode)
{
  nodeList[object] = newNode;
  numNodes++;
}

unsigned int HyperGraph::GetNumNodes(void)
{
  return (numNodes);
}

unsigned int HyperGraph::GetNumEdges(void)
{
  return (numEdges);
}

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

vector<Node *> Node::NodeGetAdjacentNodes(void)
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

Edge::Edge() 
{
}

HyperGraph::HyperGraph() 
{
  numEdges = 0;
}
