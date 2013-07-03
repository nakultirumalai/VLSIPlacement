# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>
# include <Flags.h>
# include <Legalize.h>
# include <lemon/list_graph.h>
# include <Env.h>

using namespace lemon;

void
printStartBanner(void)
{

}

void
printSystemStat(void)
{

}

void
printTimeUsage(Env &topEnv)
{
  cout << "***************************************************" << endl;
  cout << "   NETLIST READ TIME: " << topEnv.EnvGetNetlistReadTime() 
       << "s" << endl;
  cout << "   HYPERGRAPH BUILD TIME: " << topEnv.EnvGetHyperGraphBuildTime() 
       << "s" << endl;
  cout << "   CLUSTERING TIME: " << topEnv.EnvGetClusteringTime() 
       << "s" << endl;
  cout << "   GLOBAL PLACEMENT TIME: " << topEnv.EnvGetGlobalPlacementTime() 
       << "s" << endl;
  cout << "   SHAPE SELECTION TIME: " << topEnv.EnvGetShapeSelectionTime() 
       << "s" << endl;
  cout << "   LEGALIZATION TIME: " << topEnv.EnvGetLegalizationTime() 
       << "s" << endl;
  cout << "   DETAILED PLACEMENT TIME: " << topEnv.EnvGetDetailedPlacementTime() 
       << "s" << endl;
  cout << "***************************************************" << endl;
}


