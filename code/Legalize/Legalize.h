# ifndef LEGALIZE_H
# define LEGALIZE_H

# include <common.h>
# include <PhysRow.h>
# include <Cell.h>
# include <Pin.h>
# include <lemon/list_graph.h>
# include <lemon/network_simplex.h>
# include <Design.h>
# include <lemon/bfs.h>
# include <lemon/concepts/digraph.h>
# include <Flags.h>
# include <HyperGraph.h>
# include <HyperGraphIter.h>

using namespace std;

/* Default definitions */

# define DEFAULT_REGION_BEGIN 0
# define DEFAULT_REGION_END 0
# define DEFAULT_BIN_BEGIN 0
# define DEFAULT_BIN_END 0
# define DEFAULT_REMPTY 1
# define DEFAULT_ROW NIL(PhysRow*)
# define DEFAULT_SUPPLY 0
# define DEFAULT_TOTAL_CELL_WIDTH 0 
# define DEFAULT_LBOUND 0
# define DEFAULT_RBOUND 0
# define DEFAULT_INT_SUPPLY 0
# define DEFAULT_INT_WIDTH 0
# define DEFAULT_INTCELL_WIDTH 0
# define DEFAULT_ZONE_BEGIN 0
# define DEFAULT_ZONE_END 0
# define DEFAULT_EMPTY true
# define DEFAULT_ZROW NIL(PhysRow*)
# define DEFAULT_ZTOTAL_CELL_WIDTH 0 
# define DEFAULT_ZLRBOUND 2
# define debugPrint 0

class Zone;

class LegalizeBin{
 private:
  int binBegin;
  int binEnd;
  int binBot;
  int binWidth;
  int binHeight;
  bool binLBound;
  bool binRBound;
  bool isEmpty;
  PhysRow* presentInRow;
  vector<Cell*> cellsInBin;
  map<Cell*, double> cellFracts;
  double supply;
  int totalCellWidth;
  int binIndex;
  Zone *inZone;

 public:
  /* Constructor and Destructor */
  LegalizeBin();
  LegalizeBin(int, PhysRow*);
  LegalizeBin(int, PhysRow*, bool);
  LegalizeBin(int, PhysRow*, int, bool);
  LegalizeBin(int, PhysRow*, int, int, bool);
    
  /* Set Functions */
  void BinSetBegin(int);
  void BinSetEnd(int);
  void BinSetEmpty(bool);
  void BinSetSupply(double);
  void BinSetRow(PhysRow*);
  void BinSetTotalCellWidth(int);
  void BinSetLBound(bool);
  void BinSetRBound(bool);
  void BinSetIndex(int);
  void BinSetCellFract(Cell*, double);
  void BinSetHeight(int);
  void BinSetBot(int);
  void BinSetZone(Zone*);

  /* Get functions */
  int BinGetBegin(void);
  int BinGetEnd(void);
  bool BinGetEmpty(void);
  double BinGetSupply(void);
  PhysRow* BinGetRow(void);
  int BinGetWidth(void);
  bool BinGetLBound(void);
  bool BinGetRBound(void);
  int BinGetFCellWidth(void);
  int BinGetLCellWidth(void);
  int BinGetIndex(void);
  double BinGetCellFract(Cell*);
  vector<Cell*>& BinGetCellsInBin(void);
  int BinGetTotalCellWidth(void);
  void BinGetFractCells(map<Cell*, double> &);
  int BinGetHeight(void);
  int BinGetBot(void);  
  Zone* BinGetZone(void);

  /* Other functions */
  void BinAddCellToBin(Cell*);
  void BinRemoveCellFromBin(Cell*);
  void BinRemoveCellFromFract(Cell*);
  void BinCalculateWidth(void);
  void BinFindCellsInBin(vector<Cell*> &);
  void BinAddCellFracts(Cell*, double);
  map<Cell*, double>& BinGetAllFractCells(void);
  void BinRemoveAllFractCells(void);
};


class Region {
 private:
  int regionBegin;
  int regionEnd;
  int regionWidth;
  bool regionLBound;
  bool regionRBound;
  bool isEmpty;
  PhysRow* presentInRow;
  vector<Cell*> cellsInRegion;
  int supply;
  int totalCellWidth;

 public:
  /* Constructor and Destructor */
  Region();
  Region(int, PhysRow*);
  Region(int, PhysRow*, bool);
  Region(int, PhysRow*, int, bool);
  Region(int, PhysRow*, int, int, bool);
    
  /* Set Functions */
  void RegionSetBegin(int);
  void RegionSetEnd(int);
  void RegionSetEmpty(bool);
  void RegionSetSupply(int);
  void RegionSetRow(PhysRow*);
  void RegionSetTotalCellWidth(int);
  void RegionSetLBound(bool);
  void RegionSetRBound(bool);

  /* Get functions */
  int RegionGetBegin(void);
  int RegionGetEnd(void);
  bool RegionGetEmpty(void);
  int RegionGetSupply(void);
  PhysRow* RegionGetRow(void);
  int RegionGetRegionWidth(void);
  bool RegionGetLBound(void);
  bool RegionGetRBound(void);
  int RegionGetFCellWidth(void);
  int RegionGetLCellWidth(void);

  
  vector<Cell*>& RegionGetCellsInRegion(void);
  int RegionGetTotalCellWidth(void);
  
  /* Other functions */
  void RegionAddCellToRegion(Cell*);
  void RegionCalculateWidth(void);
  void RegionFindCellsInRegion(vector<Cell*> &);
  //void RegionCalculateSupply(void);
};

class Interval {
 private:
  vector<Region*> regionsInInterval;
  int intervalSupply;
  int intervalBegin;
  int intervalEnd;
  int intervalWidth;
  int totCellWidth;
  bool intervalLBound;
  bool intervalRBound;
  
