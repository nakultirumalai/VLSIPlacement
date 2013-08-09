# include <Env.h>

Env::Env() 
{
  EnvSetNumCellsFromFile(0);
  EnvSetNumPorts(0);
  EnvSetNumTerminals(0);
  EnvSetNumFixedCells(0);
  EnvSetNumMovableCells(0);
  EnvSetNumStdCells(0);
  EnvSetNumMacroCells(0);
  EnvSetNumFixedMacros(0);
  EnvSetNumNets(0);
  EnvSetUseVarBounds(false);
  EnvSetDumpHessian(false);  /* WATCH: Default value is true */
  EnvSetDiscreteWidth(true); /* WATCH: Default value is true */
  EnvSetDiscreteHeight(true); /* WATCH: Default value is true */
  EnvSetRecursiveBiPartitioning(false);
  EnvSetPlaceCellsInCluster(true);
  EnvSetPlaceCellsInClusterPostTop(true);
  EnvSetUseFDPlacer(DEFAULT_USE_FD_PLACER);
  EnvSetGlobalPlacerType(DEFAULT_ENV_GLOBAL_PLACER_TYPE);
  EnvSetSolverType(DEFAULT_ENV_SOLVER_TYPE);
  EnvSetNetModel(DEFAULT_ENV_NET_MODEL);
  EnvSetToolMode(DEFAULT_ENV_TOOL_MODE);
  EnvSetLegalizer(DEFAULT_ENV_LEGALIZER);
  EnvSetDetailedPlacer(DEFAULT_ENV_DETAIL_PLACEMENT);
  EnvSetOptType(DEFAULT_ENV_OPT_TYPE);
  EnvSetClusterType(DEFAULT_ENV_CLUSTER_TYPE);
  EnvSetClusterGlobalPlacerType(DEFAULT_ENV_GLOBAL_PLACER_TYPE);
  EnvSetClusterLegalizerType(DEFAULT_ENV_LEGALIZER);
  EnvSetClusterPlacementType(DEFAULT_ENV_CLUSTER_PLACEMENT_TYPE);
  EnvSetUnclusterType(DEFAULT_ENV_UNCLUSTER_TYPE);
  EnvSetShapeSelectionType(DEFAULT_ENV_SHAPE_SELECTION_TYPE);
  EnvSetFlowType(DEFAULT_ENV_FLOW_TYPE);
  /* Initilization of double variables */
  EnvSetMaxUtilPhaseI(DEFAULT_MAX_UTILIZATION_PHASEI);
  EnvSetClusteringRatio(DEFAULT_CLUSTERING_RATIO);
  EnvSetClusterMaxArea(DEFAULT_CLUSTER_MAX_AREA);
  EnvSetClusterMaxWidth(DEFAULT_CLUSTER_MAX_WIDTH);
  EnvSetClusterBoundPenalty(DEFAULT_CLUSTER_BOUND_PENALTY);
  EnvSetClusterHVariationPercentage(DEFAULT_CLUSTER_HVARIATION_PERCENTAGE);
  EnvSetClusterAddAreaPercentage(DEFAULT_CLUSTER_ADDITIONAL_SPACE_PERCENT);
  /* Initialization of other integer variables */
  EnvSetClusterNumRows(DEFAULT_CLUSTER_NUM_ROWS);
  EnvSetNumClusters(DEFAULT_NUM_CLUSTERS);
  EnvSetImbalanceFactor(DEFAULT_IMBALANCE_FACTOR);
  EnvSetNumKHmetisRuns(DEFAULT_NUM_KHMETIS_RUNS);
  EnvSetNumHVariationSteps(DEFAULT_NUM_HVARIATION_STEPS);
  /* Initialization of HPWL */
  EnvSetHPWLAfterConstructive(0);
  EnvSetHPWLAfterFDPlacement(0);
  EnvSetHPWLAfterClusterSwapping(0);
  EnvSetHPWLTotalInternal(0);
  EnvSetHPWLTotalGlobal(0);
  EnvSetHPWLAfterClusterFill(0);
  EnvSetHPWLAfterUnclustering(0);
  EnvSetHPWLAfterLegalization(0);
  EnvSetHPWLAfterDetailedPlacement(0);
  EnvSetHPWLAfterUnclustering(0);
  EnvSetHPWLFinal(0);

  /* Time related variables */
  /* Initialize start times to 0 */
  NetlistReadStartTime = 0;
  HyperGraphBuildStartTime = 0;
  ClusteringStartTime = 0;
  GlobalPlacementStartTime = 0;
  LegalizationStartTime = 0;
  ShapeSelectionStartTime = 0;
  DetailedPlacementStartTime = 0;

  NetlistReadTime = 0;
  HyperGraphBuildTime = 0;
  ClusteringTime = 0;
  GlobalPlacementTime = 0;
  LegalizationTime = 0;
  ShapeSelectionTime = 0;
  DetailedPlacementTime = 0;
}

