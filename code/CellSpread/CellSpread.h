# ifndef CELL_SPREAD_H
# define CELL_SPREAD_H
# include <Design.h>
# include <Cell.h>
# include <Pin.h>
# include <Net.h>
# include <HyperGraph.h>
# include <common.h>
# include <Bin.h>

class Design;

void
CellSpreadInDesign(Design &myDesign, HyperGraph &myGraph, 
		   vector<Cell *> cellsSortedByDir, double alpha,
                   uint windowWidth, char dir, uint &lastLinePos);

double
CellSpreadInDesignFastPlace(Design &myDesign, HyperGraph &myGraph);
# endif