/***********************************************************************
  FUNCTION: parseArgsAndAddToEnv
  ABSTRACT:
    This function is called to update the environment with the options
    that are passed to the tool. The supported switches are as follows:

    -analyze: Performs an analysis of the benchmark and dumps plenty 
              of files which are understood by internal scripts to 
              generate a final PDF which can be used to understand the 
              nature of the benchmark. 
              This is a switch and is not followed by any argument.
              THIS OPTION HAS BEEN DISABLED TEMPORARILY

-trace_depth: Trace depth is used to trace the components of the 
              tool. Each major step in the tool contains code to 
              print information about itself. The depth of the functions
              from the top level caller is selected by the trace_depth
              switch. It accepts an integer as an argument.

       -mode: The mode in which the tool should run. Usually, when 
              information about the tool is required, the tool has 
              the tendency to print out information about everything it 
              does to help us understand the flow. However, this is not 
              required as it may affect the speed of the tool. Therefore
              the mode can be switched from the printing mode to the non
              printing mode to avoid any writing as in the case of 
              benchmarking. Values acceptable here are:
              # Class1:
                 optimal: Mode in which the tool is pretty quiet
                 test: Mode in which the tool prints out a lot of stuff 
                       in its own file in each step

              NOTE: Multiple values which are independent shall be used
              in conjunction. No checking is done to see if the values
              are independent. If the value of the same set is mentioned
              twice, the last value stands.

  -net_model: The net model that needs to be used to consider hyperedges. 
              There are four options for this:
              clique : The hypernet is transformed into a 2 terminal nets
                       where each net connects a unique pair of pins
              star   : The hypernet is transformed into star model 
                       where a k-pin net is transformed into k 2-pin connections
                       each of which is connected to the star node of the 
                       net
              hybrid : for k < 4, the hypernet has a clique model and for 
                       k >= 4, the net hypernet has an equivalent star model
                       used in the analytical formulation
              hybrid_controlled: for k < alpha, the hypernet has a clique model 
                       and for k >= alpha, the hypernet has an equivalent
                       star model

   -optimize: The cost to which prominence should be given while 
              optimizing. The options for this are:
              "wirelength"
              "timing"

     -design: The design to select and optimize in the bookshelf 
              format. The complete path to the design's aux file
              should be specified. Expects an aux file.
     
     -solver: The solver to use to solve for the coordinates of 
              the cells. The options can be:
              "mosek" or "conjgrad"

 -use_var_bounds: A flag that instructs the solver to use variable
              bounds. Using this flag enables the use of variable 
              bounds in the solver. This option is only valid 
              when the solver used is mosek.

-global_placer: If this option is specified, an external global placer 
              is selected to perform global placement

-cluster_strategy: If this option is specified, the appropriate strategy
              to perform clustering is selected. The options as of now 
              are : 
              a) bestchoice
              b) firstchoice
              c) netcluster
              d) tdcluster1
              e) tdcluster2
              f) nocluster

-cluster_placement: This option specifies how the cells should be placed
              inside the cluster. The options as of now are:
              a) placeboundary
              b) placecentre
             
-uncluster_strategy: This option specifies how the unclustering of cells
              should be done. 
              a) placeboundary
              b) placenone

  -legalizer: This option specifies which legalizer to use.
              a) binbased
              b) fastplace

-detailed_placer: This option specifies which detailed placer to use
              a) fastplace
              b) ourplacer

-break_util_phase1: This option is a temporary option which is used 
              for experimentation

       -help: Prints out information about the tool
***********************************************************************/
bool
parseArgsAndAddToEnv(string switchName, string switchValue, Env &topEnv)
{
  bool rtv;
  bool designSpecified;
  
  rtv = false;
  designSpecified = false;
  if (switchName == "trace_depth") {
    rtv = true;
    if (!strIsNumber(switchValue)) {
      _ERROR("Trace depth specified is not a number. Assuming 1");
      traceDepth = 1;
    } else {
      traceDepth = atoi(switchValue.data());
    }
  } else if (switchName == "mode") {
    rtv = true;
    if (switchValue == "optimal") {
      topEnv.EnvSetToolMode(ENV_MODE_OPTIMAL);
    } else if (switchValue == "test") {
      topEnv.EnvSetToolMode(ENV_MODE_TEST);
    } else {
      rtv = false;
    }
  } else if (switchName == "net_model") {
    rtv = true;
    if (switchValue == "clique") {
      topEnv.EnvSetNetModel(ENV_CLIQUE_MODEL);
    } else if (switchValue == "star") {
      topEnv.EnvSetNetModel(ENV_STAR_MODEL);
    } else if (switchValue == "hybrid") {
      topEnv.EnvSetNetModel(ENV_HYBRID_MODEL);
    } else if (switchValue == "hybrid_controlled") {
      topEnv.EnvSetNetModel(ENV_VARIABLE_HYBRID_MODEL);
    } else {
      rtv = false;
    }
  } else if (switchName == "optimize") {
    rtv = true;
    if (switchValue == "wirelength") {
      topEnv.EnvSetOptType(ENV_OPTIMIZE_WIRELENGTH);
    } else if (switchValue == "timing") {
      topEnv.EnvSetOptType(ENV_OPTIMIZE_TIMING);
    } else {
      rtv = false;
    }
  } else if (switchName == "design" || switchName == "des" || 
	     switchName == "desi" || switchName == "desig") {
    rtv = true;
    /* Extract the design name and the design path 
       and store it in the env */
    if (!fileExists(switchValue)) {
      cout << "Error: Specified design " 
	   << switchValue
	   << " does not exist!" << endl;
      rtv = false;
    } else {
      uint slashPos = switchValue.find_last_of("/");
      uint dotPos = switchValue.find_last_of(".");
      string DesignPath = switchValue.substr(0, slashPos);
      string DesignName = switchValue.substr(slashPos+1, (dotPos - (slashPos + 1)));
      topEnv.EnvSetDesignPath(DesignPath);
      topEnv.EnvSetDesignName(DesignName);
      designSpecified = true;
    }
  } else if (switchName == "use_var_bounds") {
    rtv = true;
    topEnv.EnvSetUseVarBounds(true);
  } else if (switchName == "solver") {
    rtv = true;
    if (switchValue == "mosek") {
      topEnv.EnvSetSolverType(ENV_SOLVER_QUADRATIC_MOSEK);
    } else if (switchValue == "conjgrad") {
      topEnv.EnvSetSolverType(ENV_SOLVER_QUADRATIC_CONJ_GRAD);
    } else {
      rtv = false;
    }
  } else if (switchName == "global_placer") {
    rtv = true;
    if (switchValue == "ntuplace") {
      topEnv.EnvSetGlobalPlacerType(ENV_NTUPLACE_GP);
    } else if (switchValue == "fastplace") {
      topEnv.EnvSetGlobalPlacerType(ENV_FAST_PLACE_GP);
    } else if (switchValue == "mpl6") {
      topEnv.EnvSetGlobalPlacerType(ENV_MPL6_GP);
    } else {
      rtv = false;
      cout << "Error: Global placers supported are \"ntuplace\", \"fastplace\" and \"mpl6\""
	   << endl;
    }
  } else if (switchName == "cluster_strategy") {
    rtv = true;
    if (switchValue == "bestchoice") {
      topEnv.EnvSetClusterType(ENV_BEST_CHOICE_CLUSTERING);
    } else if (switchValue == "firstchoice") {
      topEnv.EnvSetClusterType(ENV_FIRST_CHOICE_CLUSTERING);
    } else if (switchValue == "netcluster") {
      topEnv.EnvSetClusterType(ENV_NET_CLUSTERING);
    } else if (switchValue == "tdcluster1") {
      topEnv.EnvSetClusterType(ENV_TIMING_DRIVEN_CLUSTERING1);
    } else if (switchValue == "tdcluster2") {
      topEnv.EnvSetClusterType(ENV_TIMING_DRIVEN_CLUSTERING2);
    } else {
      rtv = false;
      cout << "Error: Valid options for cluster_strategy are " 
	   << "\"bestchoice\", \"firstchoice\", \"netcluster\" "
	   << "\"tdcluster1\", \"tdcluster2\"" << endl;
    }
  } else if (switchName == "cluster_placement") {
    
  } else if (switchName == "uncluster_strategy") {

  } else if (switchName == "legalizer") {
    rtv = true;
    if (switchValue == "binbased") {
      topEnv.EnvSetLegalizer(ENV_BIN_BASED_LEGALIZER);
    } else if (switchValue == "fastplace") {
      topEnv.EnvSetLegalizer(ENV_FAST_PLACE_LEGALIZER);
    } else {
      rtv = false;
      cout << "Error: Valid options for legalizer are " 
	   << "\"binbased\", \"fastplace\"" 
	   << endl;
    }
  } else if (switchName == "detailed_placer") {
    rtv = true;
    if (switchValue == "fastplace") {
      topEnv.EnvSetDetailedPlacer(ENV_FAST_PLACE_DP);
    } else if (switchValue == "ourplacer") {
      topEnv.EnvSetDetailedPlacer(ENV_OURPLACER_DP);
    } else if (switchValue == "none") {
      topEnv.EnvSetDetailedPlacer(ENV_NO_DETAIL_PLACEMENT);
    } else {
      rtv = false;
      cout << "Error: Valid options for detailed_placer are " 
	   << "\"fastplace\", \"ourplacer\"" 
	   << endl;
    }
  } else if (switchName == "break_util_phase1") {
    rtv = true;
    double value = strToDouble(switchValue);
    topEnv.EnvSetMaxUtilPhaseI(value);
  } else if (switchName == "help") {
    cout << "To execute the tool, use the following options:" << endl;
    cout << endl << "-trace_depth: Given a trace depth, prints cputime and memory for all routines " << endl;
    cout << "-mode: Optimization mode to be selected. Choose \"test\" or \"optimal\"" << endl;
    cout << "-optimize: Choose the parameter to be optimized. Choose \"wirelength\" or \"timing\"" << endl;
    cout << "-design: Compulsory option. Specify the complete path of the design in the designName.aux format" << endl;
  }
  return (rtv);
}