Env::~Env()
{

}

void 
Env::EnvSetNetlistReadStartTime(void)
{
  this->NetlistReadStartTime = getCPUTime();
}

double 
Env::EnvGetNetlistReadStartTime(void)
{
  return (this->NetlistReadStartTime);
}

void 
Env::EnvSetHyperGraphBuildStartTime(void)
{
  this->HyperGraphBuildStartTime = getCPUTime();
}
double 
Env::EnvGetHyperGraphBuildStartTime(void)
{
  return (this->HyperGraphBuildStartTime);
}
  
void 
Env::EnvSetClusteringStartTime(void)
{
  this->ClusteringStartTime = getCPUTime();
}

double 
Env::EnvGetClusteringStartTime(void)
{
  return (this->ClusteringStartTime);
}
  
void 
Env::EnvSetGlobalPlacementStartTime(void)
{
  this->GlobalPlacementStartTime = getCPUTime();
}

double 
Env::EnvGetGlobalPlacementStartTime(void)
{
  return (this->GlobalPlacementStartTime);
}

void 
Env::EnvSetLegalizationStartTime(void)
{
  this->LegalizationStartTime = getCPUTime();
}

double 
Env::EnvGetLegalizationStartTime(void)
{
  return (this->LegalizationStartTime);
}
  
void 
Env::EnvSetShapeSelectionStartTime(void)
{
  this->ShapeSelectionStartTime = getCPUTime();
}

double 
Env::EnvGetShapeSelectionStartTime(void)
{
  return (this->ShapeSelectionStartTime);
}

void 
Env::EnvSetDetailedPlacementStartTime(void)
{
  this->DetailedPlacementStartTime = getCPUTime();
}

double 
Env::EnvGetDetailedPlacementStartTime(void)
{
  return (this->DetailedPlacementStartTime);
}

void
Env::EnvRecordNetlistReadTime(void)
{
  double currTime = getCPUTime();

  this->NetlistReadTime = 
    currTime - this->NetlistReadStartTime;
}

double
Env::EnvGetNetlistReadTime(void)
{
  return (this->NetlistReadTime);
}

void
Env::EnvRecordHyperGraphBuildTime(void)
{
  double currTime = getCPUTime();

  this->HyperGraphBuildTime = 
    currTime - this->HyperGraphBuildStartTime;
}

double
Env::EnvGetHyperGraphBuildTime(void)
{
  return (this->HyperGraphBuildTime);
}

void
Env::EnvRecordClusteringTime(double clusteringTime)
{
  this->ClusteringTime = clusteringTime;
}

void
Env::EnvRecordClusteringTime(void)
{
  double currTime = getCPUTime();

  this->ClusteringTime = 
    currTime - this->ClusteringStartTime;
}

double 
Env::EnvGetClusteringTime(void)
{
  return (this->ClusteringTime);
}

void 
Env::EnvRecordGlobalPlacementTime(void)
{
  double currTime = getCPUTime();

  this->GlobalPlacementTime += 
    currTime - this->GlobalPlacementStartTime;
}

