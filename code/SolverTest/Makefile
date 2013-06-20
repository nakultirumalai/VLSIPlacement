SolverTest: SolverTest.cxx $(CODE_ROOT)/common/common_utils.cxx $(CODE_ROOT)/Stat/StatMain.cxx $(CODE_ROOT)/ConjGrad/ConjGradSolver.cxx
	g++ -pg SolverTest.cxx $(CODE_ROOT)/common/common_utils.cxx \
	$(CODE_ROOT)/Stat/StatMain.cxx $(CODE_ROOT)/ConjGrad/ConjGradSolver.cxx \
	-I$(CODE_ROOT)/include -lmosek64 -pthread -lblas -o SolverTest
	g++ -g SolverTest.cxx $(CODE_ROOT)/common/common_utils.cxx \
	$(CODE_ROOT)/Stat/StatMain.cxx $(CODE_ROOT)/ConjGrad/ConjGradSolver.cxx \
	 -I$(CODE_ROOT)/include -lmosek64 -pthread -lblas -o SolverTest-g

