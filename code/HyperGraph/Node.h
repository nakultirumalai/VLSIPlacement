# ifndef NODE_H
# define NODE_H

# include <common.h>

using namespace std;

class Edge;
typedef enum edgeType {
  IN_EDGE,
  OUT_EDGE
} edgeType;

class Node {
 private:
  /* Indicate attributes on the nodes */
  unsigned char flags;
  unsigned int numInEdges;
  unsigned int numOutEdges;
  unsigned int nodeIdx;
  void *data;
  vector<Edge *>inEdges;
  vector<Edge *>outEdges;

 public:
  void* NodeGetData(void);
  unsigned int NodeGetIdx(void);
  unsigned char NodeGetNumInEdges(void);
  unsigned char NodeGetNumOutEdges(void);
  unsigned char NodeGetNumEdges(void);
  vector<Node*>& NodeGetAdjacentNodes(void);

  /* Set functions */
  void NodeSetData(void *);
  void NodeSetIdx(unsigned int);
  void NodeAddEdge(Edge *);
  void NodeAddEdge(Edge *, edgeType);

  /* Constructor */
  Node();
  Node(void *);
  /* Destructor */
  ~Node();
};

# endif 