void 
Env::EnvRecordGlobalPlacementTime(double globalPlacementTime)
{
  this->GlobalPlacementTime += globalPlacementTime;
}

double 
Env::EnvGetGlobalPlacementTime(void)
{
  return (this->GlobalPlacementTime);
}

void
Env::EnvRecordLegalizationTime(void)
{
  double currTime = getCPUTime();

  this->LegalizationTime = 
    currTime - this->LegalizationStartTime;
}

void 
Env::EnvRecordLegalizationTime(double legalizationTime)
{
  this->LegalizationTime += legalizationTime;
}

double
Env::EnvGetLegalizationTime(void)
{
  return this->LegalizationTime;
}

void
Env::EnvRecordShapeSelectionTime(void)
{
  double currTime = getCPUTime();

  this->ShapeSelectionTime = 
    currTime - this->ShapeSelectionStartTime;
}

double
Env::EnvGetShapeSelectionTime(void)
{
  return (this->ShapeSelectionTime);
}

void
Env::EnvRecordDetailedPlacementTime(void)
{
  double currTime = getCPUTime();

  this->DetailedPlacementTime = 
    currTime - this->DetailedPlacementStartTime;
}

void 
Env::EnvRecordDetailedPlacementTime(double detailedPlacementTime)
{
  this->DetailedPlacementTime += detailedPlacementTime;
}

double
Env::EnvGetDetailedPlacementTime(void)
{
  return (this->DetailedPlacementTime);
}

/* HPWL Recording functions */
void
Env::EnvSetHPWLAfterConstructive(ulong HPWLAfterConstructive)
{
  this->HPWLAfterConstructive = HPWLAfterConstructive;
}

ulong
Env::EnvGetHPWLAfterConstructive(void)
{
  return (HPWLAfterConstructive);
}

void 
Env::EnvSetHPWLAfterFDPlacement(ulong HPWLAfterFDPlacement)
{
  this->HPWLAfterFDPlacement = HPWLAfterFDPlacement;
}

ulong 
Env::EnvGetHPWLAfterFDPlacement(void)
{
  return (HPWLAfterFDPlacement);
}

void
Env::EnvSetHPWLAfterClusterSwapping(ulong HPWLAfterClusterSwapping)
{
  this->HPWLAfterClusterSwapping = HPWLAfterClusterSwapping;
}

ulong
Env::EnvGetHPWLAfterClusterSwapping(void)
{
  return (HPWLAfterClusterSwapping);
}

void
Env::EnvSetHPWLTotalInternal(ulong HPWLTotalInternal)
{
  this->HPWLTotalInternal = HPWLTotalInternal;
}

ulong
Env::EnvGetHPWLTotalInternal(void)
{
  return (HPWLTotalInternal);
}

void
Env::EnvSetHPWLTotalGlobal(ulong HPWLTotalGlobal)
{
  this->HPWLTotalGlobal;
}

ulong
Env::EnvGetHPWLTotalGlobal(void)
{
  return (HPWLTotalGlobal);
}

void
Env::EnvSetHPWLAfterClusterFill(ulong HPWLAfterClusterFill)
{
  this->HPWLAfterClusterFill = HPWLAfterClusterFill;
}

ulong
Env::EnvGetHPWLAfterClusterFill(void)
{
  return (HPWLAfterClusterFill);
}

void
Env::EnvSetHPWLAfterLegalization(ulong HPWLAfterLegalization)
{
  this->HPWLAfterLegalization = HPWLAfterLegalization;
}

ulong
Env::EnvGetHPWLAfterLegalization(void)
{
  return (HPWLAfterLegalization);
}

void
Env::EnvSetHPWLAfterDetailedPlacement(ulong HPWLAfterDetailedPlacement)
{
  this->HPWLAfterDetailedPlacement = HPWLAfterDetailedPlacement;
}

ulong
Env::EnvGetHPWLAfterDetailedPlacement(void)
{
  return (HPWLAfterDetailedPlacement);
}

void
Env::EnvSetHPWLAfterUnclustering(ulong HPWLAfterUnclustering)
{
  this->HPWLAfterUnclustering = HPWLAfterUnclustering;
}

