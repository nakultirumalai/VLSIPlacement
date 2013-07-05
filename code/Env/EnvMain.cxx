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
  EnvSetGlobalPlacerType(DEFAULT_ENV_GLOBAL_PLACER_TYPE);
  EnvSetSolverType(DEFAULT_ENV_SOLVER_TYPE);
  EnvSetNetModel(DEFAULT_ENV_NET_MODEL);
  EnvSetToolMode(DEFAULT_ENV_TOOL_MODE);
  EnvSetLegalizer(DEFAULT_ENV_LEGALIZER);
  EnvSetDetailedPlacer(DEFAULT_ENV_DETAIL_PLACEMENT);
  EnvSetOptType(DEFAULT_ENV_OPT_TYPE);
  EnvSetClusterType(DEFAULT_ENV_CLUSTER_TYPE);
  EnvSetClusterPlacementType(DEFAULT_ENV_CLUSTER_PLACEMENT_TYPE);
  EnvSetUnclusterType(DEFAULT_ENV_UNCLUSTER_TYPE);
  EnvSetShapeSelectionType(DEFAULT_ENV_SHAPE_SELECTION_TYPE);
  /* Initilization of double variables */
  EnvSetMaxUtilPhaseI(DEFAULT_MAX_UTILIZATION_PHASEI);
  EnvSetClusteringRatio(DEFAULT_CLUSTERING_RATIO);
  EnvSetClusterMaxArea(DEFAULT_CLUSTER_MAX_AREA);
  EnvSetClusterMaxWidth(DEFAULT_CLUSTER_MAX_WIDTH);
  EnvSetClusterBoundPenalty(DEFAULT_CLUSTER_BOUND_PENALTY);
  /* Initialization of other integer variables */
  EnvSetClusterNumRows(DEFAULT_CLUSTER_NUM_ROWS);
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
Env::EnvSetOutputFileName(string OutputFileName)
{
  this->OutputFileName = OutputFileName;
}

string
Env::EnvGetOutputFileName(void)
{
  return OutputFileName;
}
