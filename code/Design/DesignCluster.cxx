# include <Design.h>

void 
Design::DesignClusterCells(vector<Cell*>& CellSet)
{
  vector<Net &> NetSet;
  map<string, bool> NetHash;
  Pin CellPin;
  string NetName;
  int numAffectedNets;

  /* Remove all cells present in the vector into a map 
     and create a complex cell name. 
     Along with that, iterate over all the pins of each
     cell and get the nets corresponding to each cell */
  for (int idx=0;idx<CellSet.size();idx++) {
    BEGIN_PINS_OF_CELL_ITER(*(CellSet[idx]), PIN_DIR_ALL, CellPin) {
      Net& ConnectedNet = CellPin.PinGetNet();
      NetName = ConnectedNet.NetGetName();
      if (NetHash[NetName] == true) {
	continue;
      } else {
	NetHash[NetName] == true;
	NetSet[numAffectedNets] = ConnectedNet;
      }
    } END_ITER;
  }
}


void
Design::DesignCollapseCluster(Cell& MasterCell)
{

}


