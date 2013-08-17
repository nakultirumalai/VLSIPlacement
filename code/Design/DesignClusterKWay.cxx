# include <Design.h>

/* 
   Function to write fixed nodes of the design into 
   the khmetis format 
*/
void
DesignWriteHGraphFixFile(HyperGraph &myGraph)
{

}

inline void
getKHmetisRunTime(string logFileName, double &clusteringTime)
{
  ifstream ifile;
  string line, garbage;
  string timeStr;
  string intSubStr;
  bool foundCPU;
  int secPos;

  clusteringTime = 0;
  ifile.open(logFileName.data());

  foundCPU = false;
  while (!ifile.eof()) {
    getline(ifile, line);
    if (line.find("  Partitioning Time:") == 0) {
      istringstream stream(line, istringstream::in);
      stream >> garbage;
      stream >> garbage;
      stream >> timeStr;
      secPos = timeStr.find("sec");
      if (secPos == string::npos) {
	_ASSERT_TRUE("No time stamp found in khmetis log");
      }
      intSubStr = timeStr.substr(0, (secPos - 0));
      istringstream inpStream(intSubStr, istringstream::in);
      inpStream >> clusteringTime;
      break;
    }
  }
  ifile.close();
}

void
Design::DesignPartitionKWayHmetis(HyperGraph &myGraph, int nRuns, int nParts, int UbFactor, int *part,
				  bool recursiveBiPartition, bool useNodeWeights, bool useEdgeWeights, 
				  double &partitioningTime)
{
  /* H-metis interface variables */
  int nvtxs, nhedges;
  int *vwgts;
  int *eptr;
  int *eind;
  int *hewgts;
  int options[9];
  int edgecut;
  
  /* Other variables */
  vector<int> allEptrVec, allEindVec;
  vector<int> eptrVec, eindVec;
  double normCellWidth;
  uint numEind, numEptr;
  uint nodeIdx, cellWidth;
  uint idx, maxWidth;

  /* Get the maximum width of the cells in the design 
     for normalization */
  maxWidth = DesignGetMaxCellWidth();

  /* Get the number of nodes and edges in the graph */
  nvtxs = (int)myGraph.GetNumNodes();
  nhedges = (int)myGraph.GetNumEdges();
  
  /* 
  /* Create node weights */
  vwgts = NIL(int *);
  if (useNodeWeights) {
    vwgts = new int[nvtxs];
    for (nodeIdx = 0; nodeIdx < nvtxs; nodeIdx++) {
      Cell &thisCell = *((Cell *)myGraph.GetNodeObject(nodeIdx));
      cellWidth = thisCell.CellGetWidth();
      normCellWidth = ceil(((double)cellWidth) / (maxWidth) * 100);
      vwgts[nodeIdx] = normCellWidth;
    }
  }
  
  /* Read edges and populate eptr and eind */
  //  if (useEdgeWeights) {
  hewgts = NIL(int *);
  //  }
  allEptrVec.push_back(allEindVec.size());
  for (idx = 0; idx < nhedges; idx++) {
    eindVec.clear();
    eptrVec.clear();
    HYPERGRAPH_FOR_ALL_NODES_OF_EDGE(myGraph, idx, nodeIdx) {
      Cell &thisCell = *((Cell *)myGraph.GetNodeObject(nodeIdx));
      eindVec.push_back(nodeIdx);
    } HYPERGRAPH_END_FOR;
    allEindVec.insert(allEindVec.end(), eindVec.begin(), eindVec.end());
    allEptrVec.push_back(allEindVec.size());    
  }
  numEind = allEindVec.size();
  numEptr = allEptrVec.size();
  eptr = new int[numEptr];
  eind = new int[numEind];
  for (idx = 0; idx < numEind; idx++) eind[idx] = allEindVec[idx];
  for (idx = 0; idx < numEptr; idx++) eptr[idx] = allEptrVec[idx];  
  
  /* Build the option values here */
  /* OPTION IDX 0: 0 indicates that default values should be used and 1 
     enables use of other options */
  options[0] = 1;

  /* OPTION IDX 1: Nruns parameter */
  options[1] = nRuns;

  /* OPTION IDX 2: Scheme to be used for grouping vertices during the 
     coarsening phase */
  /*          Value = 1: Hybrid first choice scheme                            */
  /*          Value = 2: First-choice scheme                                   */
  /*          Value = 3: Greedy first-choice scheme                            */
  /*          Value = 4: Hyperedge coarsening (HEC) scheme                     */
  /*          Value = 5: Edge Coarsening: Vertices are grouped together if     */
  /*	                 they are connected by multiple hyperEdges             */
  options[2] = 1;

  if (recursiveBiPartition) {
    /* OPTION IDX 3: Determines the refinement scheme employed */
    /*          Value = 1: Fiduccia-Mattheyses(FM) refinement scheme.          */
    /*          Value = 2: One-way FM refinement scheme                        */
    /*          Value = 3: Early exit FM refinement scheme                     */
    options[3] = 1;
  } else {
    /* OPTION IDX 3: Determines the objective: */
    /*          Value = 1: Default. Minimize the hyper edge cut                */
    /*          Value = 2: Minimize the sum of external degrees                */
    options[3] = 2;
  }
  
  /* OPTION IDX 4: Determine the scheme used for V-cycle refinement */
  /*          Value = 0: Do not perform any V-cycle refinement                 */
  /*          Value = 1: Perform refinement on the final solution of each step */
  /*          Value = 2: Refinement on each intermediate solution whose        */
  /*	                 quality is equally good or better than the best found */
  /*                     so far                                                */
  /*          Value = 3: Refinement on each intermediate solution              */
  options[4] = 1;

  /* OPTION IDX 5: Determine the scheme to be used for reconstructing hyperedges 
                   during recursive bisection. */
  options[5] = 1;
  
  /* OPTION IDX 6: Determines whether or not there are sets of vertices that 
                   need to be pre-assigned to certain partitions. A value of 
                   0 indicates that no pre-assignment is needed, whereas 1 
                   indicates that there are  sets of vertices that need to be 
		   pre-assigned */
  options[6] = 0;
  
  /* OPTION IDX 7: Determines the random seed to be used to initialize the 
                   random number generator. Any value here is good */
  options[7] = 5;

  /* OPTION IDX 8: Determines the level of debugging information to be 
                   printed by hmetis. Default value is 0 */
  options[8] = 24;
  if (recursiveBiPartition) {
    //    HMETIS_PartRecursive(nvtxs, nhedges, vwgts, eptr, eind, hewgts, nParts,
    //       			 UbFactor, options, part, &edgecut);
  } else {
    //    HMETIS_PartKway(nvtxs, nhedges, vwgts, eptr, eind, hewgts, nParts,
    //       		    UbFactor, options, part, &edgecut);
  }
  
}
/* Function to write the hypergraph file for khmetis 
   The format is as follows:
   Line 1: <Number of edges> <Number of vertices> <fmt=1, 10, 11>
   Line 2: <List of HyperEdges starts here: Weight followed by list of nodes>
   Line 3: <List of nodes weights>
 */
