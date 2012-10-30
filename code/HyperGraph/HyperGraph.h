# ifndef HYPERGRAPH_H
# define HYPERGRAPH_H

# include <common.h>
# include <Node.h>
# include <Edge.h>

class pair {
 private:
  unsigned int int1;
  unsigned int int2;
 public:
  pair();
  ~pair();
};

class HyperGraph {
 private:
  /* Fastest lookup can come when nodes are indexed
     with numbers. This makes the data structures light
     and also fast */
  /* If the nodes are to be indexed with numbers, a map
     between indices and actual nodes is required */
  /* A map between the actual object and its node representation
     is required */
  /* An edge is a set of nodes. Should edges simply be a vector of
     vectors? Edges can also be indexed */
  unsigned int numNodes;
  unsigned int numEdges;

  /* Map from index to actual node. 
     Space complexity = O(n) where n is the number of nodes*/
  map<unsigned int, Node *> idx2Node;
  /* Map from object to node index.
     Space complexity = O(n) where n is the number of nodes*/     
  map<void *, unsigned int> obj2idx;
  /* Map from index to edges.
     space complexity O(n2)*/
  map<unsigned int, Edge *> idx2Edge;
  /* Array of edges. Each element is another array of 
     node indices */
  vector<vector<int>> Edges;
  /* Array of a pair. Each pair is made of
     node index, edge index */
  vector<vector<pair>> Connectivity;

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
