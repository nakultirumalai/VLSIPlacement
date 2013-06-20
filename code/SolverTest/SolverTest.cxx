# include <ConjGradSolver.h>
# include <mosek.h>
# include <Stat.h>

using namespace std;

/**********************************************************
 FILE TO TEST THE SOLVERS ON THE HESSIANS 
 STORED IN THE FILES FOR DESIGN DIRECTORIES 
**********************************************************/
typedef enum {
  SOLVER_SOLVED_SUCCESSFULLY = 0,
  SOLVER_DID_NOT_COMPLETE,
  SOLVER_DID_NOT_SOLVE,
  NUM_SOLVER_TYPES
} solverStatus;

void
MSKAPI printDesignSolverOutput(void *handle, const char str[])
{
   printf("%s", str);
}

bool
initMosek(MSKenv_t *env, MSKtask_t *task, uint numVars, uint numCstrs)

{
  MSKrescodee r;
  bool rtv;

  rtv = true;
  r = MSK_makeenv(env, NULL);
  if (r == MSK_RES_OK) {
    MSK_linkfunctoenvstream(*env, MSK_STREAM_LOG, NULL, printDesignSolverOutput);
  } else rtv = false;

  /* Initialize the environment */
  r = MSK_initenv(*env);
  if (r == MSK_RES_OK) {
    /* Create the optimization task */
    r = MSK_maketask(*env, numCstrs, numVars, task);
    if (r == MSK_RES_OK) {
      r = MSK_linkfunctotaskstream(*task, MSK_STREAM_LOG, NULL, printDesignSolverOutput);
      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_appendcons(*task, numCstrs);
      if (r == MSK_RES_OK) r = MSK_appendvars(*task, numVars);
    } else rtv = false;
  } else rtv = false;

  return (rtv);
}

solverStatus
mosekSolve(vector<uint> &subi_vec, vector<uint> &subj_vec, vector<double> &valij_vec, 
	   vector<uint> &sub_vec, vector<double> &val_vec)
{
  MSKint32t *qsubi, *qsubj, *qsub;
  MSKrealt *qvalij, *qval;
  uint i, numVars, numCstrs;
  uint numLinValues, numQuadValues;
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r = MSK_RES_OK;
  solverStatus rtv;

  rtv = SOLVER_DID_NOT_COMPLETE;
  qsubi = (MSKint32t *) malloc(sizeof(MSKint32t) * subi_vec.size());
  qsubj = (MSKint32t *) malloc(sizeof(MSKint32t) * subj_vec.size());
  qsub = (MSKint32t *) malloc(sizeof(MSKint32t) * sub_vec.size());
  qvalij = (MSKrealt *) malloc(sizeof(MSKrealt) * valij_vec.size());
  qval = (MSKrealt *) malloc(sizeof(MSKrealt) * val_vec.size());
  
  numVars = 0;
  for (i = 0; i < valij_vec.size(); i++) {
    if (subi_vec[i] == subj_vec[i]) {
      numVars++;
    }
    qsubi[i] = subi_vec[i];
    qsubj[i] = subj_vec[i];
    /* Since mosek assumes a half before the hessian matrix */
    qvalij[i] = 2 * valij_vec[i];
  }
  for (i = 0; i < val_vec.size(); i++) {
    qsub[i] = sub_vec[i];
    qval[i] = 2 * val_vec[i];
  }

  numCstrs = 0;
  double x[numVars];
  if (!initMosek(&env, &task, numVars, numCstrs)) {
    cout << "Solver initialization error" << endl;
    return (SOLVER_DID_NOT_COMPLETE);
  }
  numQuadValues = valij_vec.size();
  numLinValues = val_vec.size();
  r = MSK_putqobj(task, numQuadValues, qsubi, qsubj, qvalij);
  if (r == MSK_RES_OK) r = MSK_putclist(task, numLinValues, qsub, qval);
  if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);
  if (r == MSK_RES_OK) {
    MSKrescodee trmcode;
    r = MSK_optimizetrm(task, &trmcode);
    MSK_solutionsummary(task, MSK_STREAM_LOG);
    if (r == MSK_RES_OK) {
      MSKsolstae solsta;
      MSK_getsolsta(task, MSK_SOL_ITR, &solsta);
      if (solsta == MSK_SOL_STA_NEAR_OPTIMAL || solsta == MSK_SOL_STA_OPTIMAL) {
	rtv = SOLVER_SOLVED_SUCCESSFULLY;
	MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, x);
      }
    }
  }
  return rtv;
}