void
Design::DesignWriteHGraphFile(HyperGraph &myGraph, uint numClusters, 
			      string graphFileName, string designName)
{
  ofstream opFile;
  string edgeOutput;
  uint maxWidth, minWidth;
  uint minCellWidth, idx;
  uint numNodes, numEdges;
  uint numNodesOfEdge, nodeIdx;
  uint cellWidth, normCellWidth;

  opFile.open(graphFileName.data());
  numNodes = myGraph.GetNumNodes();
  numEdges = myGraph.GetNumEdges();
  maxWidth = DesignGetMaxCellWidth();

  //  minWidth = DesignGetMinCellWidth();
  minWidth = 152;
  opFile << "% HMETIS FILE WRITTEN FOR " << numClusters << "-way partitioning" 
	 << " of design " << designName << endl;

  /* Write out the number of edges, number of nodes and fmt value */
  opFile << numEdges << " " << numNodes << " " << endl;
  //  opFile << "11" << endl;

  /* Write out all the edges */
  for (idx = 0; idx < numEdges; idx++) {
    numNodesOfEdge = 0;
    edgeOutput = "";
    HYPERGRAPH_FOR_ALL_NODES_OF_EDGE(myGraph, idx, nodeIdx) {
      Cell &thisCell = *((Cell *)myGraph.GetNodeObject(nodeIdx));
      //      if (thisCell.CellIsTerminal()) continue;
      //      edgeOutput += " " + getStrFromInt(nodeIdx + 1);
      edgeOutput += getStrFromInt(nodeIdx + 1) + " ";
      numNodesOfEdge++;
    } HYPERGRAPH_END_FOR;
    if (numNodesOfEdge > 1) {
      //      opFile << "1 ";
      opFile << edgeOutput;
      opFile << endl;
    }
  }

  /* Write out all the nodes */
  if (1) {
    for (nodeIdx = 0; nodeIdx < numNodes; nodeIdx++) {
      Cell &thisCell = *((Cell *)myGraph.GetNodeObject(nodeIdx));
      if (thisCell.CellIsTerminal()) cellWidth = 152;
      else cellWidth = thisCell.CellGetWidth();
      normCellWidth = ceil(((double)cellWidth) / (maxWidth) * 100);
      opFile << normCellWidth << " " << endl;
    }
  }
  opFile.close();
}

