# ifndef HYPERGRAPH_H
# define HYPERGRAPH_H

# include <common.h>
# include <Node.h>
# include <Edge.h>

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

  vector<vector<int> > Edges;
  vector<vector<pair<int,int> > > Connectivity;
  vector<unsigned int> nodeConnectivity;

  /* Graph update variable: Everytime a node or an edge is added, 
     this variable indicates if the graph is dirty or not */
  bool dirtyGraph;

  /* Internal function for inserting the 
     Node * type into the graph directly */
  unsigned int AddNodeInt(void *);
  unsigned int AddEdgeInt(void *, unsigned int);
  unsigned int getNodeConnectivity(unsigned int);
  unsigned int getEdgeWeight(unsigned int);
  void setDirty(void);
  void clearDirty(void);
 public:
  /* Set functions */
  void HyperGraphSetNumNodes(unsinged int);
  void HyperGraphSetNumEdges(unsinged int);
  
  /* Insertion routines */
  void HyperGraphAddNode(void *);
  void HyperGraphAddEdge(vector<void *>&, void *, 
			 unsigned int);

  /* Cluster routines */
  void HyperGraphClusterNodes(vector<vector<unsigned int > > nodesSet);
  void HyperGraphUnclusterNodes(void);

  /* Get routines */
  unsigned int HyperGraphGetNumNodes(void);
  unsigned int HyperGraphGetNumEdges(void);

  /* Support for iterators */
  vector<unsigned int>& HyperGraphGetNodes(void);
  vector<unsigned int>& HyperGraphGetEdges(void);

  HyperGraph();
  ~HyperGraph();
};

# endif 
