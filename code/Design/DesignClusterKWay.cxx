# include <Design.h>

/* 
   Function to write fixed nodes of the design into 
   the khmetis format 
*/
void
DesignWriteHGraphFixFile(HyperGraph &myGraph)
{

}

/* 
   Function to write the hypergraph file for khmetis 
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
				    uint numClusters)
{
  Cell *thisCell;
  ifstream partOutputFile;
  uint nodeIdx, partIdx, numNodes;
  vector<vector<Cell *> > clusters;
  string fileName, line;
  
  fileName = graphFileName + ".part." + getStrFromInt(numClusters);
  partOutputFile.open(fileName.data());
  
  for (partIdx = 0; partIdx < numClusters; partIdx++) {
    clusters.push_back(vector<Cell *> ());
  }

  nodeIdx = 1;
  numNodes = myGraph.GetNumNodes();
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
  DesignFormClusters(clusters);
}

void
Design::DesignDoKWayClustering(HyperGraph &myGraph)
{
  uint numCells, numClusters;
  uint UBfactor, Nruns, CType, OType, VCycle, dbglvl;
  string graphFileName, designName, opFileName;
  Env &DesignEnv = DesignGetEnv();

  designName = DesignGetName();
  graphFileName = designName + ".khmetis";
  
  numCells = DesignGetNumCells();
  //  numClusters = 20 + (((double)numCells) / 1000) * 
  numClusters = DesignEnv.EnvGetNumClusters(); // Number for k-way partitioning 
  UBfactor = DesignEnv.EnvGetImbalanceFactor();
  Nruns = DesignEnv.EnvGetNumKHmetisRuns();
  CType = 2;
  OType = 1;
  VCycle = 3;
  dbglvl = 24;
  opFileName = graphFileName + ".part." + getStrFromInt(numClusters);
  if (!fileExists(opFileName)) {
    DesignWriteHGraphFile(myGraph, numClusters, graphFileName, designName);
    DesignRunKHMetis(graphFileName, numClusters, UBfactor, Nruns, CType, 
		     OType, VCycle, dbglvl);
  }
  DesignReadAndCreateClusters(myGraph, graphFileName, numClusters);
}
