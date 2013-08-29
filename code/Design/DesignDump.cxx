# include <Design.h>

inline void
getNeighborClusterSize(Cell *clusterCell, HyperGraph &myGraph, double &minArea,
                       double &maxArea, uint &numConnectedClusters)
{
  Cell *connectedCluster;
  void *connectedNode;
  vector<void *> connectedNodes;
  double clusterHeight, clusterWidth, clusterArea;
  uint clusterNodeIdx;

  connectedNodes = myGraph.GetConnectedNodes((void*)clusterCell);
  minArea = DBL_MAX;
  maxArea = 0;
  numConnectedClusters = 0;
  VECTOR_FOR_ALL_ELEMS(connectedNodes, void *, connectedNode) {
    connectedCluster = (Cell *)connectedNode;
    if ((*connectedCluster).CellIsPort()) {
      continue;
    }
    clusterHeight = (*connectedCluster).CellGetHeight();
    clusterWidth = (*connectedCluster).CellGetWidth();
    clusterArea = clusterHeight * clusterWidth;
    if (clusterArea > maxArea) maxArea = clusterArea;
    if (clusterArea < minArea) minArea = clusterArea;
    numConnectedClusters++;
  } END_FOR;
}

inline uint
getNumSequentialCells(vector<Cell *> &cellsOfCluster)
{
  Cell *thisCell;
  uint seqCellCount;

  seqCellCount = 0;
  VECTOR_FOR_ALL_ELEMS(cellsOfCluster, Cell *, thisCell) {
    if ((*thisCell).CellIsSequential()) {
      seqCellCount++;
    }
  } END_FOR;

  return (seqCellCount);
}

inline
double getOverLap(double L1, double R1, double L2, double R2)
{
  double left, right;
  double overlap;

  right = R2;
  if (R1 < R2) right = R1;
  left = L2;
  if (L1 > L2) left = L1;

  overlap = (int)(right - left);
  if (overlap < 0) overlap = 0.0;

  return (overlap);
}

inline void
getTotalClusterOverLap(Design &myDesign, double &totalOverlap, double &peakOverlap,
                       double &percentageOverlap)
{
  Cell *clust1, *clust2;
  map<string, Cell *> DesignClustersCopy = myDesign.DesignGetClusters();
  map<string, Cell *> &DesignClusters = myDesign.DesignGetClusters();
  map<Cell *, bool> visitedNodes;
  string cellName1, cellName2;
  double cellLeft1, cellRight1, cellTop1, cellBot1;
  double cellLeft2, cellRight2, cellTop2, cellBot2;
  double overlapx, overlapy;
  double overlapArea, chipArea;
  uint maxx, maxy;

  totalOverlap = 0;
  peakOverlap = 0;
  percentageOverlap = 0;
  MAP_FOR_ALL_ELEMS(DesignClustersCopy, string, Cell *, cellName1, clust1) {
    overlapArea = 0;
    cellLeft1 = (*clust1).CellGetXpos();
    cellRight1 = cellLeft1 + (*clust1).CellGetWidth();
    cellBot1 = (*clust1).CellGetYpos();
    cellTop1 = cellBot1 + (*clust1).CellGetHeight();
    _KEY_DOES_NOT_EXIST(visitedNodes, clust1) {
      visitedNodes[clust1] = 0;
    }
    MAP_FOR_ALL_ELEMS(DesignClusters, string, Cell *, cellName2, clust2) {
      _KEY_EXISTS(visitedNodes, clust2) {
        continue;
      }
      cellLeft2 = (*clust2).CellGetXpos();
      cellRight2 = cellLeft2 + (*clust2).CellGetWidth();
      cellBot2 = (*clust2).CellGetYpos();
      cellTop2 = cellBot2 + (*clust2).CellGetHeight();
      overlapx = getOverLap(cellLeft1, cellRight1, cellLeft2, cellRight2);
      overlapy = getOverLap(cellBot1, cellTop1, cellBot2, cellTop2);
      overlapArea += overlapx * overlapy;
    } END_FOR;
    totalOverlap += overlapArea;
    if (overlapArea > peakOverlap) {
      peakOverlap = overlapArea;
    }
  } END_FOR;

  myDesign.DesignGetBoundingBox(maxx, maxy);
  chipArea = ((double)maxx) * maxy;
  percentageOverlap = (totalOverlap / chipArea) * 100;
}

