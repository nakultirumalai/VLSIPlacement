# include <TimingModels.h>

/*******************************************************************************
   Builds the objective function to be minimized in three vectors:
   subivec: i of quadratic matrix, subj_vec: j of quadratic matrix
   val_vec: value of aij of quadratic matrix
   Builds only the lower left triangle of the objective matrix
*******************************************************************************/
void
getObjectiveCliqueModel(Design &myDesign, HyperGraph &myGraph,
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
  map<Cell *, uint> cellLookupMap;
  map<uint, uint> squaredTermsIndices;
  map<uint, uint> linearTermsIndices;
  map<uint, uint>::iterator itrUintUint;
  long cellIdxi, cellIdxj;
  uint numCells, numVars, edgeIdx;
  uint i, j;

  numCells = inputCells.size();
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
      coeffX = edgeWeight; coeffY = edgeWeight;
      /* Mark the pin as visited */
      visitedPins[pinPtri] = true;
      NET_FOR_ALL_PINS(relatedNet, pinPtrj) {
	Cell &cellj = (*pinPtrj).PinGetParentCell();
	cellPtrj = &cellj;
	if (cellPtrj == cellPtri) continue;
	/* Skip the visited pins for clique type traversal */
	_KEY_EXISTS(visitedPins, pinPtrj) continue;
	_KEY_EXISTS(cellLookupMap, cellPtrj) {
	  cellIdxj = cellLookupMap[cellPtrj];
	} else {
	  cellIdxj = -1;
	}
	_KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxi, itrUintUint) {
	  valij_vec[itrUintUint->second] += coeffX;
	} else {
	  subi_vec.push_back(cellIdxi); subj_vec.push_back(cellIdxi); valij_vec.push_back(coeffX);
	  squaredTermsIndices[cellIdxi] = (valij_vec.size() - 1);
	}
	_KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxi+numCells, itrUintUint) {
	  valij_vec[itrUintUint->second] += coeffY;
	} else {
	  subi_vec.push_back(cellIdxi+numCells); subj_vec.push_back(cellIdxi+numCells); valij_vec.push_back(coeffY);
	  squaredTermsIndices[cellIdxi+numCells] = (valij_vec.size() - 1);
	}
	if (cellIdxj > 0) {
	  _KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxj, itrUintUint) {
	    valij_vec[itrUintUint->second] += coeffX;
	  } else {
	    subi_vec.push_back(cellIdxj); subj_vec.push_back(cellIdxj); valij_vec.push_back(coeffX);
	    squaredTermsIndices[cellIdxj] = (valij_vec.size() - 1);
	  }
	  _KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxj+numCells, itrUintUint) {
	    valij_vec[itrUintUint->second] += coeffY;
	  } else {
	    subi_vec.push_back(cellIdxj+numCells); subj_vec.push_back(cellIdxj+numCells); valij_vec.push_back(coeffY);
	    squaredTermsIndices[cellIdxj+numCells] = (valij_vec.size() - 1);
	  }
	  unsigned int idxi, idxj;
	  idxi = cellIdxi; idxj = cellIdxj;
	  /* If cellIdxi < cellIdxj, swap */
	  if (cellIdxi < cellIdxj) swap(idxi, idxj);
	  /* No map handling is required for a pair of movable pins since 
	     each pair of movable pins is visited only once in the traversal*/
	  /* Insert value for x-variable */
	  subi_vec.push_back(idxi); subj_vec.push_back(idxj); valij_vec.push_back(-coeffX);
	  /* Insert value for y-variable */
	  subi_vec.push_back(idxi+numCells); subj_vec.push_back(idxj+numCells); valij_vec.push_back(-coeffY);
	}
	/* Take care of inserting value for the linear terms as well */
	xOffi = ((double)(*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	yOffi = ((double)(*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO;
	xOffj = ((double)(*pinPtrj).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	yOffj = ((double)(*pinPtrj).PinGetYOffset()) / GRID_COMPACTION_RATIO;
	if (cellj.CellIsTerminal()) {
	  celljx = ((double)(cellj.CellGetXpos())) / GRID_COMPACTION_RATIO; 
	  celljy = ((double)(cellj.CellGetYpos())) / GRID_COMPACTION_RATIO;
	  dOffx = xOffi - (celljx + xOffj);
	  dOffy = yOffi - (celljy + yOffj);
	} else {
	  dOffx = xOffi - xOffj;
	  dOffy = yOffi - yOffj;
	  _KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxj, itrUintUint) {
	    val_vec[itrUintUint->second] += -(dOffx * coeffX);
	  } else {
	    sub_vec.push_back(cellIdxj); val_vec.push_back(-(dOffx * coeffX));
	    linearTermsIndices[cellIdxj] = val_vec.size() - 1;
	  }
	  _KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxj+numCells, itrUintUint) {
	    val_vec[itrUintUint->second] += -(dOffy * coeffY);
	  } else {
	    sub_vec.push_back(cellIdxj + numCells); val_vec.push_back(-(dOffy * coeffY));
	    linearTermsIndices[cellIdxj + numCells] = val_vec.size() - 1;
	  }
	}
	_KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxi, itrUintUint) {
	  val_vec[itrUintUint->second] += dOffx * coeffX;
	} else {
	  sub_vec.push_back(cellIdxi); val_vec.push_back(dOffx * coeffX);
	  linearTermsIndices[cellIdxi] = val_vec.size() - 1;
	}
	_KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxi+numCells, itrUintUint) {
	  val_vec[itrUintUint->second] += dOffy * coeffY;
	} else {
	  sub_vec.push_back(cellIdxi + numCells); val_vec.push_back(dOffy * coeffY);
	  linearTermsIndices[cellIdxi + numCells] = val_vec.size() - 1;
	}
	constant += (coeffX * dOffx * dOffx) + (coeffY * dOffy * dOffy);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
}

