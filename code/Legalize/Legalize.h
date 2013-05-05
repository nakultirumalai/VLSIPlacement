# ifndef LEGALIZE_H
# define LEGALIZE_H

# include <common.h>
# include <PhysRow.h>
# include <Cell.h>
# include <Pin.h>
# include <lemon/list_graph.h>
# include <lemon/network_simplex.h>
# include <Design.h>
# include <lemon/bfs.h>
# include <lemon/concepts/digraph.h>
# include <Flags.h>

/* Default definitions */
# define ArcMap ListDigraph::ArcMap
# define NodeMap ListDigraph::NodeMap

using namespace lemon;

/*
  
*/

/* 
   
*/
void LegalizeGetAllBoundingBoxes(vector<PhysRow*>& , vector<vector<int> >& );

void LegalizeCheckLegality(Cell* , vector<PhysRow*>&, vector<vector<unsigned int> >&);

void LegalizeSnapToNearestRows(Cell* , vector<PhysRow*>& , rowOrientation);

void findRowCoordinates(vector<int>&, int , int , int , int nearRows[]);

void LegalizeConstructGNFGraph(vector<PhysRow*> &, ListDigraph &, ArcMap<int> &, NodeMap<int> &);

void LegalizeDesign(Design &);

void LegalizeCalculateMinCostFlow(ListDigraph &, ArcMap<int> &, NodeMap<int> &);

void LegalizeTestMinCostFlow(void);

void LegalizeFindZonesInRow(PhysRow*, vector<pair<int,unsigned int> > &,  
			    unsigned int, unsigned int);

void LegalizeFindRegionsInRow(vector<pair<int,unsigned int> > &,  
			      vector<pair<int,unsigned int> > &, PhysRow* &, int);
# endif