/***********************************************************************
  FUNCTION: placeMain
  ABSTRACT:
            Main function to run the placer. Does the initialization,
            creation of the design and so on.
***********************************************************************/
int placeMain(Env &topEnv)
{
  string DesignName, DesignPath;
  
  /* Initialize the flags here */
  FlagsInit();
  
  /* Get the design's name and path from the environment */
  DesignName = topEnv.EnvGetDesignName();
  DesignPath = topEnv.EnvGetDesignPath();

  /* Create the design */
  Design myDesign(DesignPath, DesignName, topEnv);

  Env &DesignEnv = myDesign.DesignGetEnv();

  /****************************************************
   *  BUILD THE HYPERGRAPH                            *
   ****************************************************/
  /* Record the start time of building the hypergraph */
  DesignEnv.EnvSetHyperGraphBuildStartTime();
  myDesign.DesignBuildGraph();
  /* Record the hypergraph build time */  
  DesignEnv.EnvRecordHyperGraphBuildTime();

  /****************************************************
   *  DO GLOBAL PLACEMENT                             *
   ****************************************************/
  ProfilerStart("GlobalPlacement");
  myDesign.DesignDoGlobalPlacement();
  ProfilerStart("GlobalPlacement");
    
  /****************************************************
   *  DO LEGALIZATION                                 *
   ****************************************************/
  myDesign.DesignDoLegalization();

  /****************************************************
   *  DO DETAILED PLACEMENT                           *
   ****************************************************/
  myDesign.DesignDoDetailedPlacement();

  /****************************************************
   *  PLOT THE FINAL PLACEMENT                        *
   ****************************************************/
  string plotFileName;
  plotFileName = "final.plt";
  myDesign.DesignPlotData("Title", plotFileName);

  /****************************************************
   *  WRITE THE FINAL PLACEMENT                       *
   ****************************************************/
  DesignWriteOutputPlacement(myDesign); 

  topEnv = DesignEnv;
  return 0;
}

int main(int argc, char *argv[])
{
  Env topEnv;
  string switchName, switchValue;
  bool proceed;
  int i, rtv;

  proceed = false;
  if (argc > 1) {
    i = 1;
    while (i < argc) {
      if (argv[i][0] == '-') {
	switchName = argv[i] + 1;
	if (i + 1 < argc) {
	  switchValue = argv[i+1];
	}
	proceed = parseArgsAndAddToEnv(switchName, switchValue, topEnv);
	if (!proceed) break;
      }
      i++;
    }
    if (!proceed) {
      cout << "Exiting!" << endl;
      exit(0);
    }
    FlagsInit();
  } else {
    cout << "Usage: <exec> -design <AuxFilePath>/<DesignName>.aux " << endl;
    exit(0);
  }

  printStartBanner();
  printSystemStat();
  rtv = placeMain(topEnv);
  printTimeUsage(topEnv);

  return (rtv);
}