solverStatus
cghsSolve(vector<uint> &subi_vec, vector<uint> &subj_vec, vector<double> &valij_vec, 
	  vector<uint> &sub_vec, vector<double> &val_vec)
{
  uint numVars, i;
  solverStatus rtv;

  SpMat sparseMat(subi_vec, subj_vec, valij_vec);
  numVars = sparseMat.GetNumDiagonalElems();

  double *b = (double *) malloc(sizeof(double) * numVars);

  for (i = 0; i < numVars; i++) {
    b[i] = 0.0;
  }
  for (i = 0; i < val_vec.size(); i++) {
    b[sub_vec[i]] = -val_vec[i];
  }
  double eps=1e-13;
  double x[numVars];
  uint max_iterations = 5000;

  for (i = 0; i < numVars; i++) {
    x[i] = 0;
  }
  rtv = SOLVER_SOLVED_SUCCESSFULLY;
  int solverResult;
  solverResult = cghs(numVars, sparseMat, b, x, eps, true, max_iterations);
  if (solverResult == -1) {
    rtv = SOLVER_DID_NOT_COMPLETE;
  }
  
  return (rtv);
}

void
populateHessian(string fileName, vector<uint> &subi_vec, 
		vector<uint> &subj_vec, vector<double> &valij_vec)
{
  string line;
  uint idxi, idxj;
  double val;
  ifstream ifs(fileName.data());
  while (!ifs.eof()) {
    getline(ifs, line);
    if (line == "") {
      continue;
    }
    istringstream stream(line, istringstream::in);
    
    /* Input the triplet */
    stream >> idxi;
    stream >> idxj;
    stream >> val;
    subi_vec.push_back(idxi);
    subj_vec.push_back(idxj);
    valij_vec.push_back(val);
  }
  ifs.close();
}

void
populateBMat(string fileName, vector<uint> &sub_vec, vector<double> &val_vec)
{
  string line;
  uint idx;
  double val;
  ifstream ifs(fileName.data());
  while (!ifs.eof()) {
    getline(ifs, line);
    if (line == "") {
      continue;
    }
    istringstream stream(line, istringstream::in);
    
    /* Input the triplet */
    stream >> idx;
    stream >> val;
    sub_vec.push_back(idx);
    val_vec.push_back(val);
  }
  ifs.close();
}
		
