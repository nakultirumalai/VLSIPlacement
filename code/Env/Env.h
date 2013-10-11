# include <common.h>

# ifndef ENV_H
# define ENV_H

/***************************************************
   This class is meant to store specifics
   about the environment of the tool and the 
   configurations that the tool is currently being 
   run in. An instantiation of the class can 
   be made in the class Design.
***************************************************/

/***************************************************
  SOME TYPE DEFINITIONS FOR THE ENVIRONMENT
***************************************************/
/* Define the global placer type */
typedef enum {
  ENV_NO_EXTERNAL_FP,
  ENV_NTUPLACE_FP,
  ENV_MPL6_FP,
  ENV_MPL6_NOCLUST_FP,
  ENV_FAST_PLACE_FP,
  NUM_ENV_FLAT_PLACERS
} EnvFlatPlacerType;
# define DEFAULT_ENV_FLAT_PLACER_TYPE ENV_NO_EXTERNAL_FP

typedef enum {
  ENV_NO_PLACEMENT,
  ENV_NO_EXTERNAL_GP,
  ENV_NTUPLACE_GP,
  ENV_MPL6_GP,
  ENV_MPL6_NOCLUST_GP,
  ENV_FAST_PLACE_GP,
  NUM_ENV_GLOBAL_PLACERS
} EnvGlobalPlacerType;
# define DEFAULT_ENV_GLOBAL_PLACER_TYPE ENV_NO_EXTERNAL_GP

typedef enum {
  ENV_DEFAULT_CP,
  ENV_NTUPLACE_CP,
  ENV_MPL6_CP,
  ENV_MPL6_NOCLUST_CP,
  ENV_FAST_PLACE_CP,
  NUM_ENV_CLUSTER_PLACERS
} EnvClusterPlacerType;
# define DEFAULT_ENV_CLUSTER_PLACER_TYPE ENV_DEFAULT_CP

/* Define the solver type to be used in the design */
typedef enum {
  ENV_SOLVER_QUADRATIC_MOSEK=0,
  ENV_SOLVER_QUADRATICXY_MOSEK,
  ENV_SOLVER_QUADRATIC_CONJ_GRAD,
  ENV_SOLVER_QUADRATICXY_CONJ_GRAD,
  ENV_SOLVER_NON_LINEAR,
  ENV_SOLVER_NON_LINEAR_CONJ_GRAD,
  ENV_SOLVER_FORCE_DIRECTED,
  NUM_ENV_SOLVERS
} EnvSolverType;
//# define DEFAULT_ENV_SOLVER_TYPE ENV_SOLVER_QUADRATIC_CONJ_GRAD
# define DEFAULT_ENV_SOLVER_TYPE ENV_SOLVER_FORCE_DIRECTED

/* Define the net model used for hypernets of the placement tool  */
typedef enum {
  ENV_CLIQUE_MODEL = 0,
  ENV_STAR_MODEL,
  ENV_HYBRID_MODEL,
  ENV_VARIABLE_HYBRID_MODEL,
  NUM_NET_MODELS
} EnvNetModel;
# define DEFAULT_ENV_NET_MODEL ENV_CLIQUE_MODEL

/* Define the operating mode of the placement tool  */
typedef enum {
  ENV_MODE_OPTIMAL = 0,
  ENV_MODE_TEST,
  NUM_MODES
} EnvToolMode;
# define DEFAULT_ENV_TOOL_MODE ENV_MODE_OPTIMAL

/* Define the legalizer used in placement tool  */
typedef enum {
  ENV_SIMPLE_LEGALIZER = 0,
  ENV_BIN_BASED_LEGALIZER,
  ENV_FAST_PLACE_LEGALIZER,
  ENV_NTUPLACE_LEGALIZER,
  ENV_MPL6_LEGALIZER,
  NUM_LEGALIZERS
} EnvLegalizer;
# define DEFAULT_ENV_LEGALIZER ENV_NTUPLACE_LEGALIZER

