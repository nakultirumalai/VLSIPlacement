# include <ForceDirectedPlacer.h>

/* Set functions */
void
FDPSiteSetHasCell(Cell *thisCell)
{
  (this)->siteHasCell = thisCell;
}

void
FDPSiteSetIsLocked(bool islocked)
{
  (this)->siteIsLocked = isLocked;
}

void
FDPSiteSetXpos(int siteX)
{
  (this)->siteX = siteX;
}

void
FDPSiteSetYpos(int siteY)
{
  (this)->siteY = siteY;
}

void
FDPSiteSetSiteNum(int siteNum)
{
  (this)->siteNum = siteNum;
}

void
FDPSiteSetRowNum(int rowNum)
{
  (this)->rowNum = rowNum;
}


/* Get Functions */

Cell*
FDPSiteGetHasCell(void) 
{
  return (this)->siteHasCell;
}

bool
FDPSiteGetIsLocked(void)
{
  return (this)->siteIsLocked;
}

int
FDPSiteGetXpos(void)
{
  return (this)->siteX;
}

int
FDPSiteGetYpos(void)
{
  return (this)->siteY;
}

int
FDPSiteGetSiteNum(void)
{
  return (this)->siteNum;
}

int
FDPSiteGetRowNum(void)
{
  return (this)->rowNum;
}


void
FDPSiteRemoveCell(void)
{
  (this)->siteHasCell = NIL(Cell*);
}

/* Constructors and Destructors */
FDPSite::FDPSite()
{
  Cell *nullCell = NIL(Cell*);
  FDPSiteSetHasCell(nullCell);
  FDPSiteSetIsLocked(0);
  FDPSiteSetXpos(0);
  FDPSiteSetYpos(0);
  FDPSiteSetSiteNum(0);
  FDPSiteSetRowNum(0);  
}

