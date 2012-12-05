# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>
# define MAX_ARGS 5

HyperGraph& convertDesignToGraph(Design& thisDesign)
{
  HyperGraph *myGraph = new HyperGraph();
  
  DesignCreateGraph(thisDesign, (*myGraph));

  return (*myGraph);
}

int main(int argc, char *argv[])
{
  string designName, designPath;
  string switchName;
  bool performAnalysis;
  int i;

  designName = "";
  designPath = "";

  if (argc > 1 && argc < MAX_ARGS) {
    i = 1;
    performAnalysis = false;
    while (i <= argc) {
      if (argv[i][0] == '-') {
	/* Switch to options */
	switchName = argv[i] + 1;
	if (switchName == "analyse") {
	  performAnalysis = true;
	}
      } else {
	if (designPath == "") {
	  designPath = argv[i];
	} else if (designName == "") {
	  designName = argv[i]; 
	}
      }
    }
    Design myDesign;
    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
    myDesign.DesignReadDesign(designPath, designName);
    cout << "Netlist read successfully" << endl;

    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
    if (performAnalysis == true) {
      DesignCollectStats(myDesign);
      DesignWriteStats(myDesign);
    }
    //    HyperGraph &myGraph = convertDesignToGraph(myDesign);
    //myGraph.testClustering();
    //    cout << endl << "Graph construction complete" << endl;

    //    cout << "Added " << myGraph.HyperGraphGetNumNodes() << " nodes" << endl;
    //    cout << "Added " << myGraph.HyperGraphGetNumEdges() << " edges" << endl;
    
    //    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
  }

  return 0;
}