/*******************************************************************************
   Builds the objective function for the x-variable to be minimized in three 
   vectors:
   subi_vec: i of quadratic matrix, 
   subj_vec: j of quadratic matrix
   val_vec: value of aij of quadratic matrix
   Builds only the lower left triangle of the objective matrix
*******************************************************************************/
void
getObjectiveCliqueModelX(Design &myDesign, HyperGraph &myGraph,
			 vector<Cell*> &inputCells, vector<uint> &subi_vec, 
			 vector<uint> &subj_vec, vector<double> &valij_vec, 
			 vector<uint> &sub_vec, vector<double> &val_vec, 
			 double& constant)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtri, *pinPtrj;
  double edgeWeight, coeffX;
  double xOffi, xOffj, celljx, dOffx;
  map<Cell *, uint> cellLookupMap;
  map<uint, uint> squaredTermsIndices;
  map<uint, uint> linearTermsIndices;
  map<uint, uint>::iterator itrUintUint;
  long cellIdxi, cellIdxj;
  uint numCells, edgeIdx;
  uint i, j;

  numCells = inputCells.size();
  /* Get the cell lookup map */
  cellLookupMap = getCellLookupMap(inputCells);
  /* Initialize the constant to 0 */
  constant = 0.0;
  /* Iterate over all HyperEdges of the graph */
  HYPERGRAPH_FOR_ALL_EDGES(myGraph, edgeIdx, edgeWeight) {
    map<Pin *, bool> visitedPins;
    /* Get the related logical for the hyperedge */
    Net &relatedNet = *(Net *)myGraph.GetEdgeObject(edgeIdx);
    edgeWeight = relatedNet.NetGetWeight();
    /* Begin iteration over all pins */
    NET_FOR_ALL_PINS(relatedNet, pinPtri) {
      Cell &celli = (*pinPtri).PinGetParentCell();
      cellPtri = &celli;
      /* Skip terminal cells as they do not have a variable associated with them */
      if (celli.CellIsTerminal()) continue;
      /* Get the index of the cell */
      cellIdxi = cellLookupMap[cellPtri];
      coeffX = edgeWeight;
      /* Mark the pin as visited */
      visitedPins[pinPtri] = true;
      NET_FOR_ALL_PINS(relatedNet, pinPtrj) {
	Cell &cellj = (*pinPtrj).PinGetParentCell();
	cellPtrj = &cellj;
	if (cellPtrj == cellPtri) continue;
	/* Skip the visited pins for clique type traversal */
	_KEY_EXISTS(visitedPins, pinPtrj) continue;
	_KEY_EXISTS(cellLookupMap, cellPtrj) cellIdxj = cellLookupMap[cellPtrj];
	else cellIdxj = -1;
	_KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxi, itrUintUint) {
	  valij_vec[itrUintUint->second] += coeffX;
	} else {
	  subi_vec.push_back(cellIdxi); subj_vec.push_back(cellIdxi); valij_vec.push_back(coeffX);
	  squaredTermsIndices[cellIdxi] = (valij_vec.size() - 1);
	}
	if (cellIdxj > 0) {
	  _KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxj, itrUintUint) {
	    valij_vec[itrUintUint->second] += coeffX;
	  } else {
	    subi_vec.push_back(cellIdxj); subj_vec.push_back(cellIdxj); valij_vec.push_back(coeffX);
	    squaredTermsIndices[cellIdxj] = (valij_vec.size() - 1);
	  }
	  unsigned int idxi, idxj;
	  idxi = cellIdxi; idxj = cellIdxj;
	  /* If cellIdxi < cellIdxj, swap */
	  if (cellIdxi < cellIdxj) swap(idxi, idxj);
	  /* No map handling is required for a pair of movable pins since 
	     each pair of movable pins is visited only once in the traversal*/
	  /* Insert value for x-variable */
	  subi_vec.push_back(idxi); subj_vec.push_back(idxj); valij_vec.push_back(-coeffX);
	}
	/* Take care of inserting value for the linear terms as well */
	xOffi = ((double)(*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	xOffj = ((double)(*pinPtrj).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	if (cellj.CellIsTerminal()) {
	  celljx = ((double)(cellj.CellGetXpos())) / GRID_COMPACTION_RATIO; 
	  dOffx = xOffi - (celljx + xOffj);
	} else {
	  dOffx = xOffi - xOffj;
	  _KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxj, itrUintUint) {
	    val_vec[itrUintUint->second] += -(dOffx * coeffX);
	  } else {
	    sub_vec.push_back(cellIdxj); val_vec.push_back(-(dOffx * coeffX));
	    linearTermsIndices[cellIdxj] = val_vec.size() - 1;
	  }
	}
	_KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxi, itrUintUint) {
	  val_vec[itrUintUint->second] += (dOffx * coeffX);
	} else {
	  sub_vec.push_back(cellIdxi); val_vec.push_back(dOffx * coeffX);
	  linearTermsIndices[cellIdxi] = val_vec.size() - 1;
	}
	constant += (coeffX * dOffx * dOffx);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
}

/*******************************************************************************
   Builds the objective function for the y-variable to be minimized in three 
   vectors:
   subi_vec: i of quadratic matrix, 
   subj_vec: j of quadratic matrix
   val_vec: value of aij of quadratic matrix
   Builds only the lower left triangle of the objective matrix
*******************************************************************************/
void
getObjectiveCliqueModelY(Design &myDesign, HyperGraph &myGraph,
			 vector<Cell*> &inputCells, vector<uint> &subi_vec, 
			 vector<uint> &subj_vec, vector<double> &valij_vec, 
			 vector<uint> &sub_vec, vector<double> &val_vec, 
			 double& constant)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtri, *pinPtrj;
  double edgeWeight, coeffY;
  double yOffi, yOffj, celljy, dOffy;
  map<Cell *, uint> cellLookupMap;
  map<uint, uint> squaredTermsIndices;
  map<uint, uint> linearTermsIndices;
  map<uint, uint>::iterator itrUintUint;
  long cellIdxi, cellIdxj;
  uint numCells, edgeIdx;
  uint i, j;

  numCells = inputCells.size();
  /* Get the cell lookup map */
  cellLookupMap = getCellLookupMap(inputCells);
  /* Initialize the constant to 0 */
  constant = 0.0;
  /* Iterate over all HyperEdges of the graph */
  HYPERGRAPH_FOR_ALL_EDGES(myGraph, edgeIdx, edgeWeight) {
    map<Pin *, bool> visitedPins;
    /* Get the related logical for the hyperedge */
    Net &relatedNet = *(Net *)myGraph.GetEdgeObject(edgeIdx);
    edgeWeight = relatedNet.NetGetWeight();
    /* Begin iteration over all pins */
    NET_FOR_ALL_PINS(relatedNet, pinPtri) {
      Cell &celli = (*pinPtri).PinGetParentCell();
      cellPtri = &celli;
      /* Skip terminal cells as they do not have a variable associated with them */
      if (celli.CellIsTerminal()) continue;
      /* Get the index of the cell */
      cellIdxi = cellLookupMap[cellPtri];
      coeffY = edgeWeight;
      /* Mark the pin as visited */
      visitedPins[pinPtri] = true;
      NET_FOR_ALL_PINS(relatedNet, pinPtrj) {
	Cell &cellj = (*pinPtrj).PinGetParentCell();
	cellPtrj = &cellj;
	if (cellPtri == cellPtrj) continue;
	/* Skip the visited pins for clique type traversal */
	_KEY_EXISTS(visitedPins, pinPtrj) continue;
	_KEY_EXISTS(cellLookupMap, cellPtrj) cellIdxj = cellLookupMap[cellPtrj];
	else cellIdxj = -1;
	_KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxi, itrUintUint) {
	  valij_vec[itrUintUint->second] += coeffY;
	} else {
	  subi_vec.push_back(cellIdxi); subj_vec.push_back(cellIdxi); valij_vec.push_back(coeffY);
	  squaredTermsIndices[cellIdxi] = (valij_vec.size() - 1);
	}
	if (cellIdxj > 0) {
	  _KEY_EXISTS_WITH_VAL(squaredTermsIndices, cellIdxj, itrUintUint) {
	    valij_vec[itrUintUint->second] += coeffY;
	  } else {
	    subi_vec.push_back(cellIdxj); subj_vec.push_back(cellIdxj); valij_vec.push_back(coeffY);
	    squaredTermsIndices[cellIdxj] = (valij_vec.size() - 1);
	  }
	  unsigned int idxi, idxj;
	  idxi = cellIdxi; idxj = cellIdxj;
	  /* If cellIdxi < cellIdxj, swap */
	  if (cellIdxi < cellIdxj) swap(idxi, idxj);
	  /* No map handling is required for a pair of movable pins since 
	     each pair of movable pins is visited only once in the traversal*/
	  /* Insert value for y-variable */
	  subi_vec.push_back(idxi); subj_vec.push_back(idxj); valij_vec.push_back(-coeffY);
	}
	/* Take care of inserting value for the linear terms as well */
	yOffi = ((double)(*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO; 
	yOffj = ((double)(*pinPtrj).PinGetYOffset()) / GRID_COMPACTION_RATIO; 
	if (cellj.CellIsTerminal()) {
	  celljy = ((double)(cellj.CellGetYpos())) / GRID_COMPACTION_RATIO; 
	  dOffy = yOffi - (celljy + yOffj);
	} else {
	  dOffy = yOffi - yOffj;
	  _KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxj, itrUintUint) {
	    val_vec[itrUintUint->second] += (-(dOffy * coeffY));
	  } else {
	    sub_vec.push_back(cellIdxj); val_vec.push_back(-(dOffy * coeffY));
	    linearTermsIndices[cellIdxj] = val_vec.size() - 1;
	  }
	}
	_KEY_EXISTS_WITH_VAL(linearTermsIndices, cellIdxi, itrUintUint) {
	  val_vec[itrUintUint->second] += (dOffy * coeffY);
	} else {
	  sub_vec.push_back(cellIdxi); val_vec.push_back(dOffy * coeffY);
	  linearTermsIndices[cellIdxi] = val_vec.size() - 1;
	}
	constant += (coeffY * dOffy * dOffy);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
}

void
getObjectiveCliqueModelXY(Design &myDesign, HyperGraph &myGraph,
			  vector<Cell*> &inputCells, vector<uint> &subi_vecx, 
			  vector<uint> &subj_vecx, vector<double> &valij_vecx, 
			  vector<uint> &sub_vecx, vector<double> &val_vecx, 
			  vector<uint> &subi_vecy, vector<uint> &subj_vecy, 
			  vector<double> &valij_vecy, vector<uint> &sub_vecy, 
			  vector<double> &val_vecy, double& constantx, 
			  double &constanty, map<Cell *, uint> &quadMap,
			  map<Cell *, uint> &linMap)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtri, *pinPtrj;
  double edgeWeight, coeffY, coeffX;
  double yOffi, yOffj, celljy, dOffy;
  double xOffi, xOffj, celljx, dOffx;
  map<Cell *, uint> cellLookupMap;
  map<uint, uint> squaredTermsMap;
  map<uint, uint> linearTermsMap;
  map<uint, uint>::iterator itrUintUint;
  long cellIdxi, cellIdxj;
  uint numCells, edgeIdx;
  uint i, j;

  numCells = inputCells.size();
  /* Get the cell lookup map */
  cellLookupMap = getCellLookupMap(inputCells);
  /* Initialize the constant to 0 */
  constantx = 0.0;
  /* Iterate over all HyperEdges of the graph */
  HYPERGRAPH_FOR_ALL_EDGES(myGraph, edgeIdx, edgeWeight) {
    map<Pin *, bool> visitedPins;
    /* Get the related logical for the hyperedge */
    Net &relatedNet = *(Net *)myGraph.GetEdgeObject(edgeIdx);
    edgeWeight = relatedNet.NetGetWeight();
    /* Begin iteration over all pins */
    /* Print out all pins of the net */
    NET_FOR_ALL_PINS(relatedNet, pinPtri) {
      Cell &celli = (*pinPtri).PinGetParentCell();
      cellPtri = &celli;
      /* Skip terminal cells as they do not have a variable associated with them */
      if (celli.CellIsTerminal()) continue;
      /* Get the index of the cell */
      cellIdxi = cellLookupMap[cellPtri];
      coeffX = edgeWeight;
      coeffY = edgeWeight;
      /* Mark the pin as visited */
      visitedPins[pinPtri] = true;
      NET_FOR_ALL_PINS(relatedNet, pinPtrj) {
	Cell &cellj = (*pinPtrj).PinGetParentCell();
	cellPtrj = &cellj;
	if (cellPtri == cellPtrj) continue;
	/* Skip the visited pins for clique type traversal */
	_KEY_EXISTS(visitedPins, pinPtrj) continue;
	_KEY_EXISTS(cellLookupMap, cellPtrj) cellIdxj = cellLookupMap[cellPtrj];
	else cellIdxj = -1;
	_KEY_EXISTS_WITH_VAL(squaredTermsMap, cellIdxi, itrUintUint) {
	  valij_vecx[itrUintUint->second] += coeffX;
	  valij_vecy[itrUintUint->second] += coeffY;
	} else {
	  subi_vecx.push_back(cellIdxi); subj_vecx.push_back(cellIdxi); valij_vecx.push_back(coeffX);
	  subi_vecy.push_back(cellIdxi); subj_vecy.push_back(cellIdxi); valij_vecy.push_back(coeffY);
	  squaredTermsMap[cellIdxi] = (valij_vecx.size() - 1);
	  quadMap[cellPtri] = (valij_vecx.size() - 1);
	}
	if (cellIdxj > 0) {
	  _KEY_EXISTS_WITH_VAL(squaredTermsMap, cellIdxj, itrUintUint) {
	    valij_vecx[itrUintUint->second] += coeffX;
	    valij_vecy[itrUintUint->second] += coeffY;
	  } else {
	    subi_vecx.push_back(cellIdxj); subj_vecx.push_back(cellIdxj); valij_vecx.push_back(coeffX);
	    subi_vecy.push_back(cellIdxj); subj_vecy.push_back(cellIdxj); valij_vecy.push_back(coeffY);
	    squaredTermsMap[cellIdxj] = (valij_vecx.size() - 1);
	    quadMap[cellPtrj] = (valij_vecx.size() - 1);
	  }
	  unsigned int idxi, idxj;
	  idxi = cellIdxi; idxj = cellIdxj;
	  /* If cellIdxi < cellIdxj, swap */
	  if (cellIdxi < cellIdxj) swap(idxi, idxj);
	  /* No map handling is required for a pair of movable pins since 
	     each pair of movable pins is visited only once in the traversal*/
	  /* Insert value for y-variable */
	  subi_vecx.push_back(idxi); subj_vecx.push_back(idxj); valij_vecx.push_back(-coeffX);
	  subi_vecy.push_back(idxi); subj_vecy.push_back(idxj); valij_vecy.push_back(-coeffY);
	}
	/* Take care of inserting value for the linear terms as well */
	xOffi = ((double)(*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	xOffj = ((double)(*pinPtrj).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	yOffi = ((double)(*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO; 
	yOffj = ((double)(*pinPtrj).PinGetYOffset()) / GRID_COMPACTION_RATIO; 

	if (cellj.CellIsTerminal()) {
	  celljx = ((double)(cellj.CellGetXpos())) / GRID_COMPACTION_RATIO; 
	  celljy = ((double)(cellj.CellGetYpos())) / GRID_COMPACTION_RATIO; 
	  dOffx = xOffi - (celljx + xOffj);
	  dOffy = yOffi - (celljy + yOffj);
	} else {
	  dOffy = yOffi - yOffj;
	  dOffx = xOffi - xOffj;
	  _KEY_EXISTS_WITH_VAL(linearTermsMap, cellIdxj, itrUintUint) {
	    val_vecx[itrUintUint->second] += (-(dOffx * coeffX));
	    val_vecy[itrUintUint->second] += (-(dOffy * coeffY));
	  } else {
	    sub_vecx.push_back(cellIdxj); val_vecx.push_back(-(dOffx * coeffX));
	    sub_vecy.push_back(cellIdxj); val_vecy.push_back(-(dOffy * coeffY));
	    linearTermsMap[cellIdxj] = val_vecx.size() - 1;
	    linMap[cellPtrj] = val_vecx.size() - 1;
	  }
	}
	_KEY_EXISTS_WITH_VAL(linearTermsMap, cellIdxi, itrUintUint) {
	  val_vecx[itrUintUint->second] += (dOffx * coeffX);
	  val_vecy[itrUintUint->second] += (dOffy * coeffY);
	} else {
	  sub_vecx.push_back(cellIdxi); val_vecx.push_back(dOffx * coeffX);
	  sub_vecy.push_back(cellIdxi); val_vecy.push_back(dOffy * coeffY);
	  linearTermsMap[cellIdxi] = val_vecx.size() - 1;
	  linMap[cellPtri] = val_vecx.size() - 1;
	}
	constantx += (coeffX * dOffx * dOffx);
	constanty += (coeffY * dOffy * dOffy);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
}

void
getObjectiveCliqueModelXYForCluster(vector<Net*> &internalNets, vector<Cell*> &inputCells, 
				    vector<uint> &subi_vecx, vector<uint> &subj_vecx, 
				    vector<double> &valij_vecx, 
				    vector<uint> &sub_vecx, vector<double> &val_vecx, 
				    vector<uint> &subi_vecy, vector<uint> &subj_vecy, 
				    vector<double> &valij_vecy, 
				    vector<uint> &sub_vecy, vector<double> &val_vecy, 
				    double& constantx, double &constanty)
{
  Cell *cellPtri, *cellPtrj;
  Pin *pinPtri, *pinPtrj;
  Net *netPtr;
  double edgeWeight, coeffY, coeffX;
  double yOffi, yOffj, celljy, dOffy;
  double xOffi, xOffj, celljx, dOffx;
  map<Cell *, uint> cellLookupMap;
  map<Cell *, uint>::iterator lookupItr;
  map<uint, uint> squaredTermsMap;
  map<uint, uint> linearTermsMap;
  map<uint, uint>::iterator itrUintUint;
  long cellIdxi, cellIdxj;
  uint numCells, edgeIdx;
  uint i, j;

  numCells = inputCells.size();
  /* Get the cell lookup map */
  cellLookupMap = getCellLookupMap(inputCells);
  /* Initialize the constant to 0 */
  constantx = 0.0;
  /* Iterate over all HyperEdges of the graph */
  VECTOR_FOR_ALL_ELEMS(internalNets, Net*, netPtr) {
    map<Pin *, bool> visitedPins;
    /* Get the related logical for the hyperedge */
    Net &relatedNet = *netPtr;
    edgeWeight = relatedNet.NetGetWeight();
    /* Begin iteration over all pins */
    /* Print out all pins of the net */
    NET_FOR_ALL_PINS(relatedNet, pinPtri) {
      Cell &celli = (*pinPtri).PinGetParentCell();
      cellPtri = &celli;
      /* Get the index of the cell */
      _KEY_EXISTS_WITH_VAL(cellLookupMap, cellPtri, lookupItr) {
	cellIdxi = lookupItr->second;
      } else {
	continue;
      }
      coeffX = edgeWeight;
      coeffY = edgeWeight;
      /* Mark the pin as visited */
      visitedPins[pinPtri] = true;
      NET_FOR_ALL_PINS(relatedNet, pinPtrj) {
	Cell &cellj = (*pinPtrj).PinGetParentCell();
	cellPtrj = &cellj;
	if (cellPtri == cellPtrj) continue;
	/* Skip the visited pins for clique type traversal */
	_KEY_EXISTS(visitedPins, pinPtrj) continue;
	/* Get the index of the cell */
	_KEY_EXISTS_WITH_VAL(cellLookupMap, cellPtrj, lookupItr) {
	  cellIdxj = lookupItr->second;
	} else {
	  cellIdxj = -1;
	}
	_KEY_EXISTS_WITH_VAL(squaredTermsMap, cellIdxi, itrUintUint) {
	  valij_vecx[itrUintUint->second] += coeffX;
	  valij_vecy[itrUintUint->second] += coeffY;
	} else {
	  subi_vecx.push_back(cellIdxi); subj_vecx.push_back(cellIdxi); valij_vecx.push_back(coeffX);
	  subi_vecy.push_back(cellIdxi); subj_vecy.push_back(cellIdxi); valij_vecy.push_back(coeffY);
	  squaredTermsMap[cellIdxi] = (valij_vecx.size() - 1);
	}
	if (cellIdxj > 0) {
	  _KEY_EXISTS_WITH_VAL(squaredTermsMap, cellIdxj, itrUintUint) {
	    valij_vecx[itrUintUint->second] += coeffX;
	    valij_vecy[itrUintUint->second] += coeffY;
	  } else {
	    subi_vecx.push_back(cellIdxj); subj_vecx.push_back(cellIdxj); valij_vecx.push_back(coeffX);
	    subi_vecy.push_back(cellIdxj); subj_vecy.push_back(cellIdxj); valij_vecy.push_back(coeffY);
	    squaredTermsMap[cellIdxj] = (valij_vecx.size() - 1);
	  }
	  unsigned int idxi, idxj;
	  idxi = cellIdxi; idxj = cellIdxj;
	  /* If cellIdxi < cellIdxj, swap */
	  if (cellIdxi < cellIdxj) swap(idxi, idxj);
	  /* No map handling is required for a pair of movable pins since 
	     each pair of movable pins is visited only once in the traversal*/
	  /* Insert value for y-variable */
	  subi_vecx.push_back(idxi); subj_vecx.push_back(idxj); valij_vecx.push_back(-coeffX);
	  subi_vecy.push_back(idxi); subj_vecy.push_back(idxj); valij_vecy.push_back(-coeffY);
	}
	/* Take care of inserting value for the linear terms as well */
	xOffi = ((double)(*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	xOffj = ((double)(*pinPtrj).PinGetXOffset()) / GRID_COMPACTION_RATIO; 
	yOffi = ((double)(*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO; 
	yOffj = ((double)(*pinPtrj).PinGetYOffset()) / GRID_COMPACTION_RATIO; 

	dOffy = yOffi - yOffj;
	dOffx = xOffi - xOffj;
	if (cellIdxj > 0) {
	  _KEY_EXISTS_WITH_VAL(linearTermsMap, cellIdxj, itrUintUint) {
	    val_vecx[itrUintUint->second] += (-(dOffx * coeffX));
	    val_vecy[itrUintUint->second] += (-(dOffy * coeffY));
	  } else {
	    sub_vecx.push_back(cellIdxj); val_vecx.push_back(-(dOffx * coeffX));
	    sub_vecy.push_back(cellIdxj); val_vecy.push_back(-(dOffy * coeffY));
	    linearTermsMap[cellIdxj] = val_vecx.size() - 1;
	  }
	}
	_KEY_EXISTS_WITH_VAL(linearTermsMap, cellIdxi, itrUintUint) {
	  val_vecx[itrUintUint->second] += (dOffx * coeffX);
	  val_vecy[itrUintUint->second] += (dOffy * coeffY);
	} else {
	  sub_vecx.push_back(cellIdxi); val_vecx.push_back(dOffx * coeffX);
	  sub_vecy.push_back(cellIdxi); val_vecy.push_back(dOffy * coeffY);
	  linearTermsMap[cellIdxi] = val_vecx.size() - 1;
	}
	constantx += (coeffX * dOffx * dOffx);
	constanty += (coeffY * dOffy * dOffy);
      } NET_END_FOR;
    } NET_END_FOR;
  } HYPERGRAPH_END_FOR;
}
