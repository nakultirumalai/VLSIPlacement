# ifndef EDGE_H
# define EDGE_H

# include <common.h>

using namespace std;

class Node;

class Edge {
 private:
  void *object;
  unsigned int edgeIdx;
  double weight;
 public:
  vector<unsigned int> nodeSet;

  void EdgeAddNode(unsigned int);
  void *EdgeGetObject(void);

  double EdgeGetWeight(void);
  vector<unsigned int>& EdgeGetNodes(void);

  void EdgeSetObject(void *object);
  void EdgeSetIdx(unsigned int);
  void EdgeSetWeight(double);

  Edge();
  Edge(void *);
  ~Edge();
};

# endif 
