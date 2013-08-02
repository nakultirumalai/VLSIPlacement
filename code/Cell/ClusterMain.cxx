# include <Cell.h>

Cluster::Cluster()
{
  ClusterSetNumCells(0);
  ClusterSetBCellsPlaced(false);
  ClusterSetPlacementTime(0);
  ClusterSetRowBased(false);
}

Cluster::Cluster(vector<Cell *> &cellsOfCluster)
{
  ClusterSetNumCells(cellsOfCluster.size());
  ClusterSetCellsOfCluster(cellsOfCluster);
  ClusterSetBCellsPlaced(false);
  ClusterSetPlacementTime(0);
  ClusterSetRowBased(false);
}

Cluster::Cluster(vector<Cell *> &cellsOfCluster, 
		 vector<uint> &BCellIndices,
		 vector<uint> &rowNum,
		 vector<uint> &xPosInRow,
		 map<Pin *, Pin *> &pinMap)
{
  ClusterSetNumCells(cellsOfCluster.size());
  ClusterSetCellsOfCluster(cellsOfCluster);
  ClusterSetBCellIndices(BCellIndices);
  ClusterSetRowNums(rowNum);
  ClusterSetXPosInRows(xPosInRow);
  ClusterSetPinMap(pinMap);
  ClusterSetBCellsPlaced(false);
  ClusterSetPlacementTime(0);
  ClusterSetRowBased(false);
}

Cluster::~Cluster()
{

}

void
Cluster::ClusterSetNumCells(uint numCells)
{
  this->numCells = numCells;
}

void
Cluster::ClusterSetCellsOfCluster(vector<Cell *> &cellsOfCluster)
{
  this->cellsOfCluster = cellsOfCluster;
}

void
Cluster::ClusterSetBoundaryCells(map<Cell *, uint> &boundaryCells)
{
  this->boundaryCells = boundaryCells;
}

void
Cluster::ClusterSetInternalNets(vector<Net *> &internalNets) 
{
  this->internalNets = internalNets;
}

void
Cluster::ClusterSetExternalNets(vector<Net *> &externalNets) 
{
  this->externalNets = externalNets;
}

void
Cluster::ClusterSetAffectedNets(vector<Net *> &affectedNets) 
{
  this->affectedNets = affectedNets;
}

void
Cluster::ClusterSetBCellIndices(vector<uint> &bCellIndices)
{
  this->BCellIndices = bCellIndices;
}

void
Cluster::ClusterSetRowNums(vector<uint> &rowNum)
{
  this->rowNum = rowNum;
}

void
Cluster::ClusterSetXPosInRows(vector<uint> &xPosInRow)
{
  this->xPosInRow = xPosInRow;
}

void
Cluster::ClusterSetPinMap(map<Pin*, Pin*> &pinMap)
{
  this->pinMap = pinMap;
}

void
Cluster::ClusterSetBCellsPlaced(bool bCellsPlaced)
{
  this->bCellsPlaced = bCellsPlaced;
}

void
Cluster::ClusterSetRowBased(bool clusterRowBased)
{
  this->rowBased = clusterRowBased;
}

void
Cluster::ClusterSetClusterLevel(char clusterLevel)
{
  this->clusterLevel = clusterLevel;
}

void
Cluster::ClusterSetCellArea(double clusterCellArea)
{
  this->clusterCellArea = clusterCellArea;
}

void
Cluster::ClusterSetPlacementTime(double clusterPlacementTime)
{
  this->clusterPlacementTime = clusterPlacementTime;
}

void
Cluster::ClusterSetNumBoundaryCells(uint numBoundaryCells)
{
  this->numBoundaryCells = numBoundaryCells;
}

void
Cluster::ClusterSetCellPositions(vector<vector<double> > &cellPositions)
{
  this->cellPositions = cellPositions;
}

void
Cluster::ClusterSetDimensions(vector<pair<uint, uint> > &dimensions)
{
  this->dimensions = dimensions;
}

void
Cluster::ClusterSetHeightVariations(vector<double> &heightVariations)
{
  this->heightVariations = heightVariations;
}

