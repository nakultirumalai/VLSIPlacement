# include <TimingModels.h>

void
getObjectiveStarModelXY(Design &myDesign, HyperGraph &myGraph,
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
  uint numNetPins, starNodeIdx;
  uint numClique, numStar;
  uint i, j;
  bool cellIsTerminal;

  numClique = 0;
  numStar = 0;
  numCells = inputCells.size();
  /* Get the cell lookup map */
  cellLookupMap = getCellLookupMap(inputCells);
  /* Initialize the constant to 0 */
  constantx = 0.0;
  /* Iterate over all HyperEdges of the graph */
  HYPERGRAPH_FOR_ALL_EDGES(myGraph, edgeIdx, edgeWeight) {
    map<Pin *, bool> visitedPins;
    /* Get the related logical edge for the hyperedge */
    Net &relatedNet = *(Net *)myGraph.GetEdgeObject(edgeIdx);
    edgeWeight = relatedNet.NetGetWeight();
    /* Set the cell as a star node so that the bounds 
       can be recognized */
    numNetPins = relatedNet.NetGetPinCount();
    if (numNetPins < starModelUpgradeNumPins) {
      numClique++;
      /* Get the number of pins on the net */
      /* Begin iteration over all pins */
      NET_FOR_ALL_PINS(relatedNet, pinPtri) {
	Cell &celli = (*pinPtri).PinGetParentCell();
	cellPtri = &celli;
	/* Skip terminal cells as they do not have a variable associated to 
	   them */
	_KEY_EXISTS(cellLookupMap, cellPtri) {
	  cellIdxi = cellLookupMap[cellPtri];
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
    } else {
      /* Create a virtual star node for the net */
      numStar++;
      Cell *starNode = new Cell(0,0);
      CellSetIsStarNode(starNode);
      starNodeIdx = inputCells.size();
      inputCells.push_back(starNode);
      NET_FOR_ALL_PINS(relatedNet, pinPtri) {
	Cell &celli = (*pinPtri).PinGetParentCell();
	cellPtri = &celli;
	coeffX = edgeWeight * numNetPins;
	coeffY = edgeWeight * numNetPins;
	cellIsTerminal = (*cellPtri).CellIsTerminal();
	if (!cellIsTerminal) {
	  cellIdxi = cellLookupMap[cellPtri];
	} else {
	  cellIdxi = -1;
	}
	/* For normal cells that are movable */
	if (!cellIsTerminal) {
	  /* Add term to the Hessian for the regular cell */
	  _KEY_EXISTS_WITH_VAL(squaredTermsMap, cellIdxi, itrUintUint) {
	    valij_vecx[itrUintUint->second] += coeffX;
	    valij_vecy[itrUintUint->second] += coeffY;
	  } else {
	    subi_vecx.push_back(cellIdxi); subj_vecx.push_back(cellIdxi); valij_vecx.push_back(coeffX);
	    subi_vecy.push_back(cellIdxi); subj_vecy.push_back(cellIdxi); valij_vecy.push_back(coeffY);
	    squaredTermsMap[cellIdxi] = (valij_vecx.size() - 1);
	    quadMap[cellPtri] = (valij_vecx.size() - 1);
	  }
	  subi_vecx.push_back(starNodeIdx); subj_vecx.push_back(cellIdxi); valij_vecx.push_back(-coeffX);
	  subi_vecy.push_back(starNodeIdx); subj_vecy.push_back(cellIdxi); valij_vecy.push_back(-coeffY);
	}

	/* Add term to the Hessian for the star node cell unconditionally */
	_KEY_EXISTS_WITH_VAL(squaredTermsMap, starNodeIdx, itrUintUint) {
	  valij_vecx[itrUintUint->second] += coeffX;
	  valij_vecy[itrUintUint->second] += coeffY;
	} else {
	  subi_vecx.push_back(starNodeIdx); subj_vecx.push_back(starNodeIdx); valij_vecx.push_back(coeffX);
	  subi_vecy.push_back(starNodeIdx); subj_vecy.push_back(starNodeIdx); valij_vecy.push_back(coeffY);
	  squaredTermsMap[starNodeIdx] = (valij_vecx.size() - 1);
	}

	if (!cellIsTerminal) {
	  /* Add the linear terms for the regular cell */
	  xOffi = ((double)(*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO;
	  yOffi = ((double)(*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO;
	  _KEY_EXISTS_WITH_VAL(linearTermsMap, cellIdxi, itrUintUint) {
	    val_vecx[itrUintUint->second] += (xOffi * coeffX);
	    val_vecy[itrUintUint->second] += (yOffi * coeffY);
	  } else {
	    sub_vecx.push_back(cellIdxi); val_vecx.push_back(xOffi * coeffX);
	    sub_vecy.push_back(cellIdxi); val_vecy.push_back(yOffi * coeffY);
	    linearTermsMap[cellIdxi] = val_vecx.size() - 1;
	    linMap[cellPtri] = val_vecx.size() - 1;
	  }
	} else {
	  xOffi = ((*cellPtri).CellGetXpos() + (*pinPtri).PinGetXOffset()) / GRID_COMPACTION_RATIO;
	  yOffi = ((*cellPtri).CellGetYpos() + (*pinPtri).PinGetYOffset()) / GRID_COMPACTION_RATIO;
	}

	/* Add the linear terms for the star node */
	_KEY_EXISTS_WITH_VAL(linearTermsMap, starNodeIdx, itrUintUint) {
	  val_vecx[itrUintUint->second] += (-(xOffi * coeffX));
	  val_vecy[itrUintUint->second] += (-(yOffi * coeffY));
	} else {
	  sub_vecx.push_back(starNodeIdx); val_vecx.push_back(-(xOffi * coeffX));
	  sub_vecy.push_back(starNodeIdx); val_vecy.push_back(-(yOffi * coeffY));
	  linearTermsMap[starNodeIdx] = val_vecx.size() - 1;
	}
	  
	/* Add the constant term */
	constantx += (coeffX * xOffi) * (coeffX * xOffi);
	constanty += (coeffY * yOffi) * (coeffY * yOffi);
      } NET_END_FOR;
    }
  } HYPERGRAPH_END_FOR;
}
