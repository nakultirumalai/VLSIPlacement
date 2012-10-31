# ifndef EDGE_H
# define EDGE_H

# include <common.h>

using namespace std;

class Node;

class Edge {
 private:
  void *object;
  unsigned int edgeIdx;
 public:
  vector<Node*>nodeSet;
  void EdgeAddNode(Node *);
  void *EdgeGetObject(void);
  vector<Node*>& EdgeGetNodes(void);

  void EdgeSetObject(void *object);
  void EdgeSetIdx(unsigned int);
  Edge();
  Edge(void *);
  ~Edge();
};

# endif 
