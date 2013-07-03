# include <ConjGradSolver.h>
# include <cblas.h>

SpMat::SpMat()
{
  numNonZero = 0;
  numDiagElems = 0;
  idxi = NIL(uint *);
  idxj = NIL(uint *);
  val = NIL(double *);
}

SpMat::~SpMat()
{
}

SpMat::SpMat(uint numNonZero, uint numDiag, uint *idxi, uint* idxj, double *value)
{
  this->numNonZero = numNonZero;
  this->numDiagElems = numDiagElems;
  this->idxi = (uint *)malloc(sizeof(uint) * numNonZero);
  this->idxj = (uint *)malloc(sizeof(uint) * numNonZero);
  this->val = (double *)malloc(sizeof(double) * numNonZero);

  /* Once malloced, memcpy is safe to be used */
  memcpy(idxi, this->idxi, sizeof(uint) * numNonZero);
  memcpy(idxj, this->idxi, sizeof(uint) * numNonZero);
  memcpy(value, this->val, sizeof(double) * numNonZero);
}

SpMat::SpMat(vector<uint> &idxi_vec, vector<uint> &idxj_vec, vector<double> &val_vec) 
{
  this->numNonZero = val_vec.size();
  this->numDiagElems = 0;
  this->idxi = (uint *)malloc(sizeof(uint) * this->numNonZero);
  this->idxj = (uint *)malloc(sizeof(uint) * this->numNonZero);
  this->val = (double *)malloc(sizeof(double) * this->numNonZero);
  
  for (int i = 0; i < this->numNonZero; i++) {
    (this->idxi)[i] = idxi_vec[i];
    (this->idxj)[i] = idxj_vec[i];
    if (idxi_vec[i] == idxj_vec[i]) {
      this->numDiagElems++;
    }
    (this->val)[i] = val_vec[i];
  }
}

uint
SpMat::GetNumNonZero(void) const
{
  return this->numNonZero;
}

uint
SpMat::GetNumDiagonalElems(void) const
{
  return this->numDiagElems;
}

void
SpMat::GetTriplet(uint **idxi, uint **idxj, double **val) const
{
  *idxi = this->idxi;
  *idxj = this->idxj;
  *val = this->val;
}

void
SpMat::GetTriplet(uint num, uint &idxi, uint &idxj, double &value) const
{
  if (num >= numNonZero) {
    cout << "Error: Specified index exceeds number of elements" << endl;
    exit(0);
  }
  idxi = (this->idxi)[num];
  idxj = (this->idxj)[num];
  value = (this->val)[num];
}

double
SpMat::GetValue(uint valIdx) 
{
  return (this->val)[valIdx];
}

void
SpMat::SetValue(uint valIdx, double val) 
{
  (this->val)[valIdx] = val;
}

void
SpMat::AddValue(uint valIdx, double val) 
{
  (this->val)[valIdx] += val;
}

/* Special multiplication function for the sparse matrix 
   type SpMat to perform w = Av */
void 
multold(const SpMat &T, const double *v, double *w) 
{
  uint numNonZero = T.GetNumNonZero();
  uint numDiag = T.GetNumDiagonalElems();
  uint idx;
  uint idxi, idxj;
  double val;
  
  for (idx = 0; idx < numDiag; idx++) {
    w[idx] = 0.0;
  }

  for (idx = 0; idx < numNonZero; idx++) {
    T.GetTriplet(idx, idxi, idxj, val);
    if (idxi == idxj) w[idxi] += val * v[idxi];
    else {
      w[idxi] += val * v[idxj];
      w[idxj] += val * v[idxi];
    }
  }
} 

void 
mult(const SpMat &T, const double *v, double *w) 
{
  uint numNonZero = T.GetNumNonZero();
  uint numDiag = T.GetNumDiagonalElems();
  uint idx;
  uint *idxi, *idxj;
  uint i, j;
  double *val, value;
  
  for (idx = 0; idx < numDiag; idx++) {
    w[idx] = 0.0;
  }

  T.GetTriplet(&idxi, &idxj, &val);
  for (idx = 0; idx < numNonZero; idx++) {
    i = idxi[idx]; j = idxj[idx];
    value = val[idx];
    if (i == j) w[i] += value * v[i];
    else {
      w[i] += value * v[j];
      w[j] += value * v[i];
    }
  }
} 

int
cghs( unsigned N, SpMat &A, double *b, double *x, double eps,
      bool detailed , uint max_iterations) {
  int rtv;
  if ( N==0 )
    return -1;
  double *g = new double[N];
  double *r = new double[N];
  double *p = new double[N];
  int its=0;
  double t, tau, sig, rho, gam;
  double err=eps*eps*ddot(N,b,1,b,1);
  
  /* COMPUTE THE RESIDUE r = b - Ax0 */
  /* REFER TO THE ALGORITHM IN WIKI AND 
     FOLLOW THE BELOW BY ASSUMING 
     r(in wiki) = g(here) 
     p(in wiki) = r(here)
     alpha(in wiki) = t(here)
  */
  rtv = 0;
  mult(A,x,g);
  daxpy(N,-1.,b,1,g,1);
  dscal(N,-1.,g,1);
  dcopy(N,g,1,r,1);

  while ( ddot(N,g,1,g,1)>err ) {
    mult(A,r,p); /* p = Ar */

    rho=ddot(N,p,1,p,1); 

    sig=ddot(N,r,1,p,1); 
    tau=ddot(N,g,1,r,1); 
    t=tau/sig; 

    daxpy(N,t,r,1,x,1); 
    daxpy(N,-t,p,1,g,1);
    
    gam=(t*t*rho-tau)/tau;
    dscal(N,gam,r,1);
    daxpy(N,1.,g,1,r,1);
    if ( detailed )
      cout<<"cghs "<<its<<"\t"<<dnrm2(N,g,1)<<endl;
    ++its;

    if (its > max_iterations) {
      rtv = its;
      break;
    }
  }
  rtv = its;
  delete[] g;
  delete[] r;
  delete[] p;
  return rtv;
}

