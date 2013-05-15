# include <TimingModels.h>

/*******************************************************************************
   Builds the objective function to be minimized in three vectors:
   subi_vec: i of quadratic matrix, subj_vec: j of quadratic matrix
   val_vec: value of aij of quadratic matrix
   Builds only the lower left triangle of the objective matrix
*******************************************************************************/
void
getObjectiveHybridModel(Design &myDesign, HyperGraph &myGraph,
			vector<Cell*> &inputCells, vector<uint> &subi_vec, 
			vector<uint> &subj_vec,	vector<double> &valij_vec, 
			vector<uint> &sub_vec, vector<double> &val_vec, 
			double& constant)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtri, *pinPtrj;
  double edgeWeight, coeffX, coeffY;
  double xOffi, yOffi, xOffj, yOffj;
  double celljx, celljy, dOffx, dOffy;
  uint numCells, numVars, edgeIdx;
  uint cellIdxi, cellIdxj;
  uint i, j;
  map<Cell *, uint> cellLookupMap;
  map<uint, map<uint, double> > nonSquaredTerms;

  numCells = inputCells.size();
  numVars = 2 * numCells;

  /* Get the cell lookup map */
  cellLookupMap = getCellLookupMap(inputCells);

  /* Initialize the constant to 0 */
  constant = 0.0;

  /* Iterate over all HyperEdges of the graph */
  HYPERGRAPH_FOR_ALL_EDGES(myGraph, edgeIdx, edgeWeight) {
    map<Pin *, bool> visitedPins;

    /* Get the related logical for the hyperedge */
    Net &relatedNet = *(Net *)myGraph.GetEdgeObject(edgeIdx);

    /* Begin iteration over all pins */
    NET_FOR_ALL_PINS(relatedNet, pinPtri) {
      Cell &celli = (*pinPtri).PinGetParentCell();
      cellPtri = &celli;
      /* Skip terminal cells as they do not have a variable 
	 associated with them */
      if (celli.CellIsTerminal()) continue;
      /* Get the index of the cell */
      cellIdxi = cellLookupMap[cellPtri];

      /* Mark the pin as visited */
      visitedPins[pinPtri] = true;
      NET_FOR_ALL_PINS(relatedNet, pinPtrj) {
	/* Skip the visited pins for clique type traversal */
	_KEY_EXISTS(visitedPins, pinPtrj) continue;

	Cell &cellj = (*pinPtrj).PinGetParentCell();
	cellPtrj = &cellj;
	cellIdxj = cellLookupMap[cellPtrj];

	unsigned int idxi, idxj;
	idxi = cellIdxi; idxj = cellIdxj;

	/* If cellIdxi < cellIdxj, swap */
	if (cellIdxi < cellIdxj) swap(idxi, idxj);
	
	/* Insert value for x-variable */
	subi_vec.push_back(idxj); subj_vec.push_back(idxj); valij_vec.push_back((-0.5 * coeffX));
	/* Insert value for y-variable */
	subi_vec.push_back(idxi+numCells); subj_vec.push_back(idxj+numCells); valij_vec.push_back((-0.5 * coeffY));
	
	/* Take care of inserting value for the linear terms as well */
	xOffi = ((double)(*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	yOffi = ((double)(*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO;
	xOffj = ((double)(*pinPtrj).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	yOffj = ((double)(*pinPtrj).PinGetYOffset()) / GRID_COMPACTION_RATIO;
	celljx = ((double)(cellj.CellGetXpos())) / GRID_COMPACTION_RATIO; 
	celljy = ((double)(cellj.CellGetYpos())) / GRID_COMPACTION_RATIO;

	if (cellj.CellIsTerminal()) {
	  dOffx = xOffi - celljx;
	  dOffy = yOffi - celljy;
	} else {
	  dOffx = xOffi - xOffj;
	  dOffy = yOffi - yOffj;
	  sub_vec.push_back(cellIdxj); val_vec.push_back(-(dOffx * coeffX));
	  sub_vec.push_back(cellIdxj + numCells); val_vec.push_back(-(dOffy * coeffY));
	}
	sub_vec.push_back(cellIdxi); val_vec.push_back(dOffx * coeffX);
	sub_vec.push_back(cellIdxi + numCells); val_vec.push_back(dOffy * coeffY);
	constant += (coeffX * dOffx * dOffx) + (coeffY * dOffy * dOffy);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
}
