# ifndef HYPERGRAPH_H
# define HYPERGRAPH_H

# include <common.h>
# include <Node.h>
# include <Edge.h>
# include <algorithm>
# include <HyperGraphIter.h>

typedef std::map<uint, std::vector<uint > > mapOfVectors;

/*******************************************************************************
  FLAGS DEFINED FOR NODES AND EDGES 
*******************************************************************************/
/* EDGE FLAGS */
# define EDGE_FLAG_HIDDEN 0x1 

/* NODE FLAGS */
# define NODE_FLAG_IS_TOP 0x1
# define NODE_FLAG_IS_CLUSTER 0x1 << 1

class HyperGraph {
 private:
  /* Variable to indicate if the older clusters which were absorbed into 
     newer clusters need to be erased */
  bool eraseClusters;
  /* Number of nodes in the hypergraph */
  uint numNodes;
  /* Number of edges in the hypergraph */
  uint numEdges;

  /* Map of a list of the nodes that have been marked as 
     non-Top */
  map<uint, uint> nonTopNodes;

  /* Map from index to actual node. 
     Space complexity = O(n) where n is the number of nodes*/
  vector<Node *> idx2Node;
  /* Map from object to node index.
     Space complexity = O(n) where n is the number of nodes*/     
  map<void *, uint> obj2idx;
  /* Map from index to edges. space complexity O(n2)*/
  vector<Edge *> idx2Edge;

  /* Array whose indices are Edges and values are 
     other arrays whose elements are indices of nodes
     that are connected to the edges */
  vector<map<uint, uint > > Edges2Nodes;

  /* Array whose indices are Nodes and value is another
     array whose elements are connected edge indices */
  vector<vector<uint> > Nodes2Edges;

  /* Vector of flags for edges */
  vector<char> edgeFlags;
  /* Vector of flags for nodes */
  vector<char> nodeFlags;

  /* Private functions begin */
  /* Functions for adding nodes and edges */
  uint AddNodeInt(void *);
  uint AddEdgeInt(void *, double);

  /* Functions for hiding edges */
  void HideEdge(uint);

  /* Functions for removing nodes and edges */
  uint RemoveEdge(uint);
  uint RemoveNode(uint);

  /* Get all the node indices that are connected to the specified
     node index */
  vector<uint> GetConnectedNodeIndices(uint);

  /* Function to check, set and clear if a node index represents a top level 
     node. This is used for hiding nodes when the netlist is clustered */
  void NodeSetIsTop(uint);
  void NodeClearIsTop(uint);

 public:
  /* Routine to insert a node into the hypergraph */
  void AddNode(void *);
  /* Node specific functions */
  void* GetNodeObject(uint);
  int GetObjIndex(void *);
  bool NodeIsTop(uint);
  /* Function to check if a node is marked invalid or not: Required for 
     best choice clustering */
  void NodeSetIsInvalid(uint);
  void NodeClearIsInvalid(uint);
  bool NodeIsInvalid(uint);
  /* Routine to insert an edge into the hypergraph */
  uint AddEdge(vector<void *>&, void *, double);
  /* Edge specific functions */
  double GetEdgeWeight(uint);
  void SetEdgeWeight(uint, double);
  void* GetEdgeObject(uint);
  bool EdgeIsHidden(uint);

  /* Cluster routines */
  bool ClusterNodes(vector<void *> &, void*, map<void *, bool> &,
		    map<void *, bool> &);

  /* Get routines */
  uint GetNumNodes(void);
  uint GetNumEdges(void);

  /* Get the number of nodes on an edge */
  uint EdgeGetNumNodes(uint);
  /* Get the number of edges on the node */
  uint NodeGetNumEdges(uint);

  /* Routines to support Iterators */
  map<uint, uint> GetNodesOfEdge(uint);
  vector<uint> GetEdgesOfObject(void *);
  vector<uint> GetEdgesOfObjects(void *, void *);
  vector<uint> GetEdgesOfNode(uint);
  vector<uint> GetEdgesOfNodes(uint, uint);

  /* Get the connected nodes of an object in the graph */
  vector<void *> GetConnectedNodes(void*);
  /* Check if two objects are adjacent in the graph. If they are,
     return the edge index which connects them. Else return -1 */
  long int NodesAreAdjacent(void*, void*);

  /* Special functions in case the graph only has two edges */
  void* GetOtherNodeOfEdge(uint edgeIdx, void *);

  /* Clustering test functions */
  void testClustering(void);
  
  /* Section for adding attributes on nodes */
  /* Any attribute can be added to indicate properties of nodes 
     or edges */
  /* Property of node representing a cluster */
  void NodeSetIsCluster(uint, bool);
  bool NodeIsCluster(uint); 

  /* Debug routines to print the edges and nodes of the graph */
  void PrintGraph(void);
  void PrintSubGraph(vector<uint> &nodeSet);
  void PrintEdge(uint);
  void PrintNode(uint);

  /* Return the number of non top nodes */
  uint GetNumNonTopNodes(void);

  /* Constructor and Destructor */
  HyperGraph();
  ~HyperGraph();

  /* Debug routines */
  Edge* GetEdge(uint);
  Node* GetNode(uint);
};

# endif 