void
Design::DesignReadAndCreateClusters(HyperGraph &myGraph, string graphFileName, 
				    int *part, uint numClusters)
{
  Cell *thisCell;
  double clusterCreationTime;
  ifstream partOutputFile;
  uint nodeIdx, partIdx, numNodes;
  vector<vector<Cell *> > clusters;
  string fileName, line;
  
  numNodes = myGraph.GetNumNodes();
  for (partIdx = 0; partIdx < numClusters; partIdx++) {
    clusters.push_back(vector<Cell *> ());
  }
  if (part == NIL(int *)) {
    fileName = graphFileName + ".part." + getStrFromInt(numClusters);
    partOutputFile.open(fileName.data());
    for (nodeIdx = 0; nodeIdx < numNodes; nodeIdx++) {
      if (partOutputFile.eof()) {
	break;
      }
      //    getline(partOutputFile, line);
      //    istringstream stream(line, istringstream::in);
      partOutputFile >> partIdx;
      thisCell = (Cell *)myGraph.GetNodeObject(nodeIdx);
      //    cout << "CELL: " << (*thisCell).CellGetName() << endl;
      if ((*thisCell).CellIsTerminal()) {
	//      cout << "Skipping Cell: " << (*thisCell).CellGetName() << " as it is a terminal " << endl;
	continue;
      }
      //    cout << "Reading Cell: " << (*thisCell).CellGetName() << " into partition " << partIdx << endl;
      //    cout << "Node Idx: " << nodeIdx << " Partition: " << partIdx << endl;
      (clusters[partIdx]).push_back(thisCell);
    }
    partOutputFile.close();
  } else {
    for (nodeIdx = 0; nodeIdx < numNodes; nodeIdx++) {
      partIdx = part[nodeIdx];
      thisCell = (Cell *)myGraph.GetNodeObject(nodeIdx);
      if ((*thisCell).CellIsTerminal()) {
	continue;
      }
      clusters[partIdx].push_back(thisCell);
    }
  }
  DesignFormClusters(clusters);
}

void
Design::DesignDoKWayClustering(HyperGraph &myGraph, bool useExec)
{
  int *part;
  double clusterPlacementTime, partitioningTime;
  uint numCells, numClusters, numNodes, nVcycles;
  uint UBfactor, Nruns, CType, OType, VCycle, dbglvl;
  string cType, rnType, vType, oType, rType;
  string graphFileName, designName, opFileName;
  string logFileName;
  bool recursiveBiPartition;

  Env &DesignEnv = DesignGetEnv();
  numNodes = myGraph.GetNumNodes();
  designName = DesignGetName();
  graphFileName = designName + ".khmetis";
  part = NIL(int *);
  numCells = DesignGetNumCells();
  //  numClusters = 20 + (((double)numCells) / 1000) * 
  numClusters = DesignEnv.EnvGetNumClusters(); // Number for k-way partitioning 
  UBfactor = DesignEnv.EnvGetImbalanceFactor();
  Nruns = DesignEnv.EnvGetNumKHmetisRuns();
  recursiveBiPartition = DesignEnv.EnvGetRecursiveBiPartitioning();
  CType = 2; 
  OType = 1; 
  VCycle = 3;
  dbglvl = 24;
  /* These are string variables for the hmetis2.0 engine */
  cType = "h1";
  if (recursiveBiPartition) {
    rType = "slow";
  } else {
    rType = "kpslow"; 
  }
  oType = "soed";
  nVcycles = 4;

  partitioningTime = 0;
  if (useExec) {
    opFileName = graphFileName + ".part." + getStrFromInt(numClusters);
    //    logFileName = designName + "_KHmetisLog";
    logFileName = designName + "_KHmetis2Log";
    if (!(fileExists(opFileName) && fileExists(logFileName))) {
      DesignWriteHGraphFile(myGraph, numClusters, graphFileName, designName);
      //      DesignRunKHMetis(graphFileName, numClusters, UBfactor, Nruns, CType, 
      //		       OType, VCycle, dbglvl);
      DesignRunKHMetis2(graphFileName, recursiveBiPartition, rType, cType, oType,
			UBfactor, Nruns, nVcycles, numClusters, dbglvl);
    }
    getKHmetisRunTime(logFileName, partitioningTime);
  } else {
    part = new int[numNodes];
    DesignPartitionKWayHmetis(myGraph, Nruns, numClusters, UBfactor, part,
			      recursiveBiPartition, true, false, 
			      partitioningTime);
  }
  DesignEnv.EnvRecordKWayPartitioningTime(partitioningTime);
  DesignReadAndCreateClusters(myGraph, graphFileName, part, numClusters);
}
