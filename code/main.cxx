# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>
# include <Flags.h>
# include <Legalize.h>
# include <Env.h>

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
  double kwayPartTime, clusteringTime;
  double fdNetlistBuildTime, fdSolverTime;
  double clusterSwappingTime, clusterMirroringTime;
  double clusterFillTime, unclusteringTime;
  double shapeSelectionTime, totalGPTime;
  double legalizationTime, detailedPlacementTime;
  double totalTime;
  
  kwayPartTime = topEnv.EnvGetKWayPartitioningTime();
  clusteringTime = topEnv.EnvGetClusteringTime();
  fdNetlistBuildTime = topEnv.EnvGetFDNetlistBuildTime();
  fdSolverTime = topEnv.EnvGetFDSolverTime();
  clusterSwappingTime = topEnv.EnvGetClusterSwappingTime();
  clusterMirroringTime = topEnv.EnvGetClusterMirroringTime();
  shapeSelectionTime = topEnv.EnvGetShapeSelectionTime();
  clusteringTime = topEnv.EnvGetClusteringTime();
  unclusteringTime = topEnv.EnvGetUnclusteringTime();
  clusterFillTime = topEnv.EnvGetClusterFillingTime();
  legalizationTime = topEnv.EnvGetLegalizationTime();
  detailedPlacementTime = topEnv.EnvGetDetailedPlacementTime();

  if (topEnv.EnvGetGlobalPlacerType() != ENV_NO_EXTERNAL_GP ||
      topEnv.EnvGetFlatPlacerType() != ENV_NO_EXTERNAL_FP) {
    totalGPTime = topEnv.EnvGetGlobalPlacementTime();
  } else {
    totalGPTime = kwayPartTime + clusteringTime + fdNetlistBuildTime + fdSolverTime;
    totalGPTime += clusterSwappingTime + clusterMirroringTime + clusterFillTime;
    totalGPTime += unclusteringTime;
  }
  totalTime = totalGPTime + legalizationTime + detailedPlacementTime;

  cout << "***************************************************" << endl;
  cout << "   TIME RECORD THROUGH STEPS  " << endl;
  cout << "   NETLIST READ       : " << topEnv.EnvGetNetlistReadTime() << " s" << endl;
  cout << "   HYPERGRAPH BUILD   : " << topEnv.EnvGetHyperGraphBuildTime() << " s" << endl;
  cout << "   KWAY PARITIONING   : " << kwayPartTime << " s" 
       << "\t (" << getPercent(kwayPartTime, totalTime) << " %)" 
       << endl;
  cout << "   CLUSTERING         : " << clusteringTime << " s" 
       << "\t (" << getPercent(clusteringTime, totalTime) << " %)" 
       << endl;
  cout << "   FDNETLIST BUILD    : " << fdNetlistBuildTime << " s" 
       << "\t (" << getPercent(fdNetlistBuildTime, totalTime) << " %)" 
       << endl;
  cout << "   FDSOLVER           : " << fdSolverTime << " s" 
       << "\t (" << getPercent(fdSolverTime, totalTime) << " %)" 
       << endl;
  cout << "   CLUSTER SWAPPING   : " << clusterSwappingTime << " s" 
       << "\t (" << getPercent(clusterSwappingTime, totalTime) << " %)" 
       << endl;
  cout << "   CLUSTER FILL       : " << clusterFillTime << " s" 
       << "\t (" << getPercent(clusterFillTime, totalTime) << " %)" 
       << endl;
  cout << "   CLUSTER MIRRORING  : " << clusterMirroringTime << " s" 
       << "\t (" << getPercent(clusterMirroringTime, totalTime) << " %)" 
       << endl;
  cout << "   UNCLUSTERING       : " << unclusteringTime << " s" 
       << "\t (" << getPercent(unclusteringTime, totalTime) << " %)" 
       << endl;
  cout << "   GLOBAL PLACEMENT   : " << totalGPTime << " s" 
       << "\t (" << getPercent(totalGPTime, totalTime) << " %)" 
       << endl;
  cout << "   SHAPE SELECTION    : " << shapeSelectionTime << " s"
       << "\t (" << getPercent(shapeSelectionTime, totalTime) << " %)" 
       << endl;
  cout << "   LEGALIZATION       : " << legalizationTime << " s" 
       << "\t (" << getPercent(legalizationTime, totalTime) << " %)" 
       << endl;
  cout << "   DETAILED PLACEMENT : " << detailedPlacementTime << " s" 
       << "\t (" << getPercent(detailedPlacementTime, totalTime) << " %)" 
       << endl;
  cout << "   TOTAL              : " << totalTime << " s" << endl;
  cout << "***************************************************" << endl;

  double hpwlAfterConstPlacement;
  double hpwlAfterFDPlacement;
  double hpwlAfterClusterSwapping;
  double hpwlAfterClusterFill;
  double hpwlAfterClusterMirroring;
  double hpwlTotalInternal;
  double hpwlTotalGlobal;
  double hpwlAfterUnclustering;
  double hpwlAfterLegalization;
  double hpwlAfterDetailedPlacement;
  double localImp, globalImp;

  hpwlAfterConstPlacement = topEnv.EnvGetHPWLAfterConstructive();

  hpwlAfterFDPlacement = topEnv.EnvGetHPWLAfterFDPlacement();
  hpwlAfterClusterSwapping = topEnv.EnvGetHPWLAfterClusterSwapping();
  hpwlAfterClusterFill = topEnv.EnvGetHPWLAfterClusterFill();
  hpwlAfterClusterMirroring = topEnv.EnvGetHPWLAfterClusterMirroring();
  hpwlTotalInternal = topEnv.EnvGetHPWLTotalInternal();
  hpwlTotalGlobal = topEnv.EnvGetHPWLTotalGlobal();
  hpwlAfterUnclustering = topEnv.EnvGetHPWLAfterUnclustering();
  hpwlAfterLegalization = topEnv.EnvGetHPWLAfterLegalization();
  hpwlAfterDetailedPlacement = topEnv.EnvGetHPWLAfterDetailedPlacement();

  cout << "***************************************************" << endl;
  cout << "   QUALITY (HPWL) RECORD THROUGH STEPS  " << endl;
  cout << "   CONSTRUCTIVE PLACEMENT  : " << (ulong)hpwlAfterConstPlacement << endl;
  localImp = ((hpwlAfterConstPlacement - hpwlAfterFDPlacement) / hpwlAfterConstPlacement) * 100;
  globalImp = ((hpwlAfterConstPlacement - hpwlAfterFDPlacement) / hpwlAfterConstPlacement) * 100;
  cout << "   AFTER FD PLACEMENT      : " << (ulong)hpwlAfterFDPlacement 
       << "\tIMPL  : " << setprecision(3) << localImp 
       << "\tIMPG : " << setprecision(3) << globalImp 
       << endl;
  localImp = ((hpwlAfterFDPlacement - hpwlAfterClusterSwapping) / hpwlAfterFDPlacement) * 100;
  globalImp = ((hpwlAfterConstPlacement - hpwlAfterClusterSwapping) / hpwlAfterConstPlacement) * 100;
  cout << "   AFTER SWAPPING          : " << (ulong)hpwlAfterClusterSwapping 
       << "\tIMPL  : " << localImp 
       << "\tIMPG : " << globalImp 
       << endl;
  localImp = ((hpwlAfterClusterSwapping - hpwlAfterClusterFill) / hpwlAfterClusterSwapping) * 100;
  globalImp = ((hpwlAfterConstPlacement - hpwlAfterClusterFill) / hpwlAfterConstPlacement) * 100;
  cout << "   AFTER FILLING           : " << (ulong)hpwlAfterClusterFill 
       << "\tIMPL  : " << localImp 
       << "\tIMPG : " << globalImp 
       << endl;
  localImp = ((hpwlAfterClusterFill - hpwlAfterClusterMirroring) / hpwlAfterClusterFill) * 100;
  globalImp = ((hpwlAfterConstPlacement - hpwlAfterClusterMirroring) / hpwlAfterConstPlacement) * 100;
  cout << "   AFTER MIRRORING         : " << (ulong)hpwlAfterClusterMirroring 
       << "\tIMPL  : " << localImp 
       << "\tIMPG : " << globalImp 
       << endl;
  cout << "   TOTAL INTERNAL          : " << (ulong)hpwlTotalInternal << endl;
  cout << "   TOTAL GLOBAL            : " << (ulong)hpwlTotalGlobal << endl;
  hpwlAfterUnclustering = topEnv.EnvGetHPWLAfterUnclustering();
  hpwlAfterLegalization = topEnv.EnvGetHPWLAfterLegalization();
  hpwlAfterDetailedPlacement = topEnv.EnvGetHPWLAfterDetailedPlacement();
  cout << "   AFTER UNCLUSTERING      : " << (ulong)hpwlAfterUnclustering << endl;
  cout << "   AFTER LEGALIZATION      : " << (ulong)hpwlAfterLegalization << endl;
  cout << "   AFTER DETAILED PL.      : " << (ulong)hpwlAfterDetailedPlacement << endl;
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

  -noplacement: Use this option to indicate that no placement has
              to be done.