 public:
  /* Constructor and Destructor */
  Interval();
  //To be added later if required
  // Interval(vector<Region*> &);
  //Interval(vector<Region*> &, unsigned int);
  //Interval(vector<Region*> &, unsigned int, int);
     
  /* Set Functions */
  void IntervalCalculateAndSetSupply(vector<Region*> &);
  void IntervalSetWidth(int);
  void IntervalCalculateWidth(void);
  void IntervalSetCellWidth(int);
  void IntervalSetBegin(int);
  void IntervalSetEnd(int);
  void IntervalSetLBound(void);
  void IntervalSetRBound(void);

  /* Get functions */
  int IntervalGetSupply(void);
  int IntervalGetWidth(void);
  int IntervalGetCellWidth(void);
  vector<Region*>& IntervalGetRegionsInInterval(void);
  int IntervalGetBegin(void);
  int IntervalGetEnd(void);
  bool IntervalGetLBound(void);
  bool IntervalGetRBound(void);

  
  /* Other functions */
  void IntervalAddRegion(Region*);
  
};

class Zone {
 private:
  int zoneBegin;
  int zoneEnd;
  int zoneWidth;
  unsigned int zoneLRBound;
  bool isEmpty;
  PhysRow* presentInRow;
  vector<Cell*> cellsInZone;
  int totalCellWidth;

 public:
  /* Constructor and Destructor */
  Zone();
  Zone(int, bool);
  Zone(int, PhysRow*, bool);
    
  /* Set Functions */
  void ZoneSetBegin(int);
  void ZoneSetEnd(int);
  void ZoneSetEmpty(bool);
  void ZoneSetRow(PhysRow*);
  void ZoneSetTotalCellWidth(int);
  void ZoneSetLRBound(unsigned int);
  

  /* Get functions */
  int ZoneGetBegin(void);
  int ZoneGetEnd(void);
  bool ZoneGetEmpty(void);
  PhysRow* ZoneGetRow(void);
  unsigned int ZoneGetLRBound(void);
  vector<Cell*>& ZoneGetCellsInZone(void);
  int ZoneGetTotalCellWidth(void);
  int ZoneGetBot(void);

  /* Other functions */
  void ZoneAddCellToZone(Cell*);
  void ZoneRemoveCellFromZone(Cell*);
  int ZoneGetWidth(void);
  //void ZoneFindCellsInZone(vector<Cell*> &);

};


void LegalizeGetAllBoundingBoxes(vector<PhysRow*>& , vector<vector<int> >& );

void LegalizeCheckLegality(Cell* , vector<PhysRow*>&, vector<vector<int> >&);

void LegalizeSnapToNearestRows(Cell* , vector<PhysRow*>& , rowOrientation);

void findRowCoordinates(vector<int>&, int , int , int , int nearRows[]);

//void LegalizeConstructGNFGraph(vector<PhysRow*> &, ListDigraph &, ArcMap<int> &, NodeMap<int> &);

void LegalizeDesign(Design &);

//void LegalizeCalculateMinCostFlow(ListDigraph &, ArcMap<int> &, NodeMap<int> &);

void LegalizeTestMinCostFlow(void);

void LegalizeFindZonesInRow(PhysRow*, vector<Zone*> &, int, int);

void LegalizeFindRegionsInRow(vector<pair<int,unsigned int> > &,  
			      vector<Region*> &, PhysRow* &, int, unsigned int, unsigned int);

void LegalizeCalculateSupplyForRegions(vector<Region*> &);

Region* LegalizeFindPorNRegion(vector<Region*> &, Region* , bool);

void LegalizeSnapCellsToFreeZone(int, int, vector<Cell*> &, int, int);
void LegalizeAddCellsToRegions(vector<Cell*> &, vector<Region*> &);

void LegalizeFindBinsInRow(vector<Zone*> &, vector<LegalizeBin*> &, PhysRow* &, int, int, int);

void LegalizeMergeBins(vector<LegalizeBin*> &, int);

void LegalizeAddCellsToBins(vector<Cell*> &cellsInRow, vector<LegalizeBin*> &allBins);

void LegalizeCellGetCost(Cell &, double&, double&, double&, double&, double&);

PhysRow* LegalizeGetRowByIndex(vector<PhysRow*> &, int);

double LegalizeThetaFunc(uint, uint, uint, uint);

void LegalizeDoFractReassign(vector<LegalizeBin*>&);

void LegalizePlotData(string, string, Design&, vector<vector<LegalizeBin*> > &);

void LegalizeCalcSuppForBins(vector<LegalizeBin*> &, vector<LegalizeBin*> &);

void LegalizeReCalcSuppForBins(vector<LegalizeBin*> &, vector<LegalizeBin*> &);

void
LegalizeGetNeighbours(LegalizeBin*&, LegalizeBin*&,
		      LegalizeBin*&, vector<LegalizeBin*>&);

/*** FILE LegalizeUtils.cxx ***/
double 
LegalizeGetCellsToMove(LegalizeBin &srcBin, LegalizeBin &destBin,
		       map<Cell *, double> &fractionalCells, bool sameZone,
		       map<Cell *, double> &cellsToMove, double &totalFracWidth);

void LegalizeAugPathAlgo(HyperGraph &, LegalizeBin*, vector<LegalizeBin*> &PathBins);

void
LegalizeReCalcSuppAllBins(vector<vector<LegalizeBin*> > &, vector<LegalizeBin*> &);

void
LegalizeAssignLValues(vector<vector<LegalizeBin*> > &);

void 
LegalizePrintBinData(LegalizeBin *);

# endif

