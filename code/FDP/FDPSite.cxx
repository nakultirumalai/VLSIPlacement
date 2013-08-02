# include <FDP.h>

/* Set functions */
void
FDPSite::FDPSiteSetCell(Cell *thisCell)
{
  this->siteCell = thisCell;
}

void
FDPSite::FDPSiteSetIsLocked(bool isLocked)
{
  this->siteIsLocked = isLocked;
}

void
FDPSite::FDPSiteSetXpos(int siteX)
{
  this->siteX = siteX;
}

void
FDPSite::FDPSiteSetYpos(int siteY)
{
  this->siteY = siteY;
}

void
FDPSite::FDPSiteSetSiteNum(int siteNum)
{
  this->siteNum = siteNum;
}

void
FDPSite::FDPSiteSetRowNum(int rowNum)
{
  this->rowNum = rowNum;
}


/* Get Functions */

Cell*
FDPSite::FDPSiteGetCell(void) 
{
  return this->siteCell;
}

bool
FDPSite::FDPSiteGetIsLocked(void)
{
  return this->siteIsLocked;
}

bool
FDPSite::FDPSiteIsOccupied(void)
{
  bool rtv;

  rtv = (this->siteCell != NIL(Cell *));
  
  return (rtv);
}

bool
FDPSite::FDPSiteIsVacant(void)
{
  bool rtv;

  rtv = (this->siteCell == NIL(Cell *));
  
  return (rtv);
}

int
FDPSite::FDPSiteGetXpos(void)
{
  return this->siteX;
}

int
FDPSite::FDPSiteGetYpos(void)
{
  return this->siteY;
}

int
FDPSite::FDPSiteGetSiteNum(void)
{
  return this->siteNum;
}

int
FDPSite::FDPSiteGetRowNum(void)
{
  return this->rowNum;
}


void
FDPSite::FDPSiteRemoveCell(void)
{
  this->siteCell = NIL(Cell*);
}

/* Constructors and Destructors */
FDPSite::FDPSite()
{
  Cell *nullCell = NIL(Cell*);
  FDPSiteSetCell(nullCell);
  FDPSiteSetIsLocked(0);
  FDPSiteSetXpos(0);
  FDPSiteSetYpos(0);
  FDPSiteSetSiteNum(0);
  FDPSiteSetRowNum(0);  
}

