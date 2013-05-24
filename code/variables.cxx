/* Header file for global variables */
/*********************************************************************
INTEGER GLOBAL VARIABLES
********************************************************************/
unsigned int stepDepth;

/*********************************************************************
BOOLEAN GLOBAL VARIABLES
********************************************************************/
bool debug=false;
bool performNetHidingConsistency=false;
bool performAnalysis=false;
unsigned int traceDepth=0;
bool varOption = false;

/* Variable for controlling the threshold above which the 
   net model should be upgraded to a star rather than 
   a clique */
unsigned int starModelUpgradeNumPins = 3;