void
Cluster::ClusterSetShapeHPWL(vector<double> &shapeHPWL)
{
  this->shapeHPWL = shapeHPWL;
}

uint
Cluster::ClusterGetNumCells(void)
{
  return (this->numCells);
}

vector<Cell *>&
Cluster::ClusterGetCellsOfCluster(void)
{
  return (this->cellsOfCluster);
}

map<Cell *, uint>&
Cluster::ClusterGetBoundaryCells(void)
{
  return (this->boundaryCells);
}

vector<Net *> &
Cluster::ClusterGetInternalNets(void) 
{
  return (this->internalNets);
}

vector<Net *> &
Cluster::ClusterGetExternalNets(void) 
{
  return (this->externalNets);
}

vector<Net *> &
Cluster::ClusterGetAffectedNets(void) 
{
  return (this->affectedNets);
}

vector<uint> &
Cluster::ClusterGetBCellIndices(void)
{
  return (this->BCellIndices);
}

vector<uint> &
Cluster::ClusterGetRowNums(void)
{
  return (this->rowNum);
}

vector<uint> &
Cluster::ClusterGetXPosInRows(void)
{
  return (this->xPosInRow);
}

map<Pin*, Pin*>&
Cluster::ClusterGetPinMap(void)
{
  return (this->pinMap);
}

bool
Cluster::ClusterGetBCellsPlaced(void)
{
  return (this->bCellsPlaced);
}

bool
Cluster::ClusterGetRowBased(void)
{
  return (this->rowBased);
}

char
Cluster::ClusterGetClusterLevel(void)
{
  return (this->clusterLevel);
}

double
Cluster::ClusterGetCellArea(void)
{
  return (this->clusterCellArea);
}

double
Cluster::ClusterGetPlacementTime(void)
{
  return (this->clusterPlacementTime);
}

uint
Cluster::ClusterGetNumBoundaryCells(void)
{
  return (this->numBoundaryCells);
}

vector<vector<double> >&
Cluster::ClusterGetCellPositions(void)
{
  return (this->cellPositions);
}

vector<pair<uint, uint> >&
Cluster::ClusterGetDimensions(void)
{
  return (this->dimensions);
}

vector<double>&
Cluster::ClusterGetHeightVariations(void)
{
  return (this->heightVariations);
}

vector<double>&
Cluster::ClusterGetShapeHPWL(void)
{
  return (this->shapeHPWL);
}

void
Cluster::PlotCluster(void)
{

}

void
Cluster::PrintCluster(void)
{
  uint numCells, numBCells;
  uint idx;

  vector<Cell *> &cellsOfCluster = (*this).ClusterGetCellsOfCluster();
  vector<Net *> &internalNets = (*this).ClusterGetInternalNets();
  vector<uint> &rowNums = (*this).ClusterGetRowNums();
  vector<uint> &xposInRows = (*this).ClusterGetXPosInRows();
  vector<uint> &bCells = (*this).ClusterGetBCellIndices();

  numCells = cellsOfCluster.size();
  numBCells = bCells.size();

  cout << "*********************************************" << endl;
  cout << "Printing cluster information: " << endl;
  cout << "Number of cells:" << cellsOfCluster.size() << endl;
  cout << "Number of internal nets: " << internalNets.size() << endl;
  cout << "Number of boundary cells: " << bCells.size() << endl;
  cout << endl;
  if (xposInRows.size() != bCells.size()) {
    _ASSERT_TRUE("Error: Size of vector which stores xposition in rows not equal to size of boundary cell indices");
  } 
  if (rowNums.size() != bCells.size()) {
    _ASSERT_TRUE("Error: Size of vector which stores row numbers is not equal to size of boundary cell indices");
  }
  cout << "Printing row numbers and xpos of cells: " << endl;
  for (idx = 0; idx < numBCells; idx++) {
    cout << "Boundary Cell Name: " << (*cellsOfCluster[idx]).CellGetName()
	 << "  Row number: " << rowNums[idx] 
	 << "  X-position in row: " << xposInRows[idx] << endl;
  }
  cout << "*********************************************" << endl;
}
