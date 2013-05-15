# ifndef ANALYTICALSOLVE_H
# define ANALYTICALSOLVE_H

# include <stdlib.h>
# include <common.h>
# include <Cell.h>
# include <PhysRow.h>
# include <Sort.h>
# include <Flags.h>
# include <DesignIter.h>
# include <HyperGraph.h>
# include <TimingModels.h>

# define GRID_COMPACTION_RATIO 1000

class Design;

/* Quadratic minimizer. Minimizes quadratic distance between the 
   cells provided */
void genSolve(Design&, HyperGraph&, vector<Cell *>&);
void genSolveX(Design&, HyperGraph&, vector<Cell *>&);
void genSolveY(Design&, HyperGraph&, vector<Cell *>&);

/* Internal functions to get the bounds for X and Y variables respectively*/
void mskGetVarBoundsX(Design&, vector<Cell*>&,
		      vector<uint> &, vector<double> &, 
		      vector<double> &, vector<MSKboundkeye>&);

void mskGetVarBoundsY(Design&, vector<Cell*>&,
		      vector<uint> &, vector<double> &, 
		      vector<double> &, vector<MSKboundkeye>&);

# endif