-global_placer: If this option is specified, an external global placer 
              is selected to perform global placement

  -use_wt_hpwl: A flow where the cost is computed using a weighted HPWL 
                function: Wtd.HPWL of net = (HPWL of net) * (netweight)

-use_fd_placer: If this option is specified, the force directed top level
              placer is selected to perform global placement

-cluster_strategy: If this option is specified, the appropriate strategy
              to perform clustering is selected. The options as of now 
              are : 
              a) bestchoice
              b) firstchoice
              c) netcluster
              d) tdcluster1
              e) tdcluster2
              f) largecluster
              g) nocluster

-cluster_global_placer: The global placer used for placing cells
              inside large clusters. Option values are:
              a) ntuplace
              b) mpl6
              c) fastplace

-cluster_legalizer: The legalizer used for legalizing cells 
              inside large clusters:
              a) ntuplace
              b) mpl6
              c) fastplace
              
-cluster_ratio: Clustering ratio to indicate to what extent the netlist 
              should be compressed. Lets say the clustering ratio is 0.4,
              the clustering strategy tries to compress the netlist to 
              retain 40% of the objects

-cluster_numrows: Indicates the number of rows that should be assigned 
              to each clusters. To build clusters as standard cells,
              use num_rows 1

-numclusters: Indicates the number of clusters that need to 
              be created for the design

 -hvariation: Percentage in double that indicates the maximum percentage
              of height variation of the cluster to be formed

 -addarea:    Percentage in double that indicates the minimum percentage
              of additional area that should be allocated to the area of 
              the cluster

 -numvarsteps: Number of steps in which the hvariation percentage must be 
              achieved