/* Define the detailed placer used in the placement tool  */
typedef enum {
  ENV_NO_DETAIL_PLACEMENT = 0,
  ENV_FAST_PLACE_DP,
  ENV_OURPLACER_DP,
  NUM_DETAILED_PLACEMENT
} EnvDetailedPlacer;
# define DEFAULT_ENV_DETAIL_PLACEMENT ENV_NO_DETAIL_PLACEMENT

/* Define the type of optimization desired in the placement tool  */
typedef enum {
  ENV_OPTIMIZE_TIMING = 0,
  ENV_OPTIMIZE_WIRELENGTH
} EnvOptType;
# define DEFAULT_ENV_OPT_TYPE ENV_OPTIMIZE_TIMING

/* Define the type of clustering desired in the placement tool  */
typedef enum {
  ENV_NO_CLUSTERING = 0,
  ENV_LARGE_CLUSTERING,
  ENV_FIRST_CHOICE_CLUSTERING,
  ENV_BEST_CHOICE_CLUSTERING,
  ENV_NET_CLUSTERING,
  ENV_EDGE_SEPARABILITY_BASED_CLUSTERING,
  ENV_TIMING_DRIVEN_CLUSTERING1,
  ENV_TIMING_DRIVEN_CLUSTERING2,
  ENV_GLOBAL_PLACEMENT_TIMING_DRIVEN_CLUSTERING,
  ENV_NUM_CLUSTERING_TYPES,
} EnvClusterType;
# define DEFAULT_ENV_CLUSTER_TYPE ENV_LARGE_CLUSTERING

/* Define the way in which boundary cells are placed inside 
   the cluster */
typedef enum {
  ENV_CLUSTER_PLACE_BOUNDARY = 0,
  ENV_CLUSTER_PLACE_CENTRE,
  ENV_NUM_CLUSTER_PLACE_TYPES
} EnvClusterPlacementType;
# define DEFAULT_ENV_CLUSTER_PLACEMENT_TYPE ENV_CLUSTER_PLACE_BOUNDARY

typedef enum {
  ENV_PLACE_CLUSTERS_PRE_TOP,
  ENV_PLACE_CLUSTERS_POST_TOP_WITH_PORTS,
  ENV_PLACE_CLUSTERS_POST_TOP_FIX_CELLS,
  ENV_PLACE_CLUSTERS_POST_TOP_BOUND,
  ENV_NUM_FLOWS,
} EnvFlowType;
//# define DEFAULT_ENV_FLOW_TYPE ENV_PLACE_CLUSTERS_PRE_TOP
# define DEFAULT_ENV_FLOW_TYPE ENV_PLACE_CLUSTERS_POST_TOP_FIX_CELLS
/* Define the way in which the unclustering can be done within the 
   cluster */
typedef enum {
  ENV_UNCLUSTER_PLACE_BOUNDARY = 0,
  ENV_UNCLUSTER_PLACE_INTERNAL,
  ENV_UNCLUSTER_PLACE_NONE,
  ENV_NUM_UNCLUSTER_TYPES
} EnvUnclusterType;
# define DEFAULT_ENV_UNCLUSTER_TYPE ENV_UNCLUSTER_PLACE_INTERNAL

/* Define the type of shape selection desired in the placement tool  */
typedef enum {
  ENV_NO_SHAPE_SELECTION = 0,
  ENV_STD_SHAPE_SELECTION_ALGORITHM,
  ENV_NUM_SHAPE_SELECTION_TYPES,
} EnvShapeSelectionType;
# define DEFAULT_ENV_SHAPE_SELECTION_TYPE ENV_NO_SHAPE_SELECTION

/* Define the default value of the maximum utilization 
   after which ILR would start */
