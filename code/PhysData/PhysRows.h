# ifndef ROWS_H
# define ROWS_H

# include <common.h>

/* Default definitions */
# define DEFAULT_COORDINATE 0
# define DEFAULT_SITE_ORIENTATION N
# define DEFAULT_SITE_SYMMETRY NO_SYMMETRY
# define DEFAULT_SITE_HEIGHT 0
# define DEFAULT_SITE_WIDTH 1
# define DEFAULT_SITE_SPACING 1
# define DEFAULT_NUM_SITES 0
# define DEFAULT_NUM_SUBROWS 0
# define DEFAULT_SUBROWS map<unsigned int, unsigned int> ()

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
   FN, FE, FS and FW. (think of a dial) */
typedef enum {N=1, E, S, W, FN, FE, FS, FW} siteOrientation;
typedef enum {NO_SYMMETRY=1, YES_SYMMETRY} siteSymmetry;

class PhysRow {
 private:
  int coordinate;
  siteOrientation siteOrient;
  siteSymmetry siteSym;
  rowOrientation rowType;
  unsigned int height;
  unsigned int siteWidth;
  unsigned int siteSpacing;
  unsigned int numSubRows;
  unsigned int numSites;
  map<unsigned int,unsigned int> subRows;

 public:
  /* Constructors */
  PhysRow();
  PhysRow(rowOrientation);
  PhysRow(rowOrientation, int);
  PhysRow(rowOrientation, unsigned int);
  PhysRow(rowOrientation, int, unsigned int);
  PhysRow(rowOrientation, int, unsigned int, unsigned int, unsigned int, unsigned int);
  PhysRow(rowOrientation, int, unsigned int, unsigned int, unsigned int, 
	  unsigned int, vector<int>, vector<unsigned int>);
  PhysRow(rowOrientation, int, unsigned int, vector<int>, vector<unsigned int>);

  /* Destructors */
  ~PhysRow();

  /* Get functions */
  int PhysRowGetCoordinate(void);
  siteOrientation PhysRowGetSiteOrientation(void);
  siteSymmetry PhysRowGetSiteSymmetry(void);
  rowOrientation PhysRowGetType(void);
  unsigned int PhysRowGetHeight(void);
  unsigned int PhysRowGetSiteWidth(void);
  unsigned int PhysRowGetSiteSpacing(void);
  unsigned int PhysRowGetNumSubRows(void);
  unsigned int PhysRowGetNumSites(void);
  map<unsigned int,unsigned int> PhysRowGetSubRows(void);

  /* Set functions */
  void PhysRowSetCoordinate(int);
  void PhysRowSetType(rowOrientation);
  void PhysRowSetSiteOrientation(siteOrientation);
  void PhysRowSetSiteSymmetry(siteSymmetry);
  void PhysRowSetHeight(unsigned int);
  void PhysRowSetSiteWidth(unsigned int);
  void PhysRowSetSiteSpacing(unsigned int);
  void PhysRowSetNumSites(unsigned int);
  void PhysRowSetNumSubRows(unsigned int);
  void PhysRowSetSubRows(map<unsigned int, unsigned int>);
  void PhysRowIncrementSubRows(void);
  
  /* Add a subrow to an existing row */
  void PhysRowAddSubRow(unsigned int, unsigned int);
};

# endif

