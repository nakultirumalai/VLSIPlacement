# include <AnalyticalSolve.h>

void
MSKAPI printstrGenSolveWL(void *handle, char str[])
{
  //  printf("%s", str);
}

void
genGetObjectiveMatrixWL(Design &myDesign, HyperGraph &myGraph,
			vector<Cell *> &inputCells, MSKidxt **qsubi, 
			MSKidxt **qsubj, MSKidxt **qsub, MSKrealt **qvalij, 
			MSKrealt **qval, unsigned int &numValuesQuad,
			unsigned int &numValuesLin, double &constant)
{
  vector<uint> subi_vec, subj_vec, sub_vec;
  vector<double> valij_vec, val_vec;
  uint i;

  getObjectiveCliqueModel(myDesign, myGraph, inputCells, subi_vec, subj_vec, 
			  valij_vec, sub_vec, val_vec, constant);


  *qsubi = (MSKidxt *) malloc(sizeof(MSKidxt) * subi_vec.size());
  *qsubj = (MSKidxt *) malloc(sizeof(MSKidxt) * subj_vec.size());
  *qvalij = (MSKrealt *) malloc(sizeof(MSKrealt) * valij_vec.size());
  *qsub = (MSKidxt *) malloc(sizeof(MSKidxt) * sub_vec.size());
  *qval = (MSKrealt *) malloc(sizeof(MSKrealt) * val_vec.size());

  for (i = 0; i < subi_vec.size(); i++) {
    (*qsubi)[i] = subi_vec[i];
    (*qsubj)[i] = subj_vec[i];
    (*qvalij)[i] = 2 * valij_vec[i];
  }
  numValuesQuad = subi_vec.size();
  for (i = 0; i < sub_vec.size(); i++) {
    (*qsub)[i] = sub_vec[i];
    (*qval)[i] = 2 * val_vec[i];
  }
  numValuesLin = sub_vec.size();
}

vector<double>
genSolveQOWL(Design &myDesign, HyperGraph &myGraph, vector<Cell *>& inputCells)
{
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;
  unsigned int numVars, numCstrs;
  
  numVars = 2 * inputCells.size();
  numCstrs = 0;

  /* Define a return vector */
  vector<double> X;

  /* Define an array of variables */
  double x[numVars];

  /* Define variables for quadratic objective */
  MSKidxt *qsubi; MSKidxt *qsubj; MSKrealt *qvalij;
  unsigned int qoNonZero;

  /* Define variables for linear objective */
  MSKidxt *qsub; MSKrealt *qval; double constant;
  unsigned int loNonZero;

  /* Define variables for variable bounds */
  MSKidxt *subb; MSKrealt *bu; MSKrealt *bl; MSKboundkeye *bk;
  unsigned int numVarBounds;

  /* Get the complete quadratic function */
  genGetObjectiveMatrixWL(myDesign, myGraph, inputCells, &qsubi, &qsubj, 
			  &qsub, &qvalij, &qval, qoNonZero, loNonZero, 
			  constant);
  
  /* Get the variable bounds */
  mskGetVarBounds(myDesign, myGraph, inputCells, &subb, &bu, &bl, &bk, 
		  numVarBounds);

  /* Create the mosek environment */
  r = MSK_makeenv(&env, NULL, NULL, NULL, NULL);

  /* Check if the return code is okay */
  if (r == MSK_RES_OK) {
    MSK_linkfunctoenvstream(env, MSK_STREAM_LOG, NULL, printstrGenSolveWL);
  }

  /* Initialize the environment */
  r = MSK_initenv(env);
  if (r == MSK_RES_OK) {

    /* Create the optimization task */
    r = MSK_maketask(env, numCstrs, numVars, &task);

    if (r == MSK_RES_OK) {
      r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstrGenSolveWL);

      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_putmaxnumvar(task, numVars);
      if (r == MSK_RES_OK) r = MSK_putmaxnumcon(task, numCstrs);
      if (r == MSK_RES_OK) r = MSK_putmaxnumanz(task, numVars);

      /* Append 'numCstrs' empty constraints. The constraints will initially have no bounds. */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_CON, numCstrs);

      /* Append 'numVars', the variables will initially be fixed to zero (x = 0) */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_VAR, numVars);

      /* Put bounds on the variables */
      if (r == MSK_RES_OK) r = MSK_putboundlist(task, MSK_ACC_VAR, 
						numVarBounds, subb,
						bk, bl, bu);

      /* Put the quadratic part of the objective function */
      if (r == MSK_RES_OK) r = MSK_putqobj(task, qoNonZero, qsubi, qsubj, qvalij);

      /* Put the linear part of the objective function */
      if (r == MSK_RES_OK) r = MSK_putclist(task, loNonZero, qsubi, qval);

      /* Optionally add a constant term to the objective. */
      if (r == MSK_RES_OK) r = MSK_putcfix(task, constant);

      /* Ready to invoke the optimizer here. Add the optimization sense which
	 is minimization for now. Can be parametrized later */
      if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);

      /* BEGIN THE OPTIMIZATION HERE */
      if (r == MSK_RES_OK) {
	MSKrescodee trmcode;

	_STEP_BEGIN("Begin running the optimizer");
	/* Run the optimizer */
	r = MSK_optimizetrm(task, &trmcode);
	_STEP_END("End running the optimizer");

	/* Print a summary containing information about the solution for debugging purposes */
	MSK_solutionsummary(task, MSK_STREAM_LOG);

	if (r == MSK_RES_OK) {
	  MSKsolstae solsta;

	  MSK_getsolutionstatus(task, MSK_SOL_ITR, NULL, &solsta);

	  switch(solsta) {
	  case MSK_SOL_STA_OPTIMAL:
	  case MSK_SOL_STA_NEAR_OPTIMAL:
	    MSK_getsolutionslice(task, MSK_SOL_ITR, MSK_SOL_ITEM_XX, 0, numVars, x);
	    cout << "Optimal primal solution found\n";
	    /* Copy values in x to return vector X */
	    for (int i = 0; i < numVars; i++) {
	      X.push_back(x[i]);
	    }
	    break;
	  case MSK_SOL_STA_DUAL_INFEAS_CER:
	  case MSK_SOL_STA_PRIM_INFEAS_CER:
	  case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
	  case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER:
	    cout << "Primal or dual infeasibility certificate found" << endl;
	    break;
	  case MSK_SOL_STA_UNKNOWN:
            cout << "The status of the solution could not be determined" << endl;
            break;
          default:
            cout << "Other solution status" << endl;
            break;
          };
        } else {
          cout << "Error while optimizing" << endl;
        }
      }
    }
    MSK_deletetask(&task);
  }
  MSK_deleteenv(&env);

  return X;
}

void 
genSolve(Design& myDesign, HyperGraph& myGraph, vector<Cell *>& inputCells)
{
  vector<double> X;

  X =  genSolveQOWL(myDesign, myGraph, inputCells);

  unsigned int numCells = inputCells.size();
  for (int i = 0; i < numCells; i++) {
    Cell &thisCell = *((Cell*)inputCells[i]);
    thisCell.CellSetXpos((X[i] * GRID_COMPACTION_RATIO)); 
    thisCell.CellSetYpos((X[i+numCells] * GRID_COMPACTION_RATIO));
    //    cout << "Cell: " << thisCell.CellGetName() << "\t" 
    //	 << thisCell.CellGetXpos() << "\t" 
    //	 << thisCell.CellGetYpos() << endl;
  }
}