# define DEFAULT_MAX_UTILIZATION_PHASEI 1.0
# define DEFAULT_CLUSTERING_RATIO 0.3
# define DEFAULT_CLUSTER_MAX_AREA 0.25
# define DEFAULT_CLUSTER_MAX_WIDTH 0.25
# define DEFAULT_CLUSTER_BOUND_PENALTY 1
# define DEFAULT_CLUSTER_HVARIATION_PERCENTAGE 50
# define DEFAULT_CLUSTER_ADDITIONAL_SPACE_PERCENT 15
# define DEFAULT_NUM_HVARIATION_STEPS 0
# define DEFAULT_CLUSTER_NUM_ROWS 0
# define DEFAULT_NUM_CLUSTERS 100
# define DEFAULT_NUM_SHAPE_VARIATIONS 0
# define DEFAULT_IMBALANCE_FACTOR 5
# define DEFAULT_NUM_KHMETIS_RUNS 3
# define DEFAULT_ENV_ITER_COUNT 500
# define DEFAULT_ENV_ABORT_LIMIT 50
# define DEFAULT_USE_WEIGHTED_HPWL false
# define DEFAULT_USE_FD_PLACER true
# define DEFAULT_DO_CLUSTER_SWAPPING true
# define DEFAULT_DO_CLUSTER_MIRRORING true
# define DEFAULT_ENV_OUTPUT_FILE_NAME ./DEFAULT/defaultplacement

/***************************************************
 EVERY VARIABLE DEFINED BELOW HAS TO BE DOCUMENTED!
 SET AND GET FUNCTIONS ARE ADDED TO THE ENV CLASS
 TO QUERY AND SET THE VARIABLES DURING DIFFERENT
 PHASES OF OPTIMIZATION
***************************************************/
class Env {
 private:
  /***************************************/
  /* A REFERENCE OF A TIME STAMP         */
  /***************************************/
  double NetlistReadStartTime;
  double HyperGraphBuildStartTime;
  double ClusteringStartTime;
  double GlobalPlacementStartTime;
  double LegalizationStartTime;
  double ShapeSelectionStartTime;
  double DetailedPlacementStartTime;

  /***************************************/
  /* VARIABLES FOR RECORDING TIME        */
  /***************************************/
  double NetlistReadTime;
  double HyperGraphBuildTime;
  double KWayPartitioningTime;
  double ClusteringTime;
  double FDNetlistBuildTime;
  double FDSolverTime;
  double ClusterSwappingTime;
  double ClusterMirroringTime;
  double ClusterFillingTime;
  double UnclusteringTime;
  double GlobalPlacementTime;
  double LegalizationTime;
  double ShapeSelectionTime;
  double DetailedPlacementTime;
  double TotalGlobalPlacementTime;

  /***************************************/
  /* VARIABLES FOR RECORDING HPWL        */
  /***************************************/
  ulong HPWLAfterConstructive;
  ulong HPWLAfterFDPlacement;
  ulong HPWLAfterClusterSwapping;
  ulong HPWLAfterClusterFill;
  ulong HPWLAfterClusterMirroring;
  ulong HPWLTotalInternal;
  ulong HPWLTotalGlobal;
  ulong HPWLAfterUnclustering;
  ulong HPWLAfterLegalization;
  ulong HPWLAfterDetailedPlacement;
  ulong HPWLFinal;

  /***************************************/
  /* VARIABLES FOR SOME NET STATS        */
  /***************************************/
  uint numTwoPinNets;
  uint numThreePinNets;
  uint numFourPinNets;
  uint numFivePinNets;
  uint numSixPinNets;
  uint numSevenPinNets;
  uint numGreaterThanTenPinNets;
  uint numGreaterThanTwentyPinNets;
  uint numHighFanoutNets;

  /***************************************/
  /* STRING VARIABLES                    */
  /***************************************/
  /* Variable to define the design name */
  string DesignName;
  /* Variable to define the design path */
  string DesignPath;
  /* Decide what the name of the output file should be */
  string OutputFileName;
  