/* FUNCTION TO DUMP THE FOLLOWING INFORMATION FOR EACH CLUSTER:
   - TOTAL AREA 
   - TOTAL ROWS
   - TOTAL SITES
   - TOTAL NUMBER OF INTERNAL NETS
   - TOTAL NUMBER OF BOUNDARY CELLS
   - TOTAL NUMBER OF CLUSTERS THIS CLUSTER CONNECTS TO
*/
void
Design::DesignDumpClusterInfo(string fileName)
{
  Cluster *clusterOfCell;
  Cell *clusterCellPtr;
  string cellName;
  uint numBoundaryCells, numClusters;
  uint maxx, maxy;
  double chipArea;
  double clusterArea, cellArea, totalClusterArea;
  double clusterHeight, clusterWidth;
  double clusterPlacementTime, totalClusterPlacementTime;
  double minNeighborArea, maxNeighborArea;
  uint numConnectedClusters, numCells, numSeqCells;
  double totalOverlap, peakOverlap, percentOverlap;
  ulong totalInternalHPWL;
  ulong totalClusterHeight, totalClusterWidth;
  double averageClusterWidth, averageClusterHeight;
  ofstream opFile;
  HyperGraph &myGraph = DesignGetGraph();
  Env &DesignEnv = DesignGetEnv();

  opFile.open(fileName.data());
  //  opFile << "# ClusterName\tcellArea\tclusterArea\tNumInternalNets\tNumBoundaryCells\tNumOtherClusterConnections" << endl; 
  opFile << "#ClusterName\tNumCells\tNumSeqCells\tCellArea\tClusterArea\tNumInternalNets\tNumBoundCells\t";
  opFile << "NumConnClusters\tLargestConnClustArea\tSmallestConnClustArea\tWirelength\tPlacementTime\t" << endl;
  totalClusterHeight = 0;
  totalClusterWidth = 0;
  numClusters = 0;
  clusterPlacementTime = 0;
  totalClusterPlacementTime = 0;
  totalClusterArea = 0;
  totalInternalHPWL = 0;

  DESIGN_FOR_ALL_CLUSTERS((*this), cellName, clusterCellPtr) {
    clusterOfCell = (Cluster *)CellGetCluster(clusterCellPtr);
    getNeighborClusterSize(clusterCellPtr, myGraph, minNeighborArea, maxNeighborArea,
                           numConnectedClusters);
    vector<Net *> &internalNets = (*clusterOfCell).ClusterGetInternalNets();
    vector<Cell *> &cellsOfCluster = (*clusterOfCell).ClusterGetCellsOfCluster();
    vector<double> &shapeHPWL = (*clusterOfCell).ClusterGetShapeHPWL();
    numCells = cellsOfCluster.size();
    numSeqCells = getNumSequentialCells(cellsOfCluster);
    numBoundaryCells = (*clusterOfCell).ClusterGetNumBoundaryCells();
    cellArea = (*clusterOfCell).ClusterGetCellArea();
    clusterHeight = (*clusterCellPtr).CellGetHeight();
    clusterWidth = (*clusterCellPtr).CellGetWidth();
    clusterArea = clusterHeight * clusterWidth;
    totalClusterArea += clusterArea;
    totalClusterHeight += clusterHeight;
    totalClusterWidth += clusterWidth;
    clusterPlacementTime = (*clusterOfCell).ClusterGetPlacementTime();
    totalClusterPlacementTime += clusterPlacementTime;
    totalInternalHPWL += shapeHPWL[shapeHPWL.size() - 1];
    opFile << cellName << "\t"
           << numCells << "\t"
           << numSeqCells << "\t"
           << cellArea << "\t"
           << clusterArea << "\t"
           << internalNets.size() << "\t"
           << numBoundaryCells << "\t"
           << numConnectedClusters << "\t"
           << maxNeighborArea << "\t"
           << minNeighborArea << "\t"
           << shapeHPWL[shapeHPWL.size() - 1] << "\t"
           << clusterPlacementTime << "\t"
           << endl;
    numClusters++;
  } DESIGN_END_FOR;

  DesignEnv.EnvSetHPWLTotalInternal(totalInternalHPWL);
  DesignGetBoundingBox(maxx, maxy);
  chipArea = ((double)maxx) * maxy;
  averageClusterWidth = ((double)totalClusterWidth) / numClusters;
  averageClusterHeight = ((double)totalClusterHeight) / numClusters;
  DesignSetAverageClusterCellWidth(averageClusterWidth);
  DesignSetAverageClusterCellHeight(averageClusterHeight);
  getTotalClusterOverLap((*this), totalOverlap, peakOverlap, percentOverlap);
  cout << "Total cluster placement time : " << totalClusterPlacementTime << endl;
  cout << "       Average cluster height: " << averageClusterHeight << endl;
  cout << "       Average cluster width : " << averageClusterWidth << endl;
  cout << "                Cluster Area : " << totalClusterArea << endl;
  cout << "                   Chip Area : " << chipArea << endl;
  cout << "      Total overlapping area : " << totalOverlap << endl;
  cout << " Percentage overlapping area : " << percentOverlap << endl;
  cout << "       Peak overlapping area : " << peakOverlap << endl;
  cout << "           Final Utilization : " << ((double)totalClusterArea)/chipArea << endl;
  opFile.close();
}

void
Design::DesignDumpNetDegreeProfile(string fileName)
{
  Net *netPtr;
  Pin *pinPtr;
  string netName;
  uint netNumPins, numNets;
  uint xHPWL, yHPWL;
  ulong totalHPWL;
  ofstream opFile;
  map<uint, uint> netProfile;
  map<uint, ulong> netDegreeHPWL;
  map<uint, uint>::iterator itr1;
  map<uint, ulong>::iterator itr2;

  DESIGN_FOR_ALL_NETS((*this), netName, netPtr) {
    vector<Pin *> &pinsVec = (*netPtr).NetGetAllPinsVector();
    netNumPins = 0;
    VECTOR_FOR_ALL_ELEMS(pinsVec, Pin*, pinPtr) {
      if ((*pinPtr).isHidden) {
	continue;
      }
      netNumPins++;
    } END_FOR;
    (*netPtr).NetComputeHPWL(xHPWL, yHPWL);
    _KEY_EXISTS_WITH_VAL(netProfile, netNumPins, itr1) {
      itr1->second += 1;
    } else {
      netProfile[netNumPins] = 1;
    }
    _KEY_EXISTS_WITH_VAL(netDegreeHPWL, netNumPins, itr2) {
      itr2->second += (xHPWL + yHPWL);
    } else {
      netDegreeHPWL[netNumPins] = (xHPWL + yHPWL);
    }
  } DESIGN_END_FOR;
  
  opFile.open(fileName.data());
  opFile << "# NumPins\t NumNets\t TotHPWLContrib " << endl;
  MAP_FOR_ALL_ELEMS(netProfile, uint, uint, netNumPins, numNets) {
    totalHPWL = netDegreeHPWL[netNumPins];
    opFile << " " << netNumPins << "\t " << numNets << "\t " << totalHPWL << endl;
  } END_FOR;
  opFile.close();
}
