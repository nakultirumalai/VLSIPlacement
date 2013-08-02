# include <Design.h>

void
Design::DesignMoveCellsInBinsILR(void)
{
  Cell *cellPtr;
  Bin *srcBinPtr, *targetBinPtr;
  map<Cell *, bool> visitedCells; 
  uint binIdx;
  uint oldXpos, oldYpos, newXpos, newYpos;
  ulong DesignXHPWL, DesignYHPWL, DesignHPWL;
  ulong oldXHPWL, oldYHPWL, newXHPWL, newYHPWL;
  bool moveDebug;
  
  moveDebug = false;
  //  cout << "########################################################" << endl;
  //  cout << "BEGIN ILR ITERATION"                                      << endl;
  DesignXHPWL = DesignGetXHPWL();
  DesignYHPWL = DesignGetYHPWL();
  DesignHPWL = DesignGetHPWL();
  DESIGN_FOR_ALL_BINS((*this), binIdx, srcBinPtr) {
    if (moveDebug) {
      cout << "*****************************************" << endl;
      cout << "BIN : " << binIdx << " " << endl;
      cout << "*****************************************" << endl;
    }
    vector<Cell *> &cellsOfBin = (*srcBinPtr).BinGetCells();
    uint numCells = cellsOfBin.size();
    int idx;
    for (idx = 0; idx < numCells; idx++) {
      cellPtr = cellsOfBin[idx];
      if (moveDebug) {
	cout << "*****************************************" << endl;
	cout << "CELL: " << (*cellPtr).CellGetName() << " " << endl;
	cout << "*****************************************" << endl;
      }
      _KEY_DOES_NOT_EXIST(visitedCells, cellPtr) {
	visitedCells[cellPtr] = true;
      } else {
	continue;
	//	cout << "Error: Cell being visited multiple times in an ILR loop" << endl;
	//_ASSERT_TRUE("Error: Cell being visited multiple times in an ILR loop");
      }
      if (DesignCellGetBestPosForILR((*cellPtr), (*srcBinPtr), 
				     targetBinPtr, newXpos, newYpos)) {
	if (moveDebug) {
	  oldXpos = (*cellPtr).CellGetXpos();
	  oldYpos = (*cellPtr).CellGetYpos();
	  //	  cout << "Moving cell " << (*cellPtr).CellGetName() 
	  //	       << " from " << oldXpos << "," << oldYpos 
	  //	       << " to " << newXpos << "," << newYpos << endl; 
	}
	//	cout << "Removing cell " << (*cellPtr).CellGetName() << endl;
	if (0) {
	  cout << endl;
	  cout << "Design X HPWL: " << DesignXHPWL << endl;
	  cout << "Design Y HPWL: " << DesignYHPWL << endl;
	  cout << "Design HPWL: " << DesignHPWL << endl;
	  (*cellPtr).CellMoveCell(newXpos, newYpos);
	  //	  DesignComputeHPWL();
	  DesignXHPWL = DesignGetXHPWL();
	  DesignYHPWL = DesignGetYHPWL();
	  DesignHPWL = DesignGetHPWL();
	  cout << "Design X HPWL: " << DesignXHPWL << endl;
	  cout << "Design Y HPWL: " << DesignYHPWL << endl;
	  cout << "Design HPWL: " << DesignHPWL << endl;
	} else {
	  //	  cout << endl;
	  //	  cout << "Design X HPWL: " << DesignXHPWL << endl;
	  //	  cout << "Design Y HPWL: " << DesignYHPWL << endl;
	  //	  cout << "Design HPWL: " << DesignHPWL << endl;
	  (*cellPtr).CellMoveCellComputeHPWL(newXpos, newYpos, oldXHPWL, oldYHPWL,
					     newXHPWL, newYHPWL);
	  DesignXHPWL += (((long)newXHPWL) - oldXHPWL); DesignSetXHPWL(DesignXHPWL);
	  DesignYHPWL += (((long)newYHPWL) - oldYHPWL); DesignSetYHPWL(DesignYHPWL);
	  DesignHPWL = DesignXHPWL + DesignYHPWL; DesignSetHPWL(DesignHPWL);
	  //	  cout << "Design X HPWL: " << DesignXHPWL << endl;
	  //	  cout << "Design Y HPWL: " << DesignYHPWL << endl;
	  //	  cout << "Design HPWL: " << DesignHPWL << endl;
	}
	(*srcBinPtr).BinRemoveCell(idx); //idx--;	numCells--;
	(*targetBinPtr).BinAddCell(cellPtr);
      } else {
	if (moveDebug) 
	  cout << "Not moving cell " << (*cellPtr).CellGetName() << endl; 
      }
    }
  } DESIGN_END_FOR;
}

