# ifndef CONJGRAD_SOLVER_H
# define CONJGRAD_SOLVER_H
# include <common.h>

/* Sparse matrix container which stores only the 
   lower triangular half of a positive semidefinite matrix */
class SpMat {
 private:
  uint numNonZero;
  uint numDiagElems;
  uint *idxi;
  uint *idxj;
  double *val;
 public:
  SpMat();
  SpMat(uint, uint, uint*, uint*, double*);
  SpMat(vector<uint> &, vector<uint> &, vector<double> &);
  ~SpMat();
  
  /* Get functions */
  uint GetNumNonZero(void) const;
  uint GetNumDiagonalElems(void) const;
  void GetTriplet(uint **, uint **, double **) const;
  void GetTriplet(uint, uint &, uint &, double &) const;
  double GetValue(uint);
  
  /* Set functions */
  void SetValue(uint, double);
  void AddValue(uint, double);
};

/* Iterators for iterating over all the elements of a sparse 
   matrix */
# define SPMAT_FOR_ALL(SparseMatrix, Idxi, Idxj, Value) \
  {							\
  uint numNonZero = SparseMatrix.GetNumNonZero();	\
  for (int ___i = 0; ___i < numNonZero; ___i++) {	\
  SparseMatrix.GetTriplet(Idxi, Idxj, Value);		\
  
# define SPMAT_END_FOR }} 


int
cghs(uint N, SpMat &A, double *b, double *x, double eps, bool detailed, uint max_iterations);

void 
mult(const SpMat &T, const double *v, double *w);
# endif
