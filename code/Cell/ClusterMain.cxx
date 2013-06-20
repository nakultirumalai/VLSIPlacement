# include <Cell.h>

Cluster::Cluster()
{
  ClusterSetNumCells(0);
  ClusterSetBCellsPlaced(false);
}

Cluster::Cluster(vector<Cell *> &cellsOfCluster)
{
  ClusterSetNumCells(cellsOfCluster.size());
  ClusterSetCellsOfCluster(cellsOfCluster);
  ClusterSetBCellsPlaced(false);
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
Cluster::ClusterSetInternalNets(vector<Net *> &internalNets) 
{
  this->internalNets = internalNets;
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

vector<Net *> &
Cluster::ClusterGetInternalNets(void) 
{
  return (this->internalNets);
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
