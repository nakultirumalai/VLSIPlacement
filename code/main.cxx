# include <common.h>
# include <Design.h>
# include <HyperGraph.h>
# include <Stat.h>
# include <Flags.h>
# include <Legalize.h>
# include <lemon/list_graph.h>

using namespace lemon;

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
    FlagsInit();

    Design myDesign(designPath, designName);
    HyperGraph &myGraph = convertDesignToGraph(myDesign);
    
    //myDesign.DesignSolveForSeqCells();
    
    myDesign.DesignSolveForAllCellsIter();
    //    myDesign.DesignSolveForAllCells(ALL_QO_WL);
    //    LegalizeDesign(myDesign);
    
    DesignWriteBookShelfOutput(myDesign); 

    /* Plot the stuff in the design */
    //    string plotFileName;
    //    plotFileName = "Itr.plt";
    //    myDesign.DesignPlotData("Title", plotFileName);

    if (performAnalysis == true) {
      DesignCollectStats(myDesign);
      DesignWriteStats(myDesign);
    }
  }
  
  return 0;
}

