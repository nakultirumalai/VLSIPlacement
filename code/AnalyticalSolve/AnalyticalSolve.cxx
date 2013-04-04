# include <AnalyticalSolve.h>
# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <QuadProg++.hh>

# define MAXX 120.00
# define MAXY 120.00
# define RPERLENGTHX 0.0001
# define CPERLENGTHX 0.0000001
# define RPERLENGTHY 0.0001
# define CPERLENGTHY 0.0000001
# define TCLK 2.00

using namespace QuadProgPP;
Matrix<double>
createGMatrixXY(vector<Cell *> listOfCells, HyperGraph& connectivityGraph, 
		map<unsigned int, pair<Pin*, Pin* > >& pinsOfEdges)
{
  int i, j;
  double coeffuv, coeffu;
  Cell *cellPtri, *cellPtrj;
  Matrix<double> G;
  //  int numCells = listOfCells.size();
  int numCells = 5;

  G.resize(2 * numCells, 2 * numCells);
  for (i=0;i<numCells;i++) {
    cellPtri = listOfCells[i];
    for (j=0;j<numCells;j++) {
      cellPtrj = listOfCells[j];
      G[i][j] = 0.0000000000;
      if (i == j) {
	unsigned int edgeIdx;
	unsigned int edgeWeight;
	double weight = 0;
	HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight) {
	  weight += edgeWeight;
	} HYPERGRAPH_END_FOR;
	G[i][i] = (RPERLENGTHX) * (CPERLENGTHX) * (0.5) * weight;
	G[i+numCells][i+numCells] = (RPERLENGTHY) * (CPERLENGTHY) * (0.5) * weight;
      } else {
	unsigned int edgeIdx;
	unsigned int edgeWeight;
	double weight = 0;
	HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECTS(connectivityGraph, cellPtri, cellPtrj, edgeIdx, edgeWeight) {
	  weight += edgeWeight;
	} HYPERGRAPH_END_FOR;
	G[i][j] = (RPERLENGTHX) * (CPERLENGTHX) * (0.5) * weight;
	G[i+numCells][j+numCells] = (RPERLENGTHX) * (CPERLENGTHX) * (0.5) * weight;
      }
    }
  }
  if (0) {
    for (int i = 0; i < (2 * numCells); i++) {
      for (int j = 0; j < (2 * numCells); j++) {
	cout << G[i][j] << " ";
      }
      cout << endl;
    }
  }
  return (G);
}


Vector<double>
createg0MatrixXY(vector<Cell *> listOfCells, HyperGraph& connectivityGraph,
		 map<unsigned int, pair<Pin*,Pin* > >& pinsOfEdges)
{
  Vector<double> g0;
  // int numCells = listOfCells.size();
  int numCells = 5;
  unsigned int weightedSum;

  g0.resize(2 * numCells);
  for (int i = 0; i < numCells; i++) {
    Cell* cellPtri = (Cell *)listOfCells[i];
    unsigned int cellIdx = connectivityGraph.HyperGraphGetCellIndex(cellPtri);
      
    unsigned int edgeIdx;
    unsigned int edgeWeight;
    double coeffVal1, coeffVal2;
    coeffVal1 = 0;
    coeffVal2 = 0;
    cout << "Cell " << (*cellPtri).CellGetName() << " " << i << endl;
    HYPERGRAPH_FOR_ALL_EDGES_OF_OBJECT(connectivityGraph, cellPtri, edgeIdx, edgeWeight) {
      pair<Pin*, Pin*>& pinPairOnEdge = pinsOfEdges[edgeIdx];
      unsigned int srcXOffset = (*(Pin*)pinPairOnEdge.first).PinGetXOffset();
      unsigned int srcYOffset = (*(Pin*)pinPairOnEdge.first).PinGetYOffset();
      unsigned int destXOffset = (*(Pin*)pinPairOnEdge.second).PinGetXOffset();
      unsigned int destYOffset = (*(Pin*)pinPairOnEdge.second).PinGetYOffset();
      coeffVal1 += RPERLENGTHX * CPERLENGTHX * edgeWeight * (destXOffset - srcXOffset);
      coeffVal2 += RPERLENGTHY * CPERLENGTHY * edgeWeight * (destYOffset - srcYOffset);
    } HYPERGRAPH_END_FOR;
    g0[i] = coeffVal1;
    g0[i + numCells] = coeffVal2;
  }

  for (int j = 0; j < numCells; j++) {
    printf("%f   ", g0[j]);
  }
}