ulong
Env::EnvGetHPWLAfterUnclustering(void)
{
  return (HPWLAfterUnclustering);
}

void
Env::EnvSetHPWLFinal(ulong HPWLFinal)
{
  this->HPWLFinal = HPWLFinal;
}

ulong
Env::EnvGetHPWLFinal(void)
{
  return (HPWLFinal);
}

void 
Env::EnvSetDesignName(string DesignName)
{
  this->DesignName = DesignName;
}

string
Env::EnvGetDesignName(void)
{
  return DesignName;
}

void
Env::EnvSetDesignPath(string DesignPath)
{
  this->DesignPath = DesignPath;
}

string 
Env::EnvGetDesignPath(void)
{
  return DesignPath;
}
  
void 
Env::EnvSetNumCellsFromFile(uint NumCellsFromFile)
{
  this->NumCellsFromFile = NumCellsFromFile;
}
uint
Env::EnvGetNumCellsFromFile(void)
{
  return NumCellsFromFile;
}
  
void 
Env::EnvSetNumPorts(uint NumPorts)
{
  this->NumPorts = NumPorts;
}
uint 
Env::EnvGetNumPorts(void)
{
  return NumPorts;
}

void 
Env::EnvSetNumTerminals(uint NumTerminals)
{
  this->NumTerminals = NumTerminals;
}
uint 
Env::EnvGetNumTerminals(void)
{
  return NumTerminals;
}
  
void 
Env::EnvSetNumFixedCells(uint NumFixedCells)
{
  this->NumFixedCells = NumFixedCells;
}
uint 
Env::EnvGetNumFixedCells(void)
{
  return NumFixedCells;
}

void 
Env::EnvSetNumMovableCells(uint NumMovableCells)
{
  this->NumMovableCells;
}
uint 
Env::EnvGetNumMovableCells(void)
{
  return NumMovableCells;
}

void 
Env::EnvSetNumStdCells(uint NumStdCells)
{
  this->NumStdCells = NumStdCells;
}
uint 
Env::EnvGetNumStdCells(void)
{
  return NumStdCells;
}

void 
Env::EnvSetNumMacroCells(uint NumMacroCells)
{
  this->NumMacroCells = NumMacroCells;
}
uint 
Env::EnvGetNumMacroCells(void)
{
  return NumMacroCells;
}

void 
Env::EnvSetNumFixedMacros(uint NumFixedMacros)
{
  this->NumFixedMacros = NumFixedMacros;
}
uint 
Env::EnvGetNumFixedMacros(void)
{
  return NumFixedMacros;
}

void 
Env::EnvSetNumNets(uint NumNets)
{
  this->NumNets = NumNets;
}

uint 
Env::EnvGetNumNets(void)
{
  return NumNets;
}

void
Env::EnvSetClusterNumRows(uint ClusterNumRows)
{
  this->ClusterNumRows = ClusterNumRows;
}

uint 
Env::EnvGetClusterNumRows(void)
{
  return (ClusterNumRows);
}

void 
Env::EnvSetNumClusters(uint NumClusters)
{
  this->NumClusters = NumClusters;
}

uint
Env::EnvGetNumClusters(void)
{
  return (this->NumClusters);
}

void
Env::EnvSetImbalanceFactor(uint ImbalanceFactor)
{
  this->ImbalanceFactor = ImbalanceFactor;
}

uint
Env::EnvGetImbalanceFactor(void)
{
  return (this->ImbalanceFactor);
}

void
Env::EnvSetNumKHmetisRuns(uint NumKHmetisRuns) 
{
  this->NumKHmetisRuns = NumKHmetisRuns;
}

uint
Env::EnvGetNumKHmetisRuns(void)
{
  return (this->NumKHmetisRuns);
}

void
Env::EnvSetNumHVariationSteps(uint NumHVariationSteps) 
{
  this->NumHVariationSteps = NumHVariationSteps;
}

