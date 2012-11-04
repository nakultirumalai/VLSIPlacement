# ifndef HYPERGRAPH_H
# define HYPERGRAPH_H

# include <common.h>
# include <Node.h>
# include <Edge.h>
# include <algorithm>

typedef std::map<unsigned int, std::vector<unsigned int > > mapOfVectors;

class HyperGraph {
 private:
  /* Number of nodes in the hypergraph */
  unsigned int numNodes;
  /* Number of edges in the hypergraph */
  unsigned int numEdges;

  /* Map from index to actual node. 
     Space complexity = O(n) where n is the number of nodes*/
  map<unsigned int, Node *> idx2Node;
  /* Map from object to node index.
     Space complexity = O(n) where n is the number of nodes*/     
  map<void *, unsigned int> obj2idx;
  /* Map from index to edges. space complexity O(n2)*/
  map<unsigned int, Edge *> idx2Edge;
  /* Array whose indices are Edges and values are 
     other arrays whose elements are indices of nodes
     that are connected to the edges */
  vector<map<unsigned int, unsigned int > > Edges2Nodes;
  /* Array whose indices are Nodes and value is another
     array whose elements are connected edge indices */
  vector<vector<unsigned int> > Nodes2Edges;
  /* Array whose indices are nodes and elements are a metric which 
     represents the total connectivity of the node in the graph */
  vector<unsigned int> nodeConnectivity;
  /* Graph update variable: Everytime a node or an edge is added, 
     this variable indicates if the graph is dirty or not. If the
     graph is dirty, perform some action to update the connectivity
     information */
  map<unsigned int, mapOfVectors> ClusterInfo;
  bool dirtyGraph;

  /* Private functions begin */
  /* Internal function for inserting the Node* type into 
     the hypergraph */
  unsigned int AddNodeInt();
  unsigned int AddNodeInt(void *);

  /* Internal function for inserting the Edge* type into 
     the hypergraph */
  unsigned int AddEdgeInt(void *, unsigned int);
  /* Yet to be defined */
  unsigned int getNodeConnectivity(unsigned int);
  /* Given an edge index, return the weight of the edge */
  unsigned int getEdgeWeight(unsigned int);

  /* Gets the index at which a new node can be inserted into 
     the graph */
  unsigned int getNewNodeIdx(void);
  /* Gets the index at which a new edge can be created in
     the graph */
  unsigned int getNewEdgeIdx(void);

  /* Internal function to cluster a set of nodes */
  bool clusterNodes(vector<unsigned int> &);
  /* Internal function to uncluster */
  bool unclusterNode(unsigned int);

  /* Internal function to sort the data in the node array 
     also keeping graph data up-to-date is performed 
     by this function */
  void graphUpdate(void);
  /* Function to mark the graph as dirty */
  void setDirty(void);
  /* Function to clear the dirty flag on the graph */
  void clearDirty(void);

  /* Function to check, set and clear if a node index 
     represents a top level node */
  void nodeSetIsTop(unsigned int);
  void nodeClearIsTop(unsigned int);

  /* Function to check, set and clear if a node index 
     represents a child node of a cluster */
  void nodeSetIsClusterParent(unsigned int);
  void nodeClearIsClusterParent(unsigned int);

  /* Function to check, set and clear if a node index 
     represents a child node of a cluster */
  void nodeSetIsClusterChild(unsigned int);
  void nodeClearIsClusterChild(unsigned int);

 public:
  /* Insertion routines */
  void HyperGraphAddNode(void *);
  void HyperGraphAddEdge(vector<void *>&, void *, unsigned int);
  /* Cluster routines */
  bool HyperGraphClusterNodes(vector<vector<unsigned int > > nodesSet);
  bool HyperGraphUnclusterNodes(vector<unsigned int> nodeSet);
  /* Get routines */
  unsigned int HyperGraphGetNumNodes(void);
  unsigned int HyperGraphGetNumEdges(void);
  /* Support for iterators */
  vector<unsigned int>& HyperGraphGetNodes(void);
  vector<unsigned int>& HyperGraphGetEdges(void);
  /* Node properties check */
  bool nodeIsTop(unsigned int);
  bool nodeIsClusterParent(unsigned int);
  bool nodeIsClusterChild(unsigned int);

  HyperGraph();
  ~HyperGraph();

  /* Clustering test functions */
  void testClustering(void);
};

# endif 

