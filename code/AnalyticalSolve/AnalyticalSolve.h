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

# define SOLVE_FOR_X 0
# define SOLVE_FOR_Y 1
# define SOLVE_FOR_BOTH 2

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

void
genGetVarBounds(Design &myDesign, vector<Cell *> inputCells,
                MSKidxt **subb, MSKrealt **bu, MSKrealt **bl,
                MSKboundkeye **bk, uint &numVarBounds,
                char solverVar);
# endif