uint
Env::EnvGetNumHVariationSteps(void)
{
  return (this->NumHVariationSteps);
}

void 
Env::EnvSetUseVarBounds(bool UseVarBounds)
{
  this->UseVarBounds = UseVarBounds;
}

bool 
Env::EnvGetUseVarBounds(void)
{
  return UseVarBounds;
}

void 
Env::EnvSetDumpHessian(bool DumpHessian)
{
  this->DumpHessian = DumpHessian;
}

bool 
Env::EnvGetDumpHessian(void)
{
  return DumpHessian;
}

void 
Env::EnvSetDiscreteHeight(bool DiscreteHeight)
{
  this->DiscreteHeight = DiscreteHeight;
}

bool 
Env::EnvGetDiscreteHeight(void)
{
  return DiscreteHeight;
}

void 
Env::EnvSetDiscreteWidth(bool DiscreteWidth)
{
  this->DiscreteWidth = DiscreteWidth;
}

bool 
Env::EnvGetDiscreteWidth(void)
{
  return DiscreteWidth;
}

void
Env::EnvSetRecursiveBiPartitioning(bool RecursiveBiPartitioning)
{
  this->RecursiveBiPartitioning = RecursiveBiPartitioning;
}

bool
Env::EnvGetRecursiveBiPartitioning(void) 
{
  return (this->RecursiveBiPartitioning);
}

void
Env::EnvSetPlaceCellsInCluster(bool PlaceCellsInCluster) 
{
  this->PlaceCellsInCluster = PlaceCellsInCluster;
}

bool
Env::EnvGetPlaceCellsInCluster(void)
{
  return (this->PlaceCellsInCluster);
}

void
Env::EnvSetUseFDPlacer(bool UseFDPlacer) 
{
  this->UseFDPlacer = UseFDPlacer;
}

bool
Env::EnvGetUseFDPlacer(void)
{
  return (this->UseFDPlacer);
}

void
Env::EnvSetPlaceCellsInClusterPostTop(bool PlaceCellsInClusterPostTop)
{
  this->PlaceCellsInClusterPostTop = PlaceCellsInClusterPostTop;
}

bool
Env::EnvGetPlaceCellsInClusterPostTop(void)
{
  return (this->PlaceCellsInClusterPostTop);
}

void
Env::EnvSetMaxUtilPhaseI(double MaxUtilPhaseI) 
{
  this->MaxUtilPhaseI = MaxUtilPhaseI;
}

double
Env::EnvGetMaxUtilPhaseI(void) 
{
  return (MaxUtilPhaseI);
}

void
Env::EnvSetClusteringRatio(double ClusteringRatio) 
{
  this->ClusteringRatio = ClusteringRatio;
}

double
Env::EnvGetClusteringRatio(void) 
{
  return (ClusteringRatio);
}

void
Env::EnvSetClusterMaxArea(double ClusterMaxArea) 
{
  this->ClusterMaxArea = ClusterMaxArea;
}

double
Env::EnvGetClusterMaxArea(void) 
{
  return (ClusterMaxArea);
}

void
Env::EnvSetClusterMaxWidth(double ClusterMaxWidth)
{
  this->ClusterMaxWidth = ClusterMaxWidth;
}

double
Env::EnvGetClusterMaxWidth(void) 
{
  return (ClusterMaxWidth);
}

void
Env::EnvSetClusterBoundPenalty(double ClusterBoundPenalty) 
{
  this->ClusterBoundPenalty = ClusterBoundPenalty;
}

double
Env::EnvGetClusterBoundPenalty(void) 
{
  return (ClusterBoundPenalty);
}

void
Env::EnvSetClusterHVariationPercentage(double ClusterHVariationPercentage) 
{
  this->ClusterHVariationPercentage = ClusterHVariationPercentage;
}

double
Env::EnvGetClusterHVariationPercentage(void) 
{
  return (ClusterHVariationPercentage);
}

void
Env::EnvSetClusterAddAreaPercentage(double ClusterAddAreaPercentage) 
{
  this->ClusterAddAreaPercentage = ClusterAddAreaPercentage;
}