  /***************************************/
  /* INTEGER VARIABLES                   */
  /***************************************/
  /* Variable to define the different types of cells */
  /* Total number of objects read from the nodes file */
  uint NumCellsFromFile;
  /* Total number of ports that have been read */
  uint NumPorts;
  /* Total number of terminals that have been read */
  uint NumTerminals;
  /* Total number of fixed cells that have been read */
  uint NumFixedCells;
  /* Total number of movable cells that have to be solved 
     for */
  uint NumMovableCells;
  /* Total number of standard cells */
  uint NumStdCells;
  /* Total number of movable macros */
  uint NumMacroCells;
  /* Total number of fixed macros */
  uint NumFixedMacros;
  /* Total number of nets that are read */
  uint NumNets;
  /* Total number of rows that need to be allotted to 
     a cluster */
  uint ClusterNumRows;
  /* Variable to indicate the number of partitions the k-way
     partitioner should distribute the cells into */
  uint NumClusters;
  /* Variable to indicate the number of variations of shape
     for a square cluster */
  uint NumShapeVariations;     
  /* Variable to indicate the Imbalance factor 
     ranges from 5-49 */
  uint ImbalanceFactor;
  /* Variable to indicate the number of runs of khmetis that 
     needs to run */
  uint NumKHmetisRuns;
  /* Variable to indicate the number of steps in which the 
     height variation must be performed */
  uint NumHVariationSteps;
  /* Iteration count and abort limit for the force directed
     solver */
  uint IterCount;
  uint AbortLimit;

  /***************************************/
  /* BOOLEAN VARIABLES                   */
  /***************************************/
  /* Flag to determine the use of variable bounds in the solver */
  bool UseVarBounds;
  /* Flag to determine if the tool has to dump the hessian and 
     the linear matrix for testing with solvers */
  bool DumpHessian;
  /* Flag to determine if the cluster formation algorithm 
     should consider the cluster height to be an integral 
     multiple of row height */
  bool DiscreteHeight;
  /* Flag to determine if the cluster formation algorithm 
     should consider the cluster width to be an integral 
     multiple of site width */
  bool DiscreteWidth;
  /* Flag to determine if the k-way paritioner should use 
     recursive bi-partitioning or direct k-way partitioning */
  bool RecursiveBiPartitioning;
  /* Flag to indicate if placement has to be inside the 
     cluster */
  bool PlaceCellsInCluster;
  /* Flat to indicate if a weighted HPWL computation cost 
     function must be used */
  bool UseWeightedHPWL;
  /* Flag to indicate if force directed placement has to be 
     executed */
  bool UseFDPlacer;
  /* Flag to indicate if placement inside the clusters have
     to be postponed until top level is done */
  bool PlaceCellsInClusterPostTop;
  /* Flag to indicate if swapping of clusters has to be performed */
  bool ClusterSwapping;
  /* Flag to indicate if mirroring of clusters has to be performed */
  bool ClusterMirroring;

  /***************************************/
  /* FLOAT/DOUBLE VARIABLES              */
  /***************************************/
  /* Decide the peak utilization during which the 
     global placer should switch to ILR */
  double MaxUtilPhaseI;
  /* Clustering ratio */
  double ClusteringRatio;
  /* Cluster max area */
  double ClusterMaxArea;
  /* Double cluster max width */
  double ClusterMaxWidth;
  /* Double cluster bound penalty */
  double ClusterBoundPenalty;
  /* The maximum percentage in height that the height 
     must be varied */
  double ClusterHVariationPercentage;
  /* The percentage of additional space that must 
     be alloted when allocating space for placing
     cells inside the cluster */
  double ClusterAddAreaPercentage;
  
