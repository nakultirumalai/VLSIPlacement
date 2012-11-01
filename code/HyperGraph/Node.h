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
  /* Indicates the number of IN edges */
  unsigned int numInEdges;
  /* Indicates the number of OUT edges */
  unsigned int numOutEdges;
  /* Indicates the index of the node. Required 
     as nodes are referred to by indices in order to 
     save space and lookup time */
  unsigned int nodeIdx;
  /* To annotate index on a node 
     for traversal purposes */
  unsigned int annIdx1;
  unsigned int annIdx2;
  unsigned int annIdx3;
  
  /* Actual data that corresponds to the given node */
  void *data;

 public:
  /* List of indices of all the IN Edges */
  vector<unsigned int>inEdges;
  /* List of indices of all the OUT Edges */
  vector<unsigned int>outEdges;

  /* Get functions */
  void* NodeGetData(void);
  unsigned int NodeGetIdx(void);
  unsigned int NodeGetFlags(void);
  unsigned int NodeGetAnnIdx1(void);
  unsigned int NodeGetAnnIdx2(void);
  unsigned int NodeGetAnnIdx3(void);
  unsigned int NodeGetNumInEdges(void);
  unsigned int NodeGetNumOutEdges(void);
  unsigned int NodeGetNumEdges(void);

  /* Set functions */
  void NodeSetData(void *);
  void NodeSetIdx(unsigned int);
  void NodeSetFlags(unsigned int);
  void NodeSetAnnIdx1(unsigned int);
  void NodeSetAnnIdx2(unsigned int);
  void NodeSetAnnIdx3(unsigned int);
  void NodeSetNumInEdges(unsigned int);
  void NodeSetNumOutEdges(unsigned int);
  
  /* Global functions to add an Edge to 
     a Node */
  void NodeAddEdge(unsigned int);
  void NodeAddEdge(unsigned int, edgeType);

  /* Constructors */
  Node();
  Node(void *);

  /* Destructors */
  ~Node();
};

# endif 
