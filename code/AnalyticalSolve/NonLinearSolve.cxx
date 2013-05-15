#include <fstream>
//#include <newmat.h>
#include <NLF.h>
#include <OptQNewton.h>

using NEWMAT::ColumnVector;
using namespace::OPTPP;

void init_rosen(int ndim, ColumnVector& x);
void rosen(int ndim, const ColumnVector& x, double& fx, int& result);

int main()
{
  int ndim = 2;

  FDNLF1 nlp(ndim, rosen, init_rosen);
  OptQNewton objfcn(&nlp);

  objfcn.setSearchStrategy(TrustRegion);
  objfcn.setMaxFeval(200);
  objfcn.setFcnTol(1.e-4);

  // The "0" in the second argument says to create a new file.  A "1"
  // would signify appending to an existing file.

  if (!objfcn.setOutputFile("example1.out", 0))
    cerr << "main: output file open failed" << endl;
  objfcn.optimize();
  objfcn.printStatus("Solution from quasi-newton");
  objfcn.cleanup();
}