  /***************************************/
  /* CUSTOM ENUM VARIABLES               */
  /* ALL VALUES ARE DEFAULTED TO DEFAULT */
  /* VALUES DEFINED ABOVE                */
  /***************************************/
  /* Decide what type of global placer to use */
  EnvGlobalPlacerType GlobalPlacerType;
  /* Decide what type of cluster placer to use */
  EnvClusterPlacerType ClusterPlacerType;
  /* Decide what type of flat placer to use */
  EnvFlatPlacerType FlatPlacerType;
  /* Decide what the type of the solver to use */
  EnvSolverType SolverType;
  /* Decide what mode the tool has to be used in */
  EnvNetModel NetModel;
  /* Decide what mode the tool has to be used in */
  EnvToolMode ToolMode;
  /* Decide what type of legalizer has to be used */
  EnvLegalizer Legalizer;
  /* Decide what type of detailed placement has to be
     done */
  EnvDetailedPlacer DetailedPlacer;
  /* Decide what type of optimization cost the tool 
     has to consider */
  EnvOptType OptType;
  /* Decide what the clustering algorithm type should be */
  EnvClusterType ClusterType;
  /* Decide what type of global placer to use to place cells 
     inside clusters */
  EnvGlobalPlacerType ClusterGlobalPlacer;
  /* Decide what type of Legalizer/detailed placer to use to place cells 
     inside clusters */
  EnvLegalizer ClusterLegalizer;
  /* Decide what the cluster placement type should be */
  EnvClusterPlacementType ClusterPlacementType;
  /* Decide what the unclustering type should be */
  EnvUnclusterType UnclusterType;
  /* Decide what the shape selection algorithm type should be */
  EnvShapeSelectionType ShapeSelectionType;
  /* Decide what the flow should be */
  EnvFlowType FlowType;

 public:
  /****************************************/
  /* SECTION SHOULD ONLY CONTAIN          */
  /* CONSTRUCTORS DESTRUCTORS AND GET AND */
  /* SET FUNCTIONS                        */
  /****************************************/
  
  /* Constructors */
  Env();
  /* Destructors */
  ~Env();
  
  /* Function to set the time stamp and get the time stamp */
  void EnvSetNetlistReadStartTime(void);
  double EnvGetNetlistReadStartTime(void);

  void EnvSetHyperGraphBuildStartTime(void);
  double EnvGetHyperGraphBuildStartTime(void);
  
  void EnvSetClusteringStartTime(void);
  double EnvGetClusteringStartTime(void);
  
  void EnvSetGlobalPlacementStartTime(void);
  double EnvGetGlobalPlacementStartTime(void);
  
  void EnvSetLegalizationStartTime(void);
  double EnvGetLegalizationStartTime(void);
  
  void EnvSetShapeSelectionStartTime(void);
  double EnvGetShapeSelectionStartTime(void);

  void EnvSetDetailedPlacementStartTime(void);
  double EnvGetDetailedPlacementStartTime(void);

  /* Functions to record times of various  */
  void EnvRecordNetlistReadTime(void);
  double EnvGetNetlistReadTime(void);

  void EnvRecordHyperGraphBuildTime(void);
  double EnvGetHyperGraphBuildTime(void);

  void EnvRecordKWayPartitioningTime(double);
  double EnvGetKWayPartitioningTime(void);

  void EnvRecordClusteringTime(void);
  void EnvRecordClusteringTime(double);
  double EnvGetClusteringTime(void);

  void EnvRecordGlobalPlacementTime(void);
  void EnvRecordGlobalPlacementTime(double);
  double EnvGetGlobalPlacementTime(void);

  void EnvRecordFDNetlistBuildTime(double);
  double EnvGetFDNetlistBuildTime(void);

  void EnvRecordFDSolverTime(double);
  double EnvGetFDSolverTime(void);

  void EnvRecordClusterSwappingTime(double);
  double EnvGetClusterSwappingTime(void);

  void EnvRecordClusterFillingTime(double);
  double EnvGetClusterFillingTime(void);

  void EnvRecordUnclusteringTime(double);
  double EnvGetUnclusteringTime(void);
  
  void EnvRecordClusterMirroringTime(double);
  double EnvGetClusterMirroringTime(void);

