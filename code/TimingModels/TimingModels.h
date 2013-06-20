# ifndef TIMING_MODELS_H
# define TIMING_MODELS_H

# include <Design.h>
# include <Cell.h>
# include <Pin.h>
# include <Net.h>
# include <HyperGraph.h>
# include <common.h>

class Design;

/*******************************************************************************
  a QUADRATIC objective, a LINEAR objective function, 
  a QUAD constraint formulator and a LINEAR constraint formulator
*******************************************************************************/
/* Get the objective and linear functions for timing driven placement */

/* Internal functions */
map<Cell *, uint> getCellLookupMap(vector<Cell *>inputCells);

/* Clique model */

/* Objective function both linear and quadratic */
void
getObjectiveCliqueModel(Design &myDesign, HyperGraph &myGraph,
                        vector<Cell*> &inputCells, vector<uint> &subi_vec,
                        vector<uint> &subj_vec, vector<double> &valij_vec,
                        vector<uint> &sub_vec, vector<double> &val_vec,
                        double& constant);
/* Only for the x-variables of cells */
void
getObjectiveCliqueModelX(Design &myDesign, HyperGraph &myGraph,
			 vector<Cell*> &inputCells, vector<uint> &subi_vec,
			 vector<uint> &subj_vec, vector<double> &valij_vec,
			 vector<uint> &sub_vec, vector<double> &val_vec,
			 double& constant);
/* Only for the y-variables of cells */
void
getObjectiveCliqueModelY(Design &myDesign, HyperGraph &myGraph,
			 vector<Cell*> &inputCells, vector<uint> &subi_vec,
			 vector<uint> &subj_vec, vector<double> &valij_vec,
			 vector<uint> &sub_vec, vector<double> &val_vec,
			 double& constant);

void
getObjectiveCliqueModelXY(Design &myDesign, HyperGraph &myGraph,
			  vector<Cell*> &inputCells, vector<uint> &subi_vecx, 
			  vector<uint> &subj_vecx, vector<double> &valij_vecx, 
			  vector<uint> &sub_vecx, vector<double> &val_vecx, 
			  vector<uint> &subi_vecy, vector<uint> &subj_vecy, 
			  vector<double> &valij_vecy, vector<uint> &sub_vecy, 
			  vector<double> &val_vecy, double& constantx, 
			  double &constanty, map<Cell *, uint>& quadMap,
			  map<Cell *, uint>& linMap);
void
getObjectiveCliqueModelXYForCluster(vector<Net*> &internalNets, 
				    vector<Cell*> &inputCells,
                                    vector<uint> &subi_vecx, vector<uint> &subj_vecx,
                                    vector<double> &valij_vecx,
                                    vector<uint> &sub_vecx, vector<double> &val_vecx,
                                    vector<uint> &subi_vecy, vector<uint> &subj_vecy,
                                    vector<double> &valij_vecy,
                                    vector<uint> &sub_vecy, vector<double> &val_vecy,
                                    double& constantx, double &constanty);


/* Star model */

/* Objective function both linear and quadratic */
void
getObjectiveStarModelXY(Design &myDesign, HyperGraph &myGraph,
			vector<Cell*> &inputCells, vector<uint> &subi_vecx, 
			vector<uint> &subj_vecx, vector<double> &valij_vecx, 
			vector<uint> &sub_vecx, vector<double> &val_vecx, 
			vector<uint> &subi_vecy, vector<uint> &subj_vecy, 
			vector<double> &valij_vecy, vector<uint> &sub_vecy, 
			vector<double> &val_vecy, double& constantx, 
			double &constanty, map<Cell *, uint> &quadMap,
			map<Cell *, uint> &linMap);

/* Hybrid model */
void
getObjectiveHybridModel(Design &myDesign, HyperGraph &myGraph,
                        vector<Cell*> &inputCells, vector<uint> &subi_vec,
                        vector<uint> &subj_vec, vector<double> &valij_vec,
                        vector<uint> &sub_vec, vector<double> &val_vec,
                        double& constant);

/* Any other custom model that we plan to try out */
void
getObjectiveCustomModel(Design &myDesign, HyperGraph &myGraph,
                        vector<Cell*> &inputCells, vector<uint> &subi_vec,
                        vector<uint> &subj_vec, vector<double> &valij_vec,
                        vector<uint> &sub_vec, vector<double> &val_vec,
                        double& constant);
# endif
