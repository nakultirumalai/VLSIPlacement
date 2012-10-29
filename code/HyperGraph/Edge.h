# ifndef EDGE_H
# define EDGE_H

# include <common.h>

using namespace std;

class Node;

class Edge {
 private:
  void *object;
 public:
  vector<Node*>nodeSet;
  void EdgeAddNode(Node *);
  void EdgeSetObject(void *object);
  void *EdgeGetObject(void);
  vector<Node*>& EdgeGetNodes(void);

  Edge();
  ~Edge();
};

# endif 