  void EnvRecordLegalizationTime(void);
  void EnvRecordLegalizationTime(double);
  double EnvGetLegalizationTime(void);

  void EnvRecordShapeSelectionTime(double);
  double EnvGetShapeSelectionTime(void);

  void EnvRecordDetailedPlacementTime(void);
  void EnvRecordDetailedPlacementTime(double);
  double EnvGetDetailedPlacementTime(void);

  /* Functions for recording HPWL */
  void EnvSetHPWLAfterConstructive(ulong);
  ulong EnvGetHPWLAfterConstructive(void);

  void EnvSetHPWLAfterFDPlacement(ulong);
  ulong EnvGetHPWLAfterFDPlacement(void);

  void EnvSetHPWLAfterClusterSwapping(ulong);
  ulong EnvGetHPWLAfterClusterSwapping(void);

  void EnvSetHPWLAfterClusterFill(ulong);
  ulong EnvGetHPWLAfterClusterFill(void);
  
  void EnvSetHPWLAfterClusterMirroring(ulong);
  ulong EnvGetHPWLAfterClusterMirroring(void);

  void EnvSetHPWLTotalInternal(ulong);
  ulong EnvGetHPWLTotalInternal(void);

  void EnvSetHPWLTotalGlobal(ulong);
  ulong EnvGetHPWLTotalGlobal(void);

  void EnvSetHPWLAfterUnclustering(ulong);
  ulong EnvGetHPWLAfterUnclustering(void);

  void EnvSetHPWLAfterLegalization(ulong);
  ulong EnvGetHPWLAfterLegalization(void);

  void EnvSetHPWLAfterDetailedPlacement(ulong);
  ulong EnvGetHPWLAfterDetailedPlacement(void);
  
  void EnvSetHPWLFinal(ulong);
  ulong EnvGetHPWLFinal(void);
  
  /* Functions to set netlist statistics */
  /* Set and get functions in pairs since its */
  /* easy to specify it that way              */
  void EnvSetDesignName(string);
  string EnvGetDesignName(void);

  void EnvSetDesignPath(string);
  string EnvGetDesignPath(void);
  
  void EnvSetNumCellsFromFile(uint);
  uint EnvGetNumCellsFromFile(void);
  
  void EnvSetNumPorts(uint);
  uint EnvGetNumPorts(void);

  void EnvSetNumTerminals(uint);
  uint EnvGetNumTerminals(void);
  
  void EnvSetNumFixedCells(uint);
  uint EnvGetNumFixedCells(void);

  void EnvSetNumMovableCells(uint);
  uint EnvGetNumMovableCells(void);

  void EnvSetNumStdCells(uint);
  uint EnvGetNumStdCells(void);

  void EnvSetNumMacroCells(uint);
  uint EnvGetNumMacroCells(void);

  void EnvSetNumFixedMacros(uint);
  uint EnvGetNumFixedMacros(void);

  void EnvSetNumNets(uint);
  uint EnvGetNumNets(void);

  void EnvSetClusterNumRows(uint);
  uint EnvGetClusterNumRows(void);

  void EnvSetNumClusters(uint);
  uint EnvGetNumClusters(void);
  
  void EnvSetNumShapeVariations(uint);
  uint EnvGetNumShapeVariations(void);

  void EnvSetImbalanceFactor(uint);
  uint EnvGetImbalanceFactor(void);

  void EnvSetNumKHmetisRuns(uint);
  uint EnvGetNumKHmetisRuns(void);

  void EnvSetNumHVariationSteps(uint);
  uint EnvGetNumHVariationSteps(void);

  void EnvSetIterCount(uint);
  uint EnvGetIterCount(void);

  void EnvSetAbortLimit(uint);
  uint EnvGetAbortLimit(void);

  void EnvSetUseVarBounds(bool);
  bool EnvGetUseVarBounds(void);

  void EnvSetDumpHessian(bool);
  bool EnvGetDumpHessian(void);

