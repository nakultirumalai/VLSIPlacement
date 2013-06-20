# ifndef NODE_H
# define NODE_H

# include <common.h>

/* Flags definition begin */
# define NODE_FLAG_CLEAR 0x0
# define NODE_FLAG_IS_TOP 0x1
# define NODE_FLAG_IS_CLUSTER_CHILD 0x1 << 1
# define NODE_FLAG_IS_CLUSTER_PARENT 0x1 << 2
# define NODE_FLAG_IS_INVALID 0x2 << 3

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
  /* Boolean corresponding to whether the node is a cluster or not */
  bool isCluster;
  /* Actual data that corresponds to the given node */
  void *data;

 public:
  /* List of indices of all the IN Edges */
  vector<unsigned int>inEdges;
  /* List of indices of all the OUT Edges */
  vector<unsigned int>outEdges;

  /* Get functions */
  /* Check if node is a cluster */
  bool NodeIsCluster(void);
  /* Get the data corresponding to the node */
  void* NodeGetData(void);
  /* Get the index corresponding to the node */
  unsigned int NodeGetIdx(void);
  /* Get the flags field corresponding to the node */
  unsigned int NodeGetFlags(void);
  /* Get the annotated indices of the node */
  unsigned int NodeGetAnnIdx1(void);
  unsigned int NodeGetAnnIdx2(void);
  unsigned int NodeGetAnnIdx3(void);
  /* Get the number of edges incident on the node */
  unsigned int NodeGetNumInEdges(void);
  /* Get the number of edges that are emerging outward 
     from the node */
  unsigned int NodeGetNumOutEdges(void);
  /* Get the total number of edges incident and emerging 
     out of the node */
  unsigned int NodeGetNumEdges(void);
  /* Check if node is the top level node */
  bool NodeIsTop(void);
  /* Check if node is a child of a cluster */
  bool NodeIsClusterChild(void);
  /* Check if node is a parent of a cluster */
  bool NodeIsClusterParent(void);
  /* Check if node is invalid */
  bool NodeIsInvalid(void);

  /* Set functions */
  /* Set if a node is a cluster or not */
  void NodeSetIsCluster(bool);
  /* Set the data field of the node */
  void NodeSetData(void *);
  /* Set the index of the node */
  void NodeSetIdx(unsigned int);
  /* Set the flags field of the node */
  void NodeSetFlags(unsigned int);
  /* Set the annotated index on the node */
  void NodeSetAnnIdx1(unsigned int);
  void NodeSetAnnIdx2(unsigned int);
  void NodeSetAnnIdx3(unsigned int);
  /* Set the number of edges incident on the node 
     Only to be used by constructor for resetting */
  void NodeSetNumInEdges(unsigned int);
  /* Set the number of edges emerging from the node 
     Only to be used by constructor for resetting */
  void NodeSetNumOutEdges(unsigned int);

  /* Set a flag that the node is the top level node */
  void NodeSetIsTop(void);
  /* Clear the flag that indicates node is top level */
  void NodeClearIsTop(void);

  /* Set a flag to indicate that the node is invalid: Required for BestChoice clustering */
  void NodeSetIsInvalid(void);
  /* Clear the flag that indicates that the node is invalid: Required for BestChoice clustering */
  void NodeClearIsInvalid(void);

  /* Set a flag that the node is a child of a cluster */
  void NodeSetIsClusterChild(void);
  /* Clear the flag that indicates node is child of a cluster */
  void NodeClearIsClusterChild(void);

  /* Set a flag that the node is a parent of a cluster */
  void NodeSetIsClusterParent(void);
  /* Clear the flag that indicates node is parent of a cluster */
  void NodeClearIsClusterParent(void);

  /* Global functions to add the index of the edge 
     that the node is connected to */
  void NodeAddEdge(unsigned int);
  /* Global functions to add the index of the edge 
     that the node is connected to with edge type */
  void NodeAddEdge(unsigned int, edgeType);

  /* Constructors */
  Node();
  Node(void *);

  /* Destructors */
  ~Node();
};

# endif 
