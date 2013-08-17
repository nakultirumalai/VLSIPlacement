# include <FDP.h>

FDPNet::FDPNet()
{
  FDPNetSetFirstCell(NIL(Cell*));
  FDPNetSetSecondCell(NIL(Cell*));
  FDPNetSetWeight(0);
}

FDPNet::FDPNet(Cell *firstCell, Cell *secondCell) 
{
  FDPNetSetFirstCell(firstCell);
  FDPNetSetSecondCell(secondCell);
  FDPNetSetWeight(0);
}

FDPNet::FDPNet(Cell *firstCell, Cell *secondCell, double weight) 
{
  FDPNetSetFirstCell(firstCell);
  FDPNetSetSecondCell(secondCell);
  FDPNetSetWeight(weight);
}

FDPNet::~FDPNet()
{

}

void
FDPNet::FDPNetSetFirstCell(Cell *thisCell)
{
  firstCell = thisCell;
}

void
FDPNet::FDPNetSetSecondCell(Cell *thisCell)
{
  secondCell = thisCell;
}

void 
FDPNet::FDPNetSetWeight(double weight)
{
  this->weight = weight;
}

double
FDPNet::FDPNetGetWeight(void)
{
  return weight;
}

Cell*
FDPNet::FDPNetGetOtherCell(Cell *thisCell)
{
  Cell *rtv;
  
  rtv = NIL(Cell*);
  if (thisCell == firstCell) {
    rtv = secondCell;
  } else if (thisCell == secondCell) {
    rtv = firstCell;
  } else {
    _ASSERT_TRUE("Error: FDPNet has neither the first cell or the second cell equal to \"thisCell\"");
  }
  
  return (rtv);
}