  void EnvSetDiscreteHeight(bool);
  bool EnvGetDiscreteHeight(void);

  void EnvSetDiscreteWidth(bool);
  bool EnvGetDiscreteWidth(void);

  void EnvSetRecursiveBiPartitioning(bool);
  bool EnvGetRecursiveBiPartitioning(void);

  void EnvSetPlaceCellsInCluster(bool);
  bool EnvGetPlaceCellsInCluster(void);

  void EnvSetUseWeightedHPWL(bool);
  bool EnvGetUseWeightedHPWL(void);
  
  void EnvSetUseFDPlacer(bool);
  bool EnvGetUseFDPlacer(void);

  void EnvSetPlaceCellsInClusterPostTop(bool);
  bool EnvGetPlaceCellsInClusterPostTop(void);

  void EnvSetClusterSwapping(bool);
  bool EnvGetClusterSwapping(void);
  
  void EnvSetClusterMirroring(bool);
  bool EnvGetClusterMirroring(void);
  
  void EnvSetMaxUtilPhaseI(double);
  double EnvGetMaxUtilPhaseI(void);

  void EnvSetClusteringRatio(double);
  double EnvGetClusteringRatio(void);

  void EnvSetClusterMaxArea(double);
  double EnvGetClusterMaxArea(void);

  void EnvSetClusterMaxWidth(double);
  double EnvGetClusterMaxWidth(void);
  
  void EnvSetClusterBoundPenalty(double);
  double EnvGetClusterBoundPenalty(void);

  void EnvSetClusterHVariationPercentage(double);
  double EnvGetClusterHVariationPercentage(void);

  void EnvSetClusterAddAreaPercentage(double);
  double EnvGetClusterAddAreaPercentage(void);

  void EnvSetGlobalPlacerType(EnvGlobalPlacerType);
  EnvGlobalPlacerType EnvGetGlobalPlacerType(void);

  void EnvSetClusterPlacerType(EnvClusterPlacerType);
  EnvClusterPlacerType EnvGetClusterPlacerType(void);

  void EnvSetFlatPlacerType(EnvFlatPlacerType);
  EnvFlatPlacerType EnvGetFlatPlacerType(void);

  void EnvSetSolverType(EnvSolverType);
  EnvSolverType EnvGetSolverType(void);

  void EnvSetNetModel(EnvNetModel);
  EnvNetModel EnvGetNetModel(void);
  
  void EnvSetToolMode(EnvToolMode);
  EnvToolMode EnvGetToolMode(void);
  
  void EnvSetLegalizer(EnvLegalizer);
  EnvLegalizer EnvGetLegalizer(void);

  void EnvSetDetailedPlacer(EnvDetailedPlacer);
  EnvDetailedPlacer EnvGetDetailedPlacer(void);

  void EnvSetOptType(EnvOptType);
  EnvOptType EnvGetOptType(void);
  
  void EnvSetClusterType(EnvClusterType);
  EnvClusterType EnvGetClusterType(void);

  void EnvSetClusterGlobalPlacerType(EnvGlobalPlacerType);
  EnvGlobalPlacerType EnvGetClusterGlobalPlacerType(void);
  
  void EnvSetClusterLegalizerType(EnvLegalizer);
  EnvLegalizer EnvGetClusterLegalizerType(void);

  void EnvSetClusterPlacementType(EnvClusterPlacementType);
  EnvClusterPlacementType EnvGetClusterPlacementType(void);
  
  void EnvSetUnclusterType(EnvUnclusterType);
  EnvUnclusterType EnvGetUnclusterType(void);

  void EnvSetShapeSelectionType(EnvShapeSelectionType);
  EnvShapeSelectionType EnvGetShapeSelectionType(void);
  
  void EnvSetFlowType(EnvFlowType);
  EnvFlowType EnvGetFlowType(void);

  void EnvSetOutputFileName(string);
  string EnvGetOutputFileName(void);
};

# endif
