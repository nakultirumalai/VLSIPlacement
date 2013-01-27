# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>
# include <Flags.h>

# define MAX_ARGS 7

HyperGraph& convertDesignToGraph(Design& thisDesign)
{
  HyperGraph *myGraph = new HyperGraph();
  
  DesignCreateGraph(thisDesign, (*myGraph));
  thisDesign.DesignSetGraph(*myGraph);

  return (*myGraph);
}

int main(int argc, char *argv[])
{
  string designName, designPath;
  string switchName;
  int i;

  designName = "";
  designPath = "";

  if (argc > 1 && argc < MAX_ARGS) {
    i = 1;
    while (i < argc) {
      if (argv[i][0] == '-') {
	/* Switch to options */
	switchName = argv[i] + 1;
	if (switchName == "analyse") {
	  performAnalysis = true;
	} else if (switchName == "trace_depth") {
	  string traceDepthStr = argv[i+1];
	  if (!strIsNumber(traceDepthStr)) {
	    _ERROR("Trace depth specified is not a number. Assuming 1");
	    traceDepth = 1;
	  } else {
	    traceDepth = atoi(traceDepthStr.data());
	    i++;
	  }
	}
      } else {
	if (designPath == "") {
	  designPath = argv[i];
	} else if (designName == "") {
	  designName = argv[i]; 
	}
      }
      i++;
    }

    Design myDesign(designPath, designName);

    HyperGraph &myGraph = convertDesignToGraph(myDesign);
    FlagsInit();

    //DesignWriteNodes(myDesign, "");

    myDesign.DesignClusterCells(myGraph, DEFAULT_CLUSTER);
    /* Second param is clustering type. Can take on the following values:
       FCC_CLUSTER, NET_CLUSTER, ESC_CLUSTER */

    //DesignWriteNodes(myDesign, "cluster.out");
    
    if (performAnalysis == true) {
      DesignCollectStats(myDesign);
      DesignWriteStats(myDesign);
    }

    //myGraph.testClustering();
    //    cout << endl << "Graph construction complete" << endl;

    //    cout << "Added " << myGraph.HyperGraphGetNumNodes() << " nodes" << endl;
    //    cout << "Added " << myGraph.HyperGraphGetNumEdges() << " edges" << endl;
    
    //    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
  }

  return 0;
}