void 
Design::DesignTestCellMovement(void)
{
  Cell *cellPtr;
  Net *netPtr;
  ulong prevHPWL1, prevHPWL2, curHPWL;
  uint ilrItrCount;

  DesignComputeHPWL();
  curHPWL = DesignGetHPWL();
  prevHPWL1 = curHPWL;
  prevHPWL2 = curHPWL;
  ilrItrCount = 0;
  //  cout << "*******************************************************" << endl;
  //  cout << "  BEGIN ILR " << endl;
  while (ilrItrCount > 4) {
    DesignMoveCellsInBinsILR();
    uint oldxHPWL = 0;
    uint oldyHPWL = 0;
    uint curxHPWL = 0;
    uint curyHPWL = 0;
    uint totalXHPWL = 0;
    uint totalYHPWL = 0;
    //  curHPWL = DesignGetHPWL();
    cellPtr = DesignGetNode("o72");
    CELL_FOR_ALL_NETS((*cellPtr), PIN_DIR_ALL, netPtr) {
      totalXHPWL += curxHPWL;
      totalYHPWL += curyHPWL;
    } CELL_END_FOR;
    //    cout << "ILR: " << ilrItrCount << "  HPWL-X: " << totalXHPWL
    //	 << "  HPWL-Y: " << totalYHPWL << "  HPWL: " << (totalXHPWL + totalYHPWL)
    //	 << endl;
    //    cout << "***********************************************" << endl;
    //    cout << "BEFORE MOVE:" << endl;
    //    cout << "Cell o72 :  X-pos: " << (*cellPtr).CellGetXpos() 
    //	 << "  Y-pos: " << (*cellPtr).CellGetYpos() << endl;
    (*cellPtr).CellMoveCell(5699, 2815);
    //    cout << "AFTER MOVE:" << endl;
    //    cout << "Cell o72 :  X-pos: " << (*cellPtr).CellGetXpos() 
    //	 << "  Y-pos: " << (*cellPtr).CellGetYpos() << endl;
    //    cout << "***********************************************" << endl;
    oldxHPWL = 0;
    oldyHPWL = 0;
    curxHPWL = 0;
    curyHPWL = 0;
    totalXHPWL = 0;
    totalYHPWL = 0;
    CELL_FOR_ALL_NETS((*cellPtr), PIN_DIR_ALL, netPtr) {
      //      (*netPtr).NetComputeHPWL(oldxHPWL, oldyHPWL, curxHPWL, curyHPWL);
      totalXHPWL += curxHPWL;
      totalYHPWL += curyHPWL;
    } CELL_END_FOR;
    //  DesignComputeHPWL();
    //  curHPWL = DesignGetHPWL();
    cout << "ILR: " << ilrItrCount << "  HPWL-X: " << totalXHPWL
	 << "  HPWL-Y: " << totalYHPWL << "  HPWL: " << (totalXHPWL + totalYHPWL)
	 << endl;

    /* If HPWL change is less than 10% of previous gain, break */
    //    if ((prevHPWL1 - curHPWL) < (0.1)*(prevHPWL2 - prevHPWL1)) {
    //      break;
    //    }
    ilrItrCount++;
    //  }
    DesignRefreshBins();
  }
}

void 
Design::DesignDoILRIter(void)
{
  Cell *cellPtr;
  Net *netPtr;
  ulong prevHPWL1, prevHPWL2, curHPWL;
  ulong totalXHPWL, totalYHPWL, origHPWL;
  ulong diffPrev, diffCurrent;
  uint ilrItrCount;

  DesignComputeHPWL();
  curHPWL = DesignGetHPWL();
  prevHPWL1 = INT_MAX;
  prevHPWL2 = INT_MAX;
  ilrItrCount = 0;
  totalXHPWL = DesignGetXHPWL();
  totalYHPWL = DesignGetYHPWL();
  
  cout << "ILR: " << ilrItrCount << "  HPWL-X: " << totalXHPWL
       << "  HPWL-Y: " << totalYHPWL << "  HPWL: " << (totalXHPWL + totalYHPWL)
       << endl;
  origHPWL = curHPWL;
  string DesignName = DesignGetName();
  uint maxIter = 20;
  string plotFileName = DesignName + "_ILR_" + getStrFromInt(ilrItrCount) + ".plt";
  DesignPlotData("Title", plotFileName);
  //  ProfilerStart("ILR");
  while (1) {
    prevHPWL2 = prevHPWL1;
    prevHPWL1 = curHPWL;
    DesignMoveCellsInBinsILR();
    //    DesignComputeHPWL();
    totalXHPWL = DesignGetXHPWL(); totalYHPWL = DesignGetYHPWL();
    curHPWL = totalXHPWL + totalYHPWL;
    ilrItrCount++;
    cout << "ILR: " << ilrItrCount << "  HPWL-X: " << totalXHPWL
	 << "  HPWL-Y: " << totalYHPWL << "  HPWL: " << curHPWL
	 << endl;
    if ((abs(curHPWL - prevHPWL1) < 1000) || (ilrItrCount > maxIter)) {
      break;
    }
    DesignRefreshBins();
  }
  //  ProfilerStop();
  plotFileName = DesignName + "_ILR_" + getStrFromInt(ilrItrCount) + ".plt";
  DesignPlotData("Title", plotFileName);
}