double
Env::EnvGetClusterAddAreaPercentage(void) 
{
  return (ClusterAddAreaPercentage);
}

void 
Env::EnvSetGlobalPlacerType(EnvGlobalPlacerType GlobalPlacerType)
{
  this->GlobalPlacerType = GlobalPlacerType;
}

EnvGlobalPlacerType
Env::EnvGetGlobalPlacerType(void)
{
  return GlobalPlacerType;
}

void 
Env::EnvSetSolverType(EnvSolverType SolverType)
{
  this->SolverType = SolverType;
}

EnvSolverType 
Env::EnvGetSolverType(void)
{
  return SolverType;
}

void   
Env::EnvSetNetModel(EnvNetModel NetModel)
{
  this->NetModel = NetModel;
}

EnvNetModel
Env::EnvGetNetModel(void)
{
  return NetModel;
}

void 
Env::EnvSetToolMode(EnvToolMode ToolMode)
{
  this->ToolMode = ToolMode;
}
EnvToolMode 
Env::EnvGetToolMode(void)
{
  return ToolMode;
}
  
void 
Env::EnvSetLegalizer(EnvLegalizer Legalizer)
{
  this->Legalizer = Legalizer;
}

EnvLegalizer 
Env::EnvGetLegalizer(void)
{
  return Legalizer;
}

void
Env::EnvSetDetailedPlacer(EnvDetailedPlacer DetailedPlacer)
{
  this->DetailedPlacer = DetailedPlacer;
}

EnvDetailedPlacer
Env::EnvGetDetailedPlacer(void)
{
  return DetailedPlacer;
}

void 
Env::EnvSetOptType(EnvOptType OptType)
{
  this->OptType = OptType;
}

EnvOptType 
Env::EnvGetOptType(void)
{
  return OptType;
}

void 
Env::EnvSetClusterType(EnvClusterType ClusterType)
{
  this->ClusterType = ClusterType;
}

EnvClusterType 
Env::EnvGetClusterType(void)
{
  return ClusterType;
}

void
Env::EnvSetClusterGlobalPlacerType(EnvGlobalPlacerType ClusterGlobalPlacer) 
{
  this->ClusterGlobalPlacer = ClusterGlobalPlacer;
}

EnvGlobalPlacerType
Env::EnvGetClusterGlobalPlacerType(void)
{
  return (this->ClusterGlobalPlacer);
}

void
Env::EnvSetClusterLegalizerType(EnvLegalizer ClusterLegalizer)
{
  this->ClusterLegalizer = ClusterLegalizer;
}

EnvLegalizer
Env::EnvGetClusterLegalizerType(void)
{
  return (this->ClusterLegalizer);
}

void 
Env::EnvSetClusterPlacementType(EnvClusterPlacementType ClusterPlacementType) 
{
  this->ClusterPlacementType = ClusterPlacementType;
}

EnvClusterPlacementType 
Env::EnvGetClusterPlacementType(void)
{
  return ClusterPlacementType;
}

void 
Env::EnvSetUnclusterType(EnvUnclusterType UnclusterType)
{
  this->UnclusterType = UnclusterType;
}

EnvUnclusterType 
Env::EnvGetUnclusterType(void)
{
  return UnclusterType;
}

void 
Env::EnvSetShapeSelectionType(EnvShapeSelectionType ShapeSelectionType)
{
  this->ShapeSelectionType = ShapeSelectionType;
}
EnvShapeSelectionType 
Env::EnvGetShapeSelectionType(void)
{
  return ShapeSelectionType;
}

void 
Env::EnvSetFlowType(EnvFlowType FlowType)
{
  this->FlowType = FlowType;
}

EnvFlowType 
Env::EnvGetFlowType(void)
{
  return (FlowType);
}


void
Env::EnvSetOutputFileName(string OutputFileName)
{
  this->OutputFileName = OutputFileName;
}

string
Env::EnvGetOutputFileName(void)
{
  return OutputFileName;
}
