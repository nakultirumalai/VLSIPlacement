# ifndef ANALYTICALSOLVE_H
# define ANALYTICALSOLVE_H

# include <stdlib.h>
# include <common.h>
# include <Cell.h>
# include <PhysRow.h>
# include <Sort.h>
# include <Flags.h>
# include <DesignIter.h>
# include <QuadProg++.hh>

class Design;

//extern int 
//generateSolutionUsingQuadProg(vector<Cell *>, HyperGraph&, map<unsigned int, pair<Pin*, Pin* > > &);
extern void
mosekSolveQuadratic(Design&, vector<Cell *>&, vector<Cell *>&, HyperGraph&, 
		    map<unsigned int, pair<Pin*, Pin* > > &);

#endif