-imbalance_factor: The imbalance factor for the k-way partitioning engine

       -num_runs: The number of runs of khmetis to produce the k-way partitions

-recursive_biparititon: Use recursive bi-partitioning to produce k-paritions

-fill_clusters_post_top: Perform placement inside clusters after global 
              placement

-cluster_maxarea: Indicates the maximum area of clusters that should 
              be formed.

-cluster_maxwidth: Indicates the maximum width of the clusters that should
              be formed.

-cluster_bound_penalty: Indicates that instead of using strict bounds, use soft
              bounds to penalize the excess of width 

-cluster_placement: This option specifies how the cells should be placed
              inside the cluster. The options as of now are:
              a) boundaryonly
              b) placefull

-cluster_placer: This option specifies which placer must be used to place 
              cells inside the cluster. The default placer to be used 
              is NTUPlace

   -flat_placer: This option specifies which placer must be used to place 
              cells inside the cluster when doing flat placement. Useful
              for comparing flows.

      -flow_type : This option can be used to decide what flow should be 
              used to do the full placement. The options are:
              a) placeclustersfirst
              b) placeclustersfixports
              c) placeclustersfixcells
              d) placeclustersfixbound

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

   -swapping: This option specifies whether swapping should be used as 
            an optimization technique. Takes values "true" and "false"

  -mirroring: This option specifies whether mirroring should be used as 
            an optimization technique. Takes values "true" and "false"

