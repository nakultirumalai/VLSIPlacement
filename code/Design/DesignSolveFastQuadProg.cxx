# include <Design.h>
# include <mosek.h>

void
getObjFuncInQuadProgFormat(Design &myDesign, HyperGraph &myGraph,
			   vector<Cell *> &inputCells, 
			   int **qsubix, int **qsubjx, int **qsubx, 
			   int **qsubiy, int **qsubjy, int **qsuby, 
			   double **qvalijx, double **qvalx, 
			   double **qvalijy, double **qvaly, 
			   uint &numValuesQuad, uint &numValuesLin, 
			   double &constantx, double &constanty,
			   map<Cell *, uint>& quadMap,
			   map<Cell *, uint>& linMap)
{
  vector<uint> subi_vecx, subj_vecx, sub_vecx;
  vector<uint> subi_vecy, subj_vecy, sub_vecy;
  vector<double> valij_vecx, val_vecx;
  vector<double> valij_vecy, val_vecy;
  uint i;
  uint numVars = inputCells.size();

  getObjectiveCliqueModelXY(myDesign, myGraph, inputCells, 
			    subi_vecx, subj_vecx, valij_vecx, 
			    sub_vecx, val_vecx, 
			    subi_vecy, subj_vecy, valij_vecy, 
			    sub_vecy, val_vecy, 
			    constantx, constanty, quadMap, linMap);

  *qsubix = (int *) malloc(sizeof(int) * subi_vecx.size());
  *qsubiy = (int *) malloc(sizeof(int) * subi_vecy.size());
  *qsubjx = (int *) malloc(sizeof(int) * subj_vecx.size());
  *qsubjy = (int *) malloc(sizeof(int) * subj_vecy.size());
  *qvalijx = (double *) malloc(sizeof(double) * valij_vecx.size());
  *qvalijy = (double *) malloc(sizeof(double) * valij_vecy.size());
  *qsubx = (int *) malloc(sizeof(int) * sub_vecx.size());
  *qsuby = (int *) malloc(sizeof(int) * sub_vecy.size());
  *qvalx = (double *) malloc(sizeof(double) * numVars);
  *qvaly = (double *) malloc(sizeof(double) * numVars);

  //  cout << "subix\tsubjx\tvalijx\tsubiy\tsubjy\tvalijy" << endl;
  for (i = 0; i < subi_vecx.size(); i++) {
    (*qsubix)[i] = subi_vecx[i];
    (*qsubjx)[i] = subj_vecx[i];
    (*qvalijx)[i] = 2 * valij_vecx[i];
    (*qsubiy)[i] = subi_vecy[i];
    (*qsubjy)[i] = subj_vecy[i];
    (*qvalijy)[i] = 2 * valij_vecy[i];
    //    cout << (*qsubix)[i] << "\t" << (*qsubjx)[i] << "\t" 
    //	 << (*qvalijx)[i] << "\t" << (*qsubiy)[i] << "\t"
    //	 << (*qsubjy)[i] << "\t" << (*qvalijy)[i] << endl;
  }
  numValuesQuad = subi_vecx.size();

  //  cout << "subx\tvalx\tsuby\tvaly" << endl;
  numValuesLin = sub_vecx.size();
  for (i = 0; i < numValuesLin; i++) {
    (*qsubx)[i] = sub_vecx[i];
    (*qvalx)[sub_vecx[i]] = 2 * val_vecx[i];
    (*qsuby)[i] = sub_vecy[i];
    (*qvaly)[sub_vecy[i]] = 2 * val_vecy[i];
    //    cout << (*qsubx)[i] << "\t" << (*qvalx)[i] << "\t" 
    //	 << (*qsuby)[i] << "\t" << (*qvaly)[i] << endl;
  }
  for (i = numValuesLin; i < numVars; i++) {
    (*qvalx)[i] = 0.0;
    (*qvaly)[i] = 0.0;
  }

}

void
genGetVarBoundsInOOQPFormat(Design &myDesign, vector<Cell *>inputCells, 
			    char **blex, char **buex, double **bux, double **blx,
			    char **bley, char **buey, double **buy, double **bly)
{
  uint maxx, maxy;
  double upperBound, lowerBound, delta;
  unsigned int numCells;

  myDesign.DesignGetBoundingBox(maxx, maxy);
  numCells = inputCells.size();
  delta = 0.001;

  *blex = (char *) malloc(sizeof(char) * numCells);
  *buex = (char *) malloc(sizeof(char) * numCells);
  *blx = (double *) malloc(sizeof(double) * numCells);
  *bux = (double *) malloc(sizeof(double) * numCells);
  *bley = (char *) malloc(sizeof(char) * numCells);
  *buey = (char *) malloc(sizeof(char) * numCells);
  *bly = (double *) malloc(sizeof(double) * numCells);
  *buy = (double *) malloc(sizeof(double) * numCells);

  lowerBound = 0.0 + delta;
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *(inputCells[i]);
    upperBound = ((double)maxx)/GRID_COMPACTION_RATIO -
      ((double)thisCell.CellGetWidth()) / GRID_COMPACTION_RATIO;
    (*buex)[i] = 1;
    (*bux)[i] = upperBound;
    (*blex)[i] = 1;
    (*blx)[i] = lowerBound;
    upperBound = ((double)maxy)/GRID_COMPACTION_RATIO -
      ((double)thisCell.CellGetHeight()) / GRID_COMPACTION_RATIO;
    (*buey)[i] = 1;
    (*buy)[i] = upperBound;
    (*bley)[i] = 1;
    (*bly)[i] = lowerBound;
  }
}

