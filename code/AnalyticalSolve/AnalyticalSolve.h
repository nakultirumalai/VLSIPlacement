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

# define GRID_COMPACTION_RATIO 1000
# define PULL_FACTOR 100.0
# define MAXWL_CSTR 225.00

class Design;

/* 
   Define function pointers which are going to be 
   plugged to the mosek solver that work to build the matrices 
*/
typedef void (*cstrBoundFunc)(Design& myDesign, HyperGraph& myGraph,
			      vector<Cell*>& listOfCells, 
			      MSKidxt **sub, double *cstrUBounds, 
			      MSKidxt **subj, MSKrealt **bu, MSKrealt **bl,
			      MSKboundkeye **bk, unsigned int& numCstrs);

typedef void (*varBoundFunc)(Design& myDesign, HyperGraph& myGraph,
			     vector<Cell*>& listOfCells,
			     MSKidxt **subj, MSKrealt **bu, MSKrealt **bl,
			     MSKboundkeye **bk, unsigned int& numNonZero);

typedef void (*quadObjFunc)(Design& myDesign, HyperGraph& myGraph,
			    vector<Cell*>& listOfCells, 
			    MSKidxt **qsubi, MSKidxt **qsubj, MSKrealt **qval,
			    unsigned int& numNonZero);

typedef void (*linObjFunc)(Design& myDesign, HyperGraph& myGraph, 
			   vector<Cell*>& listOfCells, 
			   MSKidxt **subi, MSKrealt **val, double &constant, 
			   unsigned int& numNonZero);

typedef void (*quadCstrFunc)(Design& myDesign, HyperGraph& myGraph,
			     vector<Cell*>& listOfCells,
			     MSKidxt **qcsubk, MSKidxt **qcsubi, MSKidxt **qcsubj, 
			     MSKrealt **qcval, unsigned int &numNonZero);

typedef void (*linCstrFunc)(Design& myDesign, HyperGraph& myGraph,
			    vector<Cell*>& listOfCells,
			    MSKidxt **sub, MSKintt **ptrb, MSKintt **ptre, MSKidxt **asub,
			    MSKrealt **aval, double **acstrUpperBnds, 
			    unsigned int& numNonZero);

extern vector<double>
mosekSolveQCQO(Design& myDesign, HyperGraph& myGraph, 
	       vector<Cell*> listOfCells, unsigned int numVars, 
	       unsigned int numCstrs, quadObjFunc qof, linObjFunc lof, 
	       quadCstrFunc qcstrf, linCstrFunc lcstrf, varBoundFunc vbf,
	       cstrBoundFunc cbf); 

extern vector<double>
mosekSolveLCQO(Design& myDesign, HyperGraph& myGraph, 
	       vector<Cell*> listOfCells, unsigned int numVars, 
	       unsigned int numCstrs, quadObjFunc qof, linObjFunc lof, 
	       linCstrFunc lcstrf, varBoundFunc vbf, cstrBoundFunc cbf); 

extern void seqSolveQuadratic(Design&, HyperGraph&, vector<Cell *>&);

/* FUNCTIONS THAT ARE COMMON TO ALL THE FLAVORS OF THE SOLVER */
void mskGetVarBounds(Design& myDesign, HyperGraph& myGraph, 
		     vector<Cell*>& listOfCells,
		     MSKidxt **subj, MSKrealt **bu, MSKrealt **bl,
		     MSKboundkeye **bk, unsigned int& numNonZero);

/* FUNCTIONS OF SeqSolveTD.cxx */
void
seqGetCstrBoundsTD(Design&, HyperGraph&, vector<Cell*>&,
		   MSKidxt**, double*, MSKidxt**, MSKrealt**, 
		   MSKrealt**, MSKboundkeye**, unsigned int&);
void
seqGetObjectiveMatrixTD(Design&, HyperGraph&, vector<Cell *>&, 
			MSKidxt**, MSKidxt**, MSKrealt**, 
			unsigned int &);
void
seqGetObjectiveLinearArrayTD(Design&, HyperGraph&,  vector<Cell*>&, 
			     MSKidxt**, MSKrealt**, double &, 
			     unsigned int &);
void
seqGetQuadraticCstrMatrixTD(Design&, HyperGraph&, vector<Cell *>&, MSKidxt**, 
			    MSKidxt**, MSKidxt**, MSKrealt**, unsigned int &);
void
seqGetLinearCstrArrayTD(Design&, HyperGraph&, vector<Cell*>&, MSKidxt**, MSKintt**, 
			MSKintt**, MSKidxt**, MSKrealt**, double**, unsigned int &);
void
seqSolveQuadraticTD(Design&, HyperGraph&, vector<Cell *>&) ;

/* FUNCTIONS OF SeqSolveWL.cxx */
void
seqGetCstrBoundsWL(Design&, HyperGraph&, vector<Cell*>&, MSKidxt**, 
		   double *cstrUBounds, MSKidxt**, MSKrealt**, MSKrealt**,
		   MSKboundkeye**, unsigned int&);
void
seqGetObjectiveMatrixWL(Design&, HyperGraph&, vector<Cell *>&, 
			MSKidxt**, MSKidxt**, MSKrealt**, unsigned int &);

void
seqGetObjectiveLinearArrayWL(Design&, HyperGraph&, vector<Cell*>&, MSKidxt**, 
			     MSKrealt**, double&, unsigned int &);

void
seqGetQuadraticCstrMatrixWL(Design&, HyperGraph&, vector<Cell *>&, MSKidxt**, 
			    MSKidxt**, MSKidxt**, MSKrealt**, unsigned int &);
void 
seqGetLinearCstrArrayWL(Design&, HyperGraph&, vector<Cell*>&, MSKidxt**, MSKintt**, 
			MSKintt**, MSKidxt**, MSKrealt**, double**, unsigned int &);
void 
seqSolveQuadraticWL(Design&, HyperGraph&, vector<Cell *>&);

#endif