-break_util_phase1: This option is a temporary option which is used 
              for experimentation

        -output : This option specifies the output file into which the 
              placement solution should be written out. The complete 
              file path must be given.

       -help: Prints out information about the tool
**********************************************************************/
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
    } else if (switchValue == "fdsolver") {
      topEnv.EnvSetSolverType(ENV_SOLVER_FORCE_DIRECTED);
    } else {
      rtv = false;
    }
  } else if (switchName == "noplacement") {
    rtv = true;
    topEnv.EnvSetGlobalPlacerType(ENV_NO_PLACEMENT);
  } else if (switchName == "global_placer") {
    rtv = true;
    if (switchValue == "ntuplace") {
      topEnv.EnvSetGlobalPlacerType(ENV_NTUPLACE_GP);
    } else if (switchValue == "fastplace") {
      topEnv.EnvSetGlobalPlacerType(ENV_FAST_PLACE_GP);
    } else if (switchValue == "mpl6") {
      topEnv.EnvSetGlobalPlacerType(ENV_MPL6_GP);
    } else if (switchValue == "mpl6noclust") {
      topEnv.EnvSetGlobalPlacerType(ENV_MPL6_NOCLUST_GP);
    } else {
      rtv = false;
      cout << "Error: Global placers supported are \"ntuplace\", \"fastplace\" and \"mpl6\""
	   << endl;
    }
  } else if (switchName == "use_wt_hpwl") {
    rtv = true;
    if (switchValue == "true") {
      topEnv.EnvSetUseWeightedHPWL(true);
    } else if (switchValue == "false") {
      topEnv.EnvSetUseWeightedHPWL(false);
    } else {
      cout << "Error: Option \"use_wt_hpwl\" can only take values \"true\" and \"false\"" << endl;
      rtv = false;
    }
  } else if (switchName == "use_fd_placer") {
    rtv = true;
    if (switchValue == "true") {
      topEnv.EnvSetUseFDPlacer(true);
    } else if (switchValue == "false") {
      topEnv.EnvSetUseFDPlacer(false);
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
    } else if (switchValue == "largecluster") {
      topEnv.EnvSetClusterType(ENV_LARGE_CLUSTERING);
    } else {
      rtv = false;
      cout << "Error: Valid options for cluster_strategy are " 
	   << "\"bestchoice\", \"firstchoice\", \"netcluster\" "
	   << "\"tdcluster1\", \"tdcluster2\"" << endl;
    }
  } else if (switchName == "cluster_global_placer") {
    rtv = true;
    if (switchValue == "ntuplace") {
      topEnv.EnvSetClusterGlobalPlacerType(ENV_NTUPLACE_GP);
    } else if (switchValue == "mpl6") {
      topEnv.EnvSetClusterGlobalPlacerType(ENV_MPL6_GP);
    } else if (switchValue == "fastplace") {
      topEnv.EnvSetClusterGlobalPlacerType(ENV_FAST_PLACE_GP);
    } else {
      rtv = false;
      cout << "Error: invalid option for cluster_global_placer" << endl;
    }
  } else if (switchName == "cluster_ratio") {
    rtv = true;
    double clusterRatio = strToDouble(switchValue);
    if (clusterRatio > 1) {
      cout << "Error: Invalid option. Must be a decimal value less than 0.7" << endl;
      rtv = false;
    } else {
      topEnv.EnvSetClusteringRatio(clusterRatio);
    }
  } else if (switchName == "cluster_numrows") {
    rtv = true;
    uint clusterNumRows = strToInt(switchValue);
    topEnv.EnvSetClusterNumRows(clusterNumRows);
  } else if (switchName == "numclusters") {
    rtv = true;
    uint numClusters = strToInt(switchValue);
    topEnv.EnvSetNumClusters(numClusters);
  } else if (switchName == "hvariation") {
    rtv = true;
    double hVariationPercentage = strToDouble(switchValue);
    topEnv.EnvSetClusterHVariationPercentage(hVariationPercentage);
  } else if (switchName == "addarea") {
    rtv = true;
    double addAreaPercentage = strToDouble(switchValue);
    topEnv.EnvSetClusterAddAreaPercentage(addAreaPercentage);
  } else if (switchName == "numvarsteps") {
    rtv = true;
    uint numHVariationSteps = strToDouble(switchValue);
    topEnv.EnvSetNumHVariationSteps(numHVariationSteps);
  } else if (switchName == "imbalance_factor") {
    rtv = true;
    uint imbalanceFactor = strToInt(switchValue);
    topEnv.EnvSetImbalanceFactor(imbalanceFactor);
  } else if (switchName == "num_runs") {
    rtv = true;
    uint numRuns = strToInt(switchValue);
    topEnv.EnvSetNumKHmetisRuns(numRuns);
  } else if (switchName == "recursive_bipartition") {
    rtv = true;
    if (switchValue == "true") {
      topEnv.EnvSetRecursiveBiPartitioning(true);
    } else {
      topEnv.EnvSetRecursiveBiPartitioning(false);
    }
  } else if (switchName == "fill_clusters_post_top") {
    rtv = true;
    if (switchValue == "true") {
      topEnv.EnvSetPlaceCellsInClusterPostTop(true);
    } else {
      topEnv.EnvSetPlaceCellsInClusterPostTop(false);
    }
  } else if (switchName == "cluster_maxarea") {
    rtv = true;
    //    if (switchValue == "") {
    //      rtv = false;
    //      cout << "Error: Invalid value for max area. Should be a percentage of chip area!" << endl;
    //    } else {
      double clusterMaxArea = strToDouble(switchValue);
      topEnv.EnvSetClusterMaxArea(clusterMaxArea);
      //    }
  } else if (switchName == "cluster_maxwidth") {
    rtv = true;
    //    if (switchValue == "") {
    //      rtv = false;
    //      cout << "Error: Invalid value for max width. Should be a percentage of chip width!" << endl;
    //    } else {
    double clusterMaxWidth = strToDouble(switchValue);
    topEnv.EnvSetClusterMaxWidth(clusterMaxWidth);
      //    }
  } else if (switchName == "cluster_bound_penalty") {
    rtv = true;
    if (!strIsNumber(switchName)) {
      rtv = false;
      cout << "Error: Invalid value for bound penalty. Should be an integer!" << endl;
    } else {
      uint clusterBoundPenalty = strToInt(switchValue);
      topEnv.EnvSetClusterBoundPenalty(clusterBoundPenalty);
    }
  } else if (switchName == "cluster_placement") {
    rtv = true;
    if (switchValue == "boundaryonly") {
      topEnv.EnvSetPlaceCellsInCluster(false);
    } else if (switchValue == "placefull") {
      topEnv.EnvSetPlaceCellsInCluster(true);
    } else {
      rtv = false;
      cout << "Error: Valid options for \"cluster_placement\" are \"boundaryonly\" or \"placefull\"" << endl;
    }
  } else if (switchName == "cluster_placer") {
    rtv = true;
    if (switchValue == "ntuplace") {
      topEnv.EnvSetClusterPlacerType(ENV_NTUPLACE_CP);
    } else if (switchValue == "mpl6") {
      topEnv.EnvSetClusterPlacerType(ENV_MPL6_CP);
    } else if (switchValue == "mpl6noclust") {
      topEnv.EnvSetClusterPlacerType(ENV_MPL6_NOCLUST_CP);
    } else {
      cout << "Error: Options for cluster_placer are \"ntuplace\" and \"mpl6\"" << endl;
      rtv = false;
    }
  } else if (switchName == "flat_placer") {
    rtv = true;
    if (switchValue == "ntuplace") {
      topEnv.EnvSetFlatPlacerType(ENV_NTUPLACE_FP);
    } else if (switchValue == "mpl6") {
      topEnv.EnvSetFlatPlacerType(ENV_MPL6_FP);
    } else if (switchValue == "mpl6noclust") {
      topEnv.EnvSetFlatPlacerType(ENV_MPL6_NOCLUST_FP);
    } else {
      rtv = false;
      cout << "Error: Options for flat_placer are \"ntuplace\" and \"mpl6\"" << endl;
    }
  } else if (switchName == "flow_type") {
    rtv = true;
    if (switchValue == "placeclustersfirst") {
      topEnv.EnvSetFlowType(ENV_PLACE_CLUSTERS_PRE_TOP);
    } else if (switchValue == "placeclustersfixports") {
      topEnv.EnvSetFlowType(ENV_PLACE_CLUSTERS_POST_TOP_WITH_PORTS);
    } else if (switchValue == "placeclustersfixcells") {
      topEnv.EnvSetFlowType(ENV_PLACE_CLUSTERS_POST_TOP_FIX_CELLS);
    } else if (switchValue == "placeclustersfixbound") {
      topEnv.EnvSetFlowType(ENV_PLACE_CLUSTERS_POST_TOP_BOUND);      
    }
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
  } else if (switchName == "swapping") {
    rtv = true;
    if (switchValue == "true") {
      topEnv.EnvSetClusterSwapping(true);
    } else if (switchValue == "false") {
      topEnv.EnvSetClusterSwapping(false);
    } else {
    }
  } else if (switchName == "mirroring") {
    rtv = true;
    if (switchValue == "true") {
      topEnv.EnvSetClusterMirroring(true);
    } else if (switchValue == "false") {
      topEnv.EnvSetClusterMirroring(false);
    }
  } else if (switchName == "break_util_phase1") {
    rtv = true;
    double value = strToDouble(switchValue);
    topEnv.EnvSetMaxUtilPhaseI(value);
  } else if (switchName == "output") {
    rtv = true;
    topEnv.EnvSetOutputFileName(switchValue);
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
  EnvGlobalPlacerType globalPlacerType;

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
  //  ProfilerStart("GlobalPlacement");
  myDesign.DesignDoGlobalPlacement();
  //  ProfilerStop();
    
  globalPlacerType = DesignEnv.EnvGetGlobalPlacerType();
  if (globalPlacerType != ENV_NO_PLACEMENT) {
  /****************************************************
   *  DO LEGALIZATION                                 *
   ****************************************************/
    myDesign.DesignDoLegalization();
    
  /****************************************************
   *  DO DETAILED PLACEMENT                           *
   ****************************************************/
    myDesign.DesignDoDetailedPlacement();
  }

  /****************************************************
   *  PLOT THE FINAL PLACEMENT                        *
   ****************************************************/
  string plotFileName;
  plotFileName = "final.plt";
  myDesign.DesignPlotData("Title", plotFileName);

  /****************************************************
   *  WRITE THE FINAL PLACEMENT                       *
   ****************************************************/
  string outputFileName = topEnv.EnvGetOutputFileName();
  DesignWriteOutputPlacement(myDesign, outputFileName); 
  
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

