# include <PhysRows.h>

void
PhysRow::PhysRowSetCoordinate(int coordinate)
{
  this->coordinate = coordinate;
}

void
PhysRow::PhysRowSetType(rowOrientation rowType)
{
  this->rowType = rowType;
}

void
PhysRow::PhysRowSetHeight(unsigned int height)
{
  this->height = height;
}

void
PhysRow::PhysRowSetSiteWidth(unsigned int siteWidth)
{
  this->siteWidth = siteWidth;
}

void
PhysRow::PhysRowSetSiteSpacing(unsigned int siteSpacing)
{
  this->siteWidth = siteSpacing;
}

void
PhysRow::PhysRowSetSiteOrientation(siteOrientation orient)
{
  this->siteOrient = orient;
}

void
PhysRow::PhysRowSetSiteSymmetry(siteSymmetry symmetry)
{
  this->siteSym = symmetry;
}

void
PhysRow::PhysRowSetSubRows(map<unsigned int, unsigned int> subRows)
{
  this->subRows = subRows;
}

void
PhysRow::PhysRowSetNumSubRows(unsigned int numSubRows)
{
  this->numSubRows = numSubRows;
}

void
PhysRow::PhysRowIncrementSubRows(void)
{
  this->numSubRows++;
}

int
PhysRow::PhysRowGetCoordinate(void)
{
  return coordinate;
}

siteOrientation
PhysRow::PhysRowGetSiteOrientation(void)
{
  return (siteOrient);
}

siteSymmetry
PhysRow::PhysRowGetSiteSymmetry(void)
{
  return siteSym;
}

rowOrientation
PhysRow::PhysRowGetType(void)
{
  return rowType;
}

unsigned int
PhysRow::PhysRowGetHeight(void)
{
  return height;
}

unsigned int
PhysRow::PhysRowGetSiteWidth(void)
{
  return siteWidth;
}

unsigned int 
PhysRow::PhysRowGetSiteSpacing(void)
{
  return siteSpacing;
}

unsigned int
PhysRow::PhysRowGetNumSubRows(void)
{
  return numSubRows;
}

map<unsigned int, unsigned int>
PhysRow::PhysRowGetSubRows(void)
{
  map<unsigned int, unsigned int> subRows;
  
  subRows = this->subRows;
  
  return subRows;
}

void
PhysRow::PhysRowAddSubRow(unsigned int rowOrigin, unsigned int numSites) 
{
  this->subRows[rowOrigin] = numSites;
  this->numSites += numSites;
}

/* Constructors begin */
PhysRow::
PhysRow(rowOrientation orient)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
}

PhysRow::PhysRow(rowOrientation orient, int coordinate)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
}

PhysRow::PhysRow(rowOrientation orient, unsigned int height)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 unsigned int siteWidth, unsigned int siteSpacing, unsigned int numSubRows)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(DEFAULT_COORDINATE);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(DEFAULT_SITE_HEIGHT);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(DEFAULT_SUBROWS);
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 unsigned int siteWidth, unsigned int siteSpacing, 
		 unsigned int numSubRows, map<unsigned int, unsigned int> subRows)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(height);
  PhysRowSetSiteSpacing(siteSpacing);
  PhysRowSetSiteWidth(siteWidth);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(numSubRows);
  PhysRowSetSubRows(subRows);  
}

PhysRow::PhysRow(rowOrientation orient, int coordinate, unsigned int height,
		 map<unsigned int, unsigned int> subRows)
{
  PhysRowSetType(orient);
  PhysRowSetCoordinate(coordinate);
  PhysRowSetSiteOrientation(DEFAULT_SITE_ORIENTATION);
  PhysRowSetSiteSymmetry(DEFAULT_SITE_SYMMETRY);
  PhysRowSetSiteHeight(height);
  PhysRowSetSiteSpacing(DEFAULT_SITE_SPACING);
  PhysRowSetSiteWidth(DEFAULT_SITE_WIDTH);
  PhysRowSetSiteNumSites(DEFAULT_NUM_SITES);
  PhysRowSetSiteNumSubRows(DEFAULT_NUM_SUBROWS);
  PhysRowSetSubRows(subRows);
}
