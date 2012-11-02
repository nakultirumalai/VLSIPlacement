# ifndef EDGE_H
# define EDGE_H

# include <common.h>

using namespace std;

class Node;

class Edge {
 private:
  void *object;
  unsigned int edgeIdx;
  unsigned int weight;
 public:
  vector<unsigned int> nodeSet;

  void EdgeAddNode(unsigned int);
  void *EdgeGetObject(void);

  unsigned int EdgeGetWeight(void);
  vector<unsigned int>& EdgeGetNodes(void);

  void EdgeSetObject(void *object);
  void EdgeSetIdx(unsigned int);
  void EdgeSetWeight(unsigned int);

  Edge();
  Edge(void *);
  ~Edge();
};

# endif 
