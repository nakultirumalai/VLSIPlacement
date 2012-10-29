# include <HyperGraph.h>
# include <Cell.h>

/* HyperGraph functions */
void HyperGraph::AddNodeInt(void *object, Node *newNode)
{
  nodeList[object] = newNode;
  numNodes++;
}

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

unsigned int HyperGraph::GetNumNodes(void)
{
  return (numNodes);
}

unsigned int HyperGraph::GetNumEdges(void)
{
  return (numEdges);
}

HyperGraph::HyperGraph() 
{
  numEdges = 0;
}

