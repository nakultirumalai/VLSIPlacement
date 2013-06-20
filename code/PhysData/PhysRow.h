# ifndef PHYSROW_H
# define PHYSROW_H

# include <common.h>
# include <Cell.h>
# include <Flags.h>
/* Default definitions */
# define DEFAULT_COORDINATE 0
# define DEFAULT_SITE_ORIENTATION N
# define DEFAULT_SITE_SYMMETRY NO_SYMMETRY
# define DEFAULT_SITE_HEIGHT 0
# define DEFAULT_SITE_WIDTH 1
# define DEFAULT_SITE_SPACING 1
# define DEFAULT_NUM_SITES 0
# define DEFAULT_NUM_SUBROWS 0
# define DEFAULT_TOTAL_CELL_WIDTH 0
# define DEFAULT_TOTAL_BOUNDINGBOX_WIDTH 0
# define DEFAULT_BLOCKED_WIDTH 0
# define DEFAULT_SUBROWS map<unsigned int, unsigned int> ()
# define DEFAULT_ROW_BEGIN 0

using namespace std;

/*
  Row definition in the bookshelf format;
        CoreRow Horizontal
	Coordinate :    www  # row starts at coordinate www
	# (y-coordinate if horizontal, x-coordinate if vertical row)
	Height :        yyy  # row height is yyy
	Sitewidth :     1    
	Sitespacing :   1    # distance between the left edges of two adjacent sites
	Siteorient :    1
	Sitesymmetry :  1
       # You can specify subrows within a row. They following says the subrow spans from
# aaa to (aaa + (bbb-1)*Sitespacing + Sitewidth) with yyy height.
	SubrowOrigin :  aaa  NumSites : bbb
       # Additional Subrows may be specified if there are breaks in the row
# due to obstacles, but are not required. One subrow is always required.
*/

typedef enum {VERTICAL,HORIZONTAL} rowOrientation;

/* How site orientation is defined:
   the default orientation is "vertically
   and face up" - N (North). Rotate by 90deg 
   clockwise to get E, S and W, flip to get 
   FN, FE, FS and FW. (think of a dial) 
*/

typedef enum {NO_SYMMETRY=1, YES_SYMMETRY} siteSymmetry;

class PhysRow {
 private:
  int rowBegin;
  int rowEnd;
  int coordinate;
  objOrient siteOrient;
  siteSymmetry siteSym;
  rowOrientation rowType;
  unsigned int height;
  unsigned int siteWidth;
  unsigned int siteSpacing;
  unsigned int numSubRows;
  unsigned int numSites;
  map<unsigned int,unsigned int> subRows;
  bool containsFixed;
  //vector<vector<Cell*> > allCellsInRow;
  vector<Cell*> cellsInRow;
  int totalCellWidth;
  int blockedWidth;
  int totalBoundingBoxWidth;
  int wMax;
  int rowIndex;

 public:
  /* Constructors */
  PhysRow();
  PhysRow(rowOrientation);
  PhysRow(rowOrientation, int);
  PhysRow(rowOrientation, unsigned int);
  PhysRow(rowOrientation, int, unsigned int);
  PhysRow(rowOrientation, int, unsigned int, unsigned int, unsigned int);
  PhysRow(rowOrientation, int, unsigned int, unsigned int, unsigned int, objOrient, 
	  siteSymmetry);
  PhysRow(rowOrientation, int, unsigned int, unsigned int, unsigned int, unsigned int, 
	  map<unsigned int, unsigned int>);
  PhysRow(rowOrientation, int, unsigned int, map<unsigned int, unsigned int>);

  /* Destructors */
  ~PhysRow();

  /* Get functions */
  int PhysRowGetCoordinate(void);
  objOrient PhysRowGetSiteOrientation(void);
  string PhysRowGetSiteOrientationStr(void);
  siteSymmetry PhysRowGetSiteSymmetry(void);
  string PhysRowGetSiteSymmetryStr(void);
  rowOrientation PhysRowGetType(void);
  string PhysRowGetTypeStr(void);
  unsigned int PhysRowGetHeight(void);
  unsigned int PhysRowGetSiteWidth(void);
  unsigned int PhysRowGetSiteSpacing(void);
  unsigned int PhysRowGetNumSubRows(void);
  unsigned int PhysRowGetNumSites(void);
  int PhysRowGetTotalCellWidth(void);
  int PhysRowGetBoundingBoxWidth(void);
  int PhysRowGetBlockedWidth(void);
  int PhysRowGetWMax(void);
  map<unsigned int,unsigned int> PhysRowGetSubRows(void);
  void PhysRowGetBoundingBox(vector<int>& );
  void PhysRowGetBoundingBox(int &, int&, int&, int&);
  void PhysRowGetCellsInRow(vector<Cell*> &); 
  int PhysRowGetRowBegin(void);
  int PhysRowGetIndex(void);

  /* Set functions */
  void PhysRowSetCoordinate(int);
  void PhysRowSetType(rowOrientation);
  void PhysRowSetSiteOrientation(objOrient);
  void PhysRowSetSiteSymmetry(siteSymmetry);
  void PhysRowSetHeight(unsigned int);
  void PhysRowSetSiteWidth(unsigned int);
  void PhysRowSetSiteSpacing(unsigned int);
  void PhysRowSetNumSites(unsigned int);
  void PhysRowSetNumSubRows(unsigned int);
  void PhysRowSetSubRows(map<unsigned int, unsigned int>);
  void PhysRowIncrementSubRows(void);
  void PhysRowSetTotalCellWidth(int);
  void PhysRowSetBoundingBoxWidth(int);
  void PhysRowSetBlockedWidth(int);
  void PhysRowCalculateWMax(void);
  void PhysRowSetRowBegin(int);
  void PhysRowSetIndex(int);
  
  /* Add a subrow to an existing row */
  void PhysRowAddSubRow(unsigned int, unsigned int);

   /* Add a cell to a subrow of an existing row */
  void PhysRowAddCellToSubRow(Cell* &, unsigned int);
  
  /* Add a cell to a row */ 
  void PhysRowAddCellToRow(Cell* &);
  void PhysRowRemoveCellFromRow(Cell* &);
  
  /* Add a zone to row */
  void PhysRowAddZoneToRow(int, int);
  
  /* Find zones in a row 
  void PhysRowFindZonesInRow(void);

  Print all the zones in a row 
  void PhysRowPrintAllZones(void);
  */
  void PhysRowMarkFixedCellsInRow(int);
  int PhysRowCalculateRowEnd(void);
  void PhysRowRemoveAllCells(void);
};

extern rowOrientation PhysRowGetRowTypeFromStr(string);
extern siteSymmetry PhysRowGetSiteSymmetryFromStr(string);
# endif

