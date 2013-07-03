# include <Legalize.h>

using namespace std;

/* Set Functions */

void
Interval::IntervalSetWidth(int width)
{
  this->intervalWidth = width;
}

void
Interval::IntervalCalculateWidth(void)
{
  this->intervalWidth = intervalBegin - intervalEnd;
}

void
Interval::IntervalSetCellWidth(int totCellWidth)
{
  this->totCellWidth = totCellWidth;
}

void
Interval::IntervalSetBegin(int Begin)
{
  this->intervalBegin = Begin;
}

void
Interval::IntervalSetEnd(int End)
{
  this->intervalEnd = End;
}

void
Interval::IntervalSetLBound(void)
{
  Region* firstRegion = regionsInInterval[0];
  bool LBound = firstRegion->RegionGetLBound();
  this->intervalLBound = LBound;
}

void
Interval::IntervalSetRBound(void)
{
  int lastIdx = regionsInInterval.size() - 1;
  Region* lastRegion = regionsInInterval[lastIdx];
  bool RBound = lastRegion->RegionGetRBound();
  this->intervalRBound = RBound;
}

/* Get Functionss */
int
Interval::IntervalGetSupply(void)
{
  return intervalSupply;
}

int
Interval::IntervalGetWidth(void)
{
  return intervalWidth;
}

int
Interval::IntervalGetCellWidth(void)
{
  return totCellWidth;
}

vector<Region*> &
Interval::IntervalGetRegionsInInterval(void)
{
  return (this->regionsInInterval);
}

int
Interval::IntervalGetBegin(void)
{
  return intervalBegin;
}

int
Interval::IntervalGetEnd(void)
{
  return intervalEnd;
}

bool
Interval::IntervalGetLBound(void)
{
  return intervalLBound;
}

bool
Interval::IntervalGetRBound(void)
{
  return intervalRBound;
}

/* other Interval functions */
void
Interval::IntervalAddRegion(Region* region)
{
  (this->regionsInInterval).push_back(region);
  int regionWidth = region->RegionGetRegionWidth();
  (this->intervalWidth) += regionWidth;
  int cellWidth = region->RegionGetTotalCellWidth();
  (this->totCellWidth) += cellWidth;
}

void
Interval::IntervalCalculateAndSetSupply(vector<Region*> &formInt)
{
  int supply = (this->totCellWidth) - (this->intervalWidth);
  int numRegions = regionsInInterval.size();
  Region* first = regionsInInterval[0];
  Region* last = regionsInInterval[(numRegions-1)];
  bool supplyFound = false;
  int fCellWidth, lCellWidth;
  if (supply > 0){
    fCellWidth = first->RegionGetFCellWidth();
    lCellWidth = last->RegionGetLCellWidth();
    
    if ((intervalLBound) && !(intervalRBound)){            // Bound on the left
      supply -= ((0.5) * lCellWidth);
    } else if (!(intervalLBound) && (intervalRBound)){     // Bound on the right
      supply -= ((0.5) * fCellWidth);
    } else if (!(intervalLBound) && !(intervalRBound)){    // Not Bound in any direction
      supply -= ((0.5) * lCellWidth);
      supply -= ((0.5) * fCellWidth);
      // If bound in both direction no extra calculation required for supply
    }
    supplyFound = true;
  } else if (supply <= 0){
    /* Indicates a possible demand region */
    if ((intervalLBound) && !(intervalRBound)){
      // Bound on the left, so find the first cell width of next region
      Region* next = LegalizeFindPorNRegion(formInt, last, false);
      fCellWidth = next->RegionGetFCellWidth(); 
      supply += ((0.5) * fCellWidth);
    } else if (!(intervalLBound) && (intervalRBound)){
      // Bound on the right, so find the last cell width of previous region
      Region* prev = LegalizeFindPorNRegion(formInt, first, true);
      lCellWidth = prev->RegionGetLCellWidth();
      supply += ((0.5) * lCellWidth);
    } else if (!(intervalLBound) && !(intervalRBound)){
      /* Not bound on either side so, find the width of first cell of next
	     region and the last cell of the previous region */
      Region* next = LegalizeFindPorNRegion(formInt, last, false);
      fCellWidth = next->RegionGetFCellWidth();
      Region* prev = LegalizeFindPorNRegion(formInt, first, true);
      lCellWidth = prev->RegionGetLCellWidth();
      supply += ((0.5) * lCellWidth);
      supply += ((0.5) * fCellWidth);
    }
  }
  // For a supposed supply node gives the value of max(supply, 0)
  if((supplyFound) && (supply < 0)){
    this->intervalSupply = 0;
  } else if ((supplyFound) && (supply >= 0)){
    this->intervalSupply = supply;
  } else if((!supplyFound) && (supply < 0)){
    // For a supposed demand node gives the value of min(supply,0)
    this->intervalSupply = supply;
  } else if((!supplyFound) && (supply >= 0)){
    this->intervalSupply = 0;
  }
}


/* Constructors and Destructors */

Interval::Interval()
{
  IntervalSetCellWidth(DEFAULT_INTCELL_WIDTH);
  IntervalSetWidth(DEFAULT_INT_WIDTH);
}
