# ifndef HYPERGRAPH_H
# define HYPERGRAPH_H

# include <common.h>
# include <Node.h>
# include <Edge.h>

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
