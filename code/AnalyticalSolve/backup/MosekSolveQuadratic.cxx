# include <HyperGraph.h>
# include <HyperGraphIter.h>
# include <AnalyticalSolve.h>

void
MSKAPI printstr(void *handle, char str[]) 
{
  printf("%s", str);
}

/* 
   Solve the quadratically constrained quadratic minimization problem:
   f = (1/2)(x^T)*Q0*(x) + c^T * x
   s.t. lkx <= (1/2)(x^T)*Q1*(x) + Ax <= b
 */
vector<double>
mosekSolveQCQO(Design& myDesign, HyperGraph& myGraph, 
	       vector<Cell*> listOfCells, unsigned int numVars, 
	       unsigned int numCstrs, quadObjFunc qof, linObjFunc lof, 
	       quadCstrFunc qcstrf, linCstrFunc lcstrf, varBoundFunc vbf,
	       cstrBoundFunc cbf)
{
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;

  /* Define a return vector */
  vector<double> X;

  /* Define an array of variables */
  double x[numVars];

  /* Define variables for quadratic objective */
  MSKidxt *qsubi; MSKidxt *qsubj; MSKrealt *qval; 
  unsigned int qoNonZero;
  
  /* Define variables for linear objective */
  MSKidxt *subi; MSKrealt *val; double constant;
  unsigned int loNonZero;
  
  /* Define variables for quadratic constraints */
  MSKidxt *qcsubk; MSKidxt *qcsubi; MSKidxt *qcsubj; MSKrealt *qcval; 
  unsigned int qcNonZero;

  /* Define variables for linear constraints */
  MSKidxt *sub; MSKintt *ptrb; MSKintt *ptre; MSKidxt *asub; MSKrealt *aval; 
  double *cstrUBounds; unsigned int lcNonZero;

  /* Define variables for variable bounds */
  MSKidxt *subb; MSKrealt *bu; MSKrealt *bl; MSKboundkeye *bk;
  unsigned int numVarBounds;

  /* Define variables for constraint bounds */
  MSKidxt *subc; MSKrealt *buc; MSKrealt *blc; MSKboundkeye *bkc;
  unsigned int numCstrBounds;

  
  bool debugPrint = false;

  _STEP_BEGIN("Begin creating objective matrix entries");
  /* Get the quadratic objective matrix */
  if (qof != NULL) {
    qof(myDesign, myGraph, listOfCells, &qsubi, &qsubj, &qval, qoNonZero);
  }

  /* Get the linear objective matrix */
  if (lof != NULL) {
    lof(myDesign, myGraph, listOfCells, &subi, &val, constant, loNonZero);  
  }
  _STEP_END("Begin creating objective matrix entries");

  _STEP_BEGIN("Begin creating constraint matrices");
  /* Get the quadratic constraints matrices */
  if (qcstrf != NULL) {
    qcstrf(myDesign, myGraph, listOfCells, &qcsubk, &qcsubi, &qcsubj, &qcval, 
	   qcNonZero);
  }

  /* Get the linear constraints matrices */
  if (lcstrf != NULL) {
    lcstrf(myDesign, myGraph, listOfCells, &sub, &ptrb, &ptre, &asub, &aval, 
	   &cstrUBounds, lcNonZero);
  }

  /* Get the variable bounds */
  if (vbf != NULL) {
    vbf(myDesign, myGraph, listOfCells, &subb, &bu, &bl, &bk, numVarBounds);
  }

  /* Get the constraint bounds */
  if (cbf != NULL) {
    numCstrBounds = lcNonZero;
    cbf(myDesign, myGraph, listOfCells, &sub, cstrUBounds, &subc, &buc, &blc, &bkc, 
	numCstrBounds);
  }

  _STEP_BEGIN("End creating constraint matrices");

  /* Create the mosek environment */
  r = MSK_makeenv(&env, NULL, NULL, NULL, NULL);

  /* Check if the return code is okay */
  if (r == MSK_RES_OK) {
    MSK_linkfunctoenvstream(env, MSK_STREAM_LOG, NULL, printstr);
  }

  /* Initialize the environment */
   r = MSK_initenv(env);
   if (r == MSK_RES_OK) {

    /* Create the optimization task */
    r = MSK_maketask(env, numCstrs, numVars, &task);

    if (r == MSK_RES_OK) {
      r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);

      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_putmaxnumvar(task, numVars);
      if (r == MSK_RES_OK) r = MSK_putmaxnumcon(task, numCstrs);
      if (r == MSK_RES_OK) r = MSK_putmaxnumanz(task, numVars);
      
      /* Append 'numCstrs' empty constraints. The constraints will initially have no bounds. */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_CON, numCstrs);
      
      /* Append 'numVars', the variables will initially be fixed to zero (x = 0) */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_VAR, numVars);

      /* Put bounds on the variables */
      if (vbf != NULL) {
	if (r == MSK_RES_OK) r = MSK_putboundlist(task, MSK_ACC_VAR, numVarBounds, subb,
						  bk, bl, bu);
      } else {
	for (int i = 0; i < numVars && r == MSK_RES_OK; i++) {
	  r = MSK_putbound(task, MSK_ACC_VAR, i, MSK_BK_FR, 0.0, 
			   MSK_DPAR_DATA_TOL_BOUND_INF);
	}
      }

      /* Get the constraint bounds */
      //      if (r == MSK_RES_OK) r = MSK_putboundlist(task, MSK_ACC_CON, numCstrBounds, subc,
      //						bkc, blc, buc);
      
      if (cbf != NULL) {
	if (r == MSK_RES_OK) r = MSK_putboundlist(task, MSK_ACC_CON, numCstrBounds, subc,
						  bkc, blc, buc);
      } else if (numCstrs > 0) {
	for (int i = 0; i < numCstrs && r == MSK_RES_OK; i++) {
	  r = MSK_putbound(task, MSK_ACC_CON, i, MSK_BK_FR, 0.0, 
			   MSK_DPAR_DATA_TOL_BOUND_INF);
	}
      } 

      /* Put the quadratic part of the objective function */   
      if (qof != NULL) {
	if (r == MSK_RES_OK) r = MSK_putqobj(task, qoNonZero, qsubi, qsubj, qval);
      }
      
      /* Put the linear part of the objective function */
      if (lof != NULL) {
	if (r == MSK_RES_OK) r = MSK_putclist(task, loNonZero, subi, val);
      }

      /* Optionally add a constant term to the objective. */
      if (constant > 0.0) {
	if (r == MSK_RES_OK) r = MSK_putcfix(task, constant);
      }
      
      /* Put the quadratic part of the constraint function */
      //      if (r == MSK_RES_OK) r = MSK_putqcon(task, qcNonZero, qcsubk, qcsubi, qcsubj, qcval);
      
      /* Put the linear part of the constraint function */
      //      if (r == MSK_RES_OK) r = MSK_putaveclist(task, MSK_ACC_CON, lcNonZero, sub, ptrb, 
      //      					       ptre, asub, aval);
      if (qcstrf != NULL) {
	if (r == MSK_RES_OK) r = MSK_putqcon(task, qcNonZero, qcsubk, qcsubi, qcsubj, 
					     qcval);
      }
      
      /* Put the linear part of the constraint function */
      if (lcstrf != NULL) {
	if (r == MSK_RES_OK) r = MSK_putaveclist(task, MSK_ACC_CON, lcNonZero, sub, 
						 ptrb, ptre, asub, aval);
      }
      
      /* Ready to invoke the optimizer here. Add the optimization sense which 
         is minimization for now. Can be parametrized later */
      if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);

      /* BEGIN THE OPTIMIZATION HERE */
      if (r == MSK_RES_OK) {
	MSKrescodee trmcode;
	
	/* Run the optimizer */
	r = MSK_optimizetrm(task, &trmcode);
	
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

/* 
   Solve the linearly constrained quadratic minimization problem:
   f = (1/2)(x^T)*Q0*(x) + c^T * x
   s.t. lc <= Ax + b <= uc 
   Conditions: lc < uc
 */
vector<double>
mosekSolveLCQO(Design& myDesign, HyperGraph& myGraph, 
	       vector<Cell*> listOfCells, unsigned int numVars, 
	       unsigned int numCstrs, quadObjFunc qof, linObjFunc lof, 
	       linCstrFunc lcstrf, varBoundFunc vbf, cstrBoundFunc cbf)
{
  MSKenv_t env;
  MSKtask_t task;
  MSKrescodee r;

  /* Define a return vector */
  vector<double> X;

  /* Define an array of variables */
  double x[numVars];

  /* Define variables for quadratic objective */
  MSKidxt *qsubi; MSKidxt *qsubj; MSKrealt *qval; 
  unsigned int qoNonZero;
  
  /* Define variables for linear objective */
  MSKidxt *subi; MSKrealt *val; double constant;
  unsigned int loNonZero;
  
  /* Define variables for linear constraints */
  MSKidxt *sub; MSKintt *ptrb; MSKintt *ptre; MSKidxt *asub; MSKrealt *aval; 
  double *cstrUBounds; unsigned int lcNonZero;

  /* Define variables for constraint bounds */
  MSKidxt *subc; MSKrealt *buc; MSKrealt *blc; MSKboundkeye *bkc;
  unsigned int numCstrBounds;
  
  bool debugPrint = false;

  _STEP_BEGIN("Begin creating objective matrix entries");
  /* Get the quadratic objective matrix */
  qof(myDesign, myGraph, listOfCells, &qsubi, &qsubj, &qval, qoNonZero);

  /* Get the linear objective matrix */
  lof(myDesign, myGraph, listOfCells, &subi, &val, constant, loNonZero);  
  _STEP_END("Begin creating objective matrix entries");

  _STEP_BEGIN("Begin creating constraint matrices");
  /* Get the linear constraints matrices */
  lcstrf(myDesign, myGraph, listOfCells, &sub, &ptrb, &ptre, &asub, &aval, 
	 &cstrUBounds, lcNonZero);

  /* Get the constraint bounds */
  cbf(myDesign, myGraph, listOfCells, &sub, cstrUBounds, &subc, &buc, &blc, &bkc, 
      lcNonZero);

  _STEP_BEGIN("End creating constraint matrices");

  //  return X;
  /* Create the mosek environment */
  r = MSK_makeenv(&env, NULL, NULL, NULL, NULL);

  /* Check if the return code is okay */
  if (r == MSK_RES_OK) {
    MSK_linkfunctoenvstream(env, MSK_STREAM_LOG, NULL, printstr);
  }

  /* Initialize the environment */
   r = MSK_initenv(env);
   numCstrs = lcNonZero;
   //lcNonZero = 0;
   if (r == MSK_RES_OK) {
    /* Create the optimization task */
    r = MSK_maketask(env, numCstrs, numVars, &task);

    if (r == MSK_RES_OK) {
      r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);

      /* Provide MOSEK with an estimate of the size of the input data. */
      if (r == MSK_RES_OK) r = MSK_putmaxnumvar(task, numVars);
      if (r == MSK_RES_OK) r = MSK_putmaxnumcon(task, numCstrs);
      if (r == MSK_RES_OK) r = MSK_putmaxnumanz(task, numVars);
      
      /* Append 'numCstrs' empty constraints. The constraints will initially have no bounds. */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_CON, numCstrs);
      
      /* Append 'numVars', the variables will initially be fixed to zero (x = 0) */
      if (r == MSK_RES_OK) r = MSK_append(task, MSK_ACC_VAR, numVars);

      /* Get the variable bounds */
      MSKidxt *subb; MSKrealt *bu; MSKrealt *bl; MSKboundkeye *bk;
      unsigned int numVarBounds;
      vbf(myDesign, myGraph, listOfCells, &subb, &bu, &bl, &bk, numVarBounds);

      /* Put bounds on the variables */
      if (r == MSK_RES_OK) r = MSK_putboundlist(task, MSK_ACC_VAR, numVarBounds, subb,
						bk, bl, bu);
      
      /* Put bounds on the constraints */
      if (r == MSK_RES_OK) r = MSK_putboundlist(task, MSK_ACC_CON, numCstrBounds, subc,
						bkc, blc, buc);
      
      if (r == MSK_RES_OK) r = MSK_putqobj(task, qoNonZero, qsubi, qsubj, qval);
      
      /* Put the linear part of the objective function */
      if (r == MSK_RES_OK) r = MSK_putclist(task, loNonZero, subi, val);

      /* Optionally add a constant term to the objective. */
      if (r == MSK_RES_OK) r = MSK_putcfix(task, constant);
      
      /* Put the linear part of the constraint function */
      if (r == MSK_RES_OK) r = MSK_putaveclist(task, MSK_ACC_CON, lcNonZero, sub, ptrb, 
      					       ptre, asub, aval);
      
      /* Ready to invoke the optimizer here. Add the optimization sense which 
         is minimization for now. Can be parametrized later */
      if (r == MSK_RES_OK) r = MSK_putobjsense(task , MSK_OBJECTIVE_SENSE_MINIMIZE);

      /* BEGIN THE OPTIMIZATION HERE */
      if (r == MSK_RES_OK) {
	MSKrescodee trmcode;
	
	/* Run the optimizer */
	r = MSK_optimizetrm(task, &trmcode);
	
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