void
createInequalityCstrs(Matrix<double>& CI, Vector<double>& ci0 , 
		      vector<Cell *>& listOfCells, HyperGraph& connectivityGraph, 
		      map<unsigned int, pair<Pin*,Pin* > >& pinsOfEdges)
{
  unsigned int numCstrs = connectivityGraph.GetNumEdges();
  unsigned int numCells = listOfCells.size();
  int cstrNum;
  int i, j, k;

  /* Create CI first */
  for (k = 0; k < numCstrs; k++) {
    for (i = 0; i < numCells; i++) {
      Cell *srcCell = listOfCells[i];
      for (j = 0; j < numCells; j++) {
	Cell *destCell = listOfCells[j];
	if (connectivityGraph.HyperGraphNodesAreAdjacent(srcCell, destCell) > 0) {
	  CI[i][k] = 1;
	  CI[j][k] = -1;
	  CI[i+numCells][k] = 1;
	  CI[j+numCells][k] = -1;
	}
      }
    }
    ci0[k] = -(sqrt((0.75 * TCLK)));
    ci0[k+numCells] = -(sqrt((0.25 * TCLK)));
  }
}


int 
generateSolutionUsingQuadProg(vector<Cell *> listOfCells, 
			      HyperGraph& connectivityGraph, 
			      map<unsigned int, pair<Pin*,Pin* > >& pinsOfEdges)
{
  double solutionCost, sumOfWeights;
  unsigned int numCells = listOfCells.size();
  unsigned int numVars;
  int i,j;

  Matrix<double> G, Gcpy, CE, CI;
  Vector<double> g0, ce0, ci0, x, y;
  
  _STEP_BEGIN("Analytical solve for sequential cells");  
  _STEP_BEGIN("Create matrix for given graph and list of cells");

  printf(" Printing graph:");
  
  Cell *cellPtr;
  //  VECTOR_FOR_ALL_ELEMS(listOfCells, Cell*, cellPtr) {
    //    cout << " Node Name: " << (*cellPtr).CellGetName() << endl;
    //    cout << " Number of connected Nodes: " << connectivityGraph.GetNumPairsOfNode(cellPtr) << endl;
  //  } END_FOR;

  numVars = 2 * numCells;
  /* For variables */
  G.resize(numVars, numVars);
  g0.resize(numVars);
  x.resize(numVars);

  /* For constraints */
  /* For every pair of cells that are connected i.e. 
     equal to the number of edges in the graph, 
     there is a row of constraints */
  unsigned int numEqualCstrs = connectivityGraph.GetNumEdges();
  unsigned int edgeIdx;
  ce0.resize(2 * numEqualCstrs);
  CE.resize(2 * numEqualCstrs, 2 * numCells);

  CI.resize(2 * numCells, 2 * numEqualCstrs);
  ci0.resize(2 * numEqualCstrs);
  
  /* Create the G & g0 matrix */
  G = createGMatrixXY(listOfCells, connectivityGraph, pinsOfEdges);
  g0 = createg0MatrixXY(listOfCells, connectivityGraph, pinsOfEdges);

  /* Create the inequality constraints */
  createInequalityCstrs(CI, ci0, listOfCells, connectivityGraph, pinsOfEdges);
  
  _STEP_END("Create matrix for given graph and list of cells");
  
  solve_quadprog(G, g0, CE, ce0, CI, ci0, x);
# if 0
  solve_quadprog(Gcpy, g0, CE, ce0, CI, ci0, y);
# endif
  _STEP_END("Analytical solve for sequential cells");  
}