void
Design::DesignSolveFastOOQP(void)
{
  void *cellObj;
  vector<Cell *> inputCells;
  map<Cell *, uint> quadMap;
  map<Cell *, uint> linMap;
  uint nx, nodeIdx;

  /* All variables related to the X value */
  int *qsubix, *qsubjx, *qsubx;
  char *blex, *buex; /* lower/upper Bound exists for x? 1 indicates yes and 0 indicates no */
  double *qvalijx, *qvalx;
  double *bux, *blx;
  double constantx;
  
  /* All variables related to the Y value */
  int *qsubiy, *qsubjy, *qsuby;
  char *bley, *buey; /* lower/upper Bound exist for y? 1 indicates yes and 0 indicates no */
  double *qvalijy, *qvaly;
  double *buy, *bly;
  double constanty;
  
  uint numValuesQuad, numValuesLin;

  HyperGraph &myGraph = DesignGetGraph();
  HYPERGRAPH_FOR_ALL_NODES(myGraph, nodeIdx, cellObj) {
    if ((*(Cell*)cellObj).CellIsTerminal()) continue;
    inputCells.push_back((Cell *)cellObj);
    cellsToSolve.push_back((Cell *)cellObj);
  } HYPERGRAPH_END_FOR;

  nx = inputCells.size();
  double x[nx];
  double y[nx];

  DesignSetCellsToSolve(inputCells);
  getObjFuncInOOQPFormat((*this), myGraph, inputCells, 
			 &qsubix, &qsubjx, &qsubx, 
			 &qsubiy, &qsubjy, &qsuby, 
			 &qvalijx, &qvalx, &qvalijy, &qvaly, 
			 numValuesQuad, numValuesLin, 
			 constantx, constanty, quadMap, linMap);
  genGetVarBoundsInOOQPFormat((*this), inputCells, &blex, &buex, &bux, &blx,
			      &bley, &buey, &buy, &bly);
  
  /* SOLVER SPECIFIC VARIABLES */
  uint my = 0; /* Number of equality constraints in Ax = B */
  uint mz = 0; /* Number of inequality constraints in Cx > D */
  uint nnzQ = numValuesQuad; /* Number of non-zero entries in the Hessian */
  uint nnzA = 0; /* Number of non-zero entries in the matrix A for eq. cstrs */
  uint nnzC = 0; /* Number of non-zero entries in the matrix C for ineq. cstrs */
  
  int *irowA = NIL(int *);
  int *jcolA = NIL(int *);
  double *dA = NIL(double *);
  double *b = NIL(double *);
  int *irowC = NIL(int *);
  int *jcolC = NIL(int *);
  double *dC = NIL(double *);
  double *clow = NIL(double *);
  char *iclow = NIL(char *);
  double *cupp = NIL(double *);
  char *icupp = NIL(char *);

  /* Generate the sparse matrix */
  QpGenSparseMa27 *qp = new QpGenSparseMa27(nx, my, mz, nnzQ, nnzA, nnzC);

  QpGenData *prob = (QpGenData *) qp->copyDataFromSparseTriple(qvalx, qsubix, nnzQ, qsubjx,  qvalijx,
							       blx,   blex,  bux,  buex,
							       irowA,  nnzA,   jcolA,  dA, b,
							       irowC,  nnzC,   jcolC,  dC,
							       clow,   iclow,  cupp,   icupp);

  int i;
  //  cout << "subix\tsubjx\tvalijx\tsubiy\tsubjy\tvalijy" << endl;
  for (i = 0; i < nnzQ; i++) {
    //    cout << (qsubix)[i] << "\t" << (qsubjx)[i] << "\t"
    //	 << (qvalijx)[i] << "\t" << (qsubiy)[i] << "\t"
    //	 << (qsubjy)[i] << "\t" << (qvalijy)[i] << endl;

  }
  cout << "subx\tvalx" << endl;
  for (i = 0; i < nx; i++) {
    cout << i << "\t" << qvalx[i] << endl;
  }

  exit(0);
  QpGenVars *vars = (QpGenVars *) qp->makeVariables(prob);
  QpGenResiduals *resid = (QpGenResiduals *) qp->makeResiduals(prob);
  MehrotraSolver *s = new MehrotraSolver(qp, prob);

  s->monitorSelf();
  int ierr = s->solve(prob, vars, resid);
  
  if( ierr == 0 ) {
    cout.precision(4);
    cout << "Solution: \n";
    vars->x->writefToStream( cout, "x[%{index}] = %{value}" );
  } else {
    cout << "Could not solve the problem.\n";
  }
}
