# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>

HyperGraph& convertDesignToGraph(Design& thisDesign)
{
  HyperGraph *myGraph = new HyperGraph();
  
  DesignCreateGraph(thisDesign, (*myGraph));

  return (*myGraph);
}

int main(int argc, char *argv[])
{
  string designName, designPath;
  
  if (argc > 1 && argc < 4) {
    designPath = string(argv[1]);
    designName = string(argv[2]);

    Design myDesign;
    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
    myDesign.DesignReadDesign(designPath, designName);
    cout << "Netlist read successfully" << endl;

    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;

    HyperGraph &myGraph = convertDesignToGraph(myDesign);
    myGraph.testClustering();
    cout << endl << "Graph construction complete" << endl;

    cout << "Added " << myGraph.HyperGraphGetNumNodes() << " nodes" << endl;
    cout << "Added " << myGraph.HyperGraphGetNumEdges() << " edges" << endl;
    
    cout << "Memory used: " << getMemUsage() << MEM_USAGE_UNIT << endl;
  }

  return 0;
}

