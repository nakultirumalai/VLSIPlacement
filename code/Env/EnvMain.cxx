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
  EnvSetDumpHessian(true);  /* WATCH: Default value is true */
  EnvSetSolverType(DEFAULT_ENV_SOLVER_TYPE);
  EnvSetNetModel(DEFAULT_ENV_NET_MODEL);
  EnvSetToolMode(DEFAULT_ENV_TOOL_MODE);
  EnvSetLegalizer(DEFAULT_ENV_LEGALIZER);
  EnvSetOptType(DEFAULT_ENV_OPT_TYPE);
  EnvSetClusterType(DEFAULT_ENV_CLUSTER_TYPE);
  EnvSetShapeSelectionType(DEFAULT_ENV_SHAPE_SELECTION_TYPE);
}

Env::~Env()
{
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
Env::EnvSetShapeSelectionType(EnvShapeSelectionType ShapeSelectionType)
{
  this->ShapeSelectionType = ShapeSelectionType;
}
EnvShapeSelectionType 
Env::EnvGetShapeSelectionType(void)
{
  return ShapeSelectionType;
}
