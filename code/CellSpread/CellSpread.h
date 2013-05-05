# ifndef CELL_SPREAD_H
# define CELL_SPREAD_H
# include <Design.h>
# include <Cell.h>
# include <Pin.h>
# include <Net.h>
# include <HyperGraph.h>
# include <common.h>

# define XDIRECTION 0
# define YDIRECTION 1

class Design;

void
CellSpreadInDesign(Design &myDesign, HyperGraph &myGraph, 
		   vector<Cell *> cellsSortedByDir, double alpha,
                   uint windowWidth, char dir, uint &lastLinePos);

# endif
