# include <Env.h>

Env::Env() 
{
  EnvSetDesignName(DEFAULT_DESIGN_NAME);
  EnvSetDesignPath(DEFAULT_DESIGN_PATH);
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
  EnvSetOptType(DEFAULT_ENV_OPT_TYPE);
  EnvSetClusterType(DEFAULT_ENV_CLUSTER_TYPE);
  EnvSetShapeSelectionType(DEFAULT_ENV_SHAPE_SELECTION_TYPE);

  /* Time related variables */
  StartTime = 0;
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
Env::EnvSetStartTime(void)
{
  this->StartTime = getCPUTime();
}

double
Env::EnvGetStartTime(void)
{
  return (StartTime);
}

void
Env::EnvSetNetlistReadTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();
 
  NetlistReadTime = currTime - startTime;
}

double
Env::EnvGetNetlistReadTime(void)
{
  return (this->NetlistReadTime);
}

void
Env::EnvSetHyperGraphBuildTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();
 
  HyperGraphBuildTime = currTime - startTime;
}

double
Env::EnvGetHyperGraphBuildTime(void)
{
  this->HyperGraphBuildTime;
}

void
Env::EnvSetClusteringTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();
 
  ClusteringTime = currTime - startTime;
}

double 
Env::EnvGetClusteringTime(void)
{
  return (this->ClusteringTime);
}

void
Env::EnvSetGlobalPlacementTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();
  
  GlobalPlacementTime = currTime - startTime;
}

double 
Env::EnvGetGlobalPlacementTime(void)
{
  return (this->GlobalPlacementTime);
}

void
Env::EnvSetLegalizationTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();

  LegalizationTime = currTime - startTime;
}

double
Env::EnvGetLegalizationTime(void)
{
  return (this->LegalizationTime);
}

void
Env::EnvSetShapeSelectionTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();

  ShapeSelectionTime = currTime - startTime;
}

double
Env::EnvGetShapeSelectionTime(void)
{
  return (this->ShapeSelectionTime);
}

void
Env::EnvSetDetailedPlacementTime(void)
{
  double startTime = EnvGetStartTime();
  double currTime = getCPUTime();

  DetailedPlacementTime = currTime - startTime;
}

double
Env::EnvGetDetailedPlacementTime(void)
{
  return (DetailedPlacementTime);
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
Env::EnvSetUnClusterType(EnvUnClusterType UnClusterType)
{
  this->UnClusterType = UnClusterType;
}

EnvUnClusterType 
Env::EnvGetUnClusterType(void)
{
  return UnClusterType;
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