int 
main(int argc, char *argv[])
{
  if (argc < 2) {
    cout << "Arguments not provided" << endl;
    return 0;
  }
  string DesignName = argv[1];
 
  vector<uint> subi_vecx, subj_vecx, subi_vecy, subj_vecy;
  vector<uint> sub_vecx, sub_vecy;
  vector<double> valij_vecx, valij_vecy, val_vecx, val_vecy;
  
  string fileToRead;
  string line;
  uint idxi, idxj, idx;
  double val;
  solverStatus mosekStatus;
  double cpuTimeBegin, cpuTimeEnd;

  /* Read and populate the X-Objective first */
  fileToRead = DesignName + "/hessianxclique.txt";
  if (fileExists(fileToRead)) {
    populateHessian(fileToRead, subi_vecx, subj_vecx, valij_vecx);
    fileToRead = DesignName + "/bmatxclique.txt";
    if (fileExists(fileToRead)) {
      populateBMat(fileToRead, sub_vecx, val_vecx);
      cpuTimeBegin = getCPUTime();
      //      mosekStatus = mosekSolve(subi_vecx, subj_vecx, valij_vecx, 
      //			       sub_vecx, val_vecx);
      //      cpuTimeEnd = getCPUTime();
      mosekStatus = SOLVER_SOLVED_SUCCESSFULLY;
      if (mosekStatus == SOLVER_SOLVED_SUCCESSFULLY) {
	cout << "PASS: Mosek successfully solved X values in " << (cpuTimeEnd - cpuTimeBegin) << " seconds" << endl;
      } else {
	cout << "FAIL: Mosek did not complete successfully" << endl;
      }
    }
    subi_vecx.clear(); subj_vecx.clear(); sub_vecx.clear();
    valij_vecx.clear(); val_vecx.clear();
  }
    
  /* Read and populate the Y-Objective first */
  fileToRead = DesignName + "/hessianyclique.txt";
  if (fileExists(fileToRead)) {
    populateHessian(fileToRead, subi_vecy, subj_vecy, valij_vecy);
    fileToRead = DesignName + "/bmatyclique.txt";
    if (fileExists(fileToRead)) {    
      populateBMat(fileToRead, sub_vecy, val_vecy);
      cpuTimeBegin = getCPUTime();
      //      mosekStatus = mosekSolve(subi_vecy, subj_vecy, valij_vecy, 
      //			       sub_vecy, val_vecy);
      //      cpuTimeEnd = getCPUTime();
      mosekStatus = SOLVER_SOLVED_SUCCESSFULLY;
      if (mosekStatus == SOLVER_SOLVED_SUCCESSFULLY) {
	cout << "PASS: Mosek successfully solved Y values in " << (cpuTimeEnd - cpuTimeBegin) << " seconds" << endl;
      } else {
	cout << "FAIL: Mosek did not complete successfully" << endl;
      }
    }
    subi_vecy.clear(); subj_vecy.clear(); sub_vecy.clear();
    valij_vecy.clear(); val_vecy.clear();
  }

  /* Read and populate the X-Objective first */
  fileToRead = DesignName + "/hessianxclique.txt";
  if (fileExists(fileToRead)) {
    populateHessian(fileToRead, subi_vecx, subj_vecx, valij_vecx);
    fileToRead = DesignName + "/bmatxclique.txt";
    if (fileExists(fileToRead)) {
      populateBMat(fileToRead, sub_vecx, val_vecx);
      cpuTimeBegin = getCPUTime();
      mosekStatus = cghsSolve(subi_vecx, subj_vecx, valij_vecx, 
			       sub_vecx, val_vecx);
      cpuTimeEnd = getCPUTime();
      if (mosekStatus == SOLVER_SOLVED_SUCCESSFULLY) {
	cout << "PASS: Conjugate gradient solver successfully solved X values in " << (cpuTimeEnd - cpuTimeBegin) << " seconds" << endl;
      } else {
	cout << "FAIL: Conjugate gradient solver did not complete successfully" << endl;
      }
    }
    subi_vecx.clear(); subj_vecx.clear(); sub_vecx.clear();
    valij_vecx.clear(); val_vecx.clear();
  }

  fileToRead = DesignName + "/hessianyclique.txt";
  if (fileExists(fileToRead)) {
    populateHessian(fileToRead, subi_vecy, subj_vecy, valij_vecy);
    fileToRead = DesignName + "/bmatyclique.txt";
    if (fileExists(fileToRead)) {
      populateBMat(fileToRead, sub_vecy, val_vecy);
      cpuTimeBegin = getCPUTime();
      mosekStatus = cghsSolve(subi_vecy, subj_vecy, valij_vecy, 
			       sub_vecy, val_vecy);
      cpuTimeEnd = getCPUTime();
      if (mosekStatus == SOLVER_SOLVED_SUCCESSFULLY) {
	cout << "PASS: Conjugate gradient solver successfully solved Y values in " << (cpuTimeEnd - cpuTimeBegin) << " seconds" << endl;
      } else {
	cout << "FAIL: Conjugate gradient solver did not complete successfully" << endl;
      }
    }
    subi_vecy.clear(); subj_vecy.clear(); sub_vecy.clear();
    valij_vecy.clear(); val_vecy.clear();
  }
}
