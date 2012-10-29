# ifndef HYPERGRAPH_H
# define HYPERGRAPH_H

# include <common.h>

/**********************************************
How this package should work:
**********************************************/
using namespace std;
typedef enum edgeType {
  IN_EDGE, 
  OUT_EDGE
} edgeType;

class Edge;

class Node {
 private:
  /* Indicate attributes on the nodes */
  unsigned char flags;
  /* Number of in and out edges */
  unsigned int numInEdges;
  unsigned int numOutEdges;
  /* Data contained or represented by the node */
  void *data;
  /* For node traversal */
  unsigned int nodeIdx;
  vector<Edge *>inEdges;
  vector<Edge *>outEdges;

 public:
  /* Get functions */
  /* Get the data corresponding to the node */
  void* NodeGetData(void);
  /* Node traversal get functions */
  unsigned int NodeGetIdx(void);
  /* Get the number of edges that terminate from the node */
  unsigned char NodeGetNumInEdges(void);
  /* Get the number of edges that emanate from the node*/
  unsigned char NodeGetNumOutEdges(void);
  /* Get total number of edges around the node */
  unsigned char NodeGetNumEdges(void);
  /* Get all the nodes adjacent to the current node */
  vector<Node*> NodeGetAdjacentNodes(void);

  /* Set functions */
  void NodeSetData(void *);
  void NodeAddEdge(Edge *);
  void NodeAddEdge(Edge *, edgeType);
  /* Constructor */
  Node();
  Node(void *);
  ~Node();
};

class Edge {
 private:
  void *object;
 public:
  vector<Node*>nodeSet;
  void EdgeAddNode(Node *);
  void EdgeSetObject(void *object);
  void *EdgeGetObject(void);

  Edge();
  ~Edge();
};

class HyperGraph {
 private:
  unsigned int numNodes;
  unsigned int numEdges;
  map<void*, Node*>nodeList;
  vector<Edge*>edgeList;
  /* Internal function for inserting the 
     Node * type into the graph directly */
  void AddNodeInt(void *, Node *);

 public:
  /* Insertion routines */
  void AddEdge(vector<void *>&, void *);
  void AddNode(void *);

  unsigned int GetNumNodes(void);
  unsigned int GetNumEdges(void);

  HyperGraph();
  ~HyperGraph();
};

# endif 
