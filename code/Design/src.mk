MODULE = Design
SRCFILES:=  DesignMain DesignUtils DesignRead DesignGraph  DesignAnalysis DesignWrite \
	    DesignProperties DesignSpread DesignSpreadILR DesignDebug \
	    DesignSolveFastConjGrad DesignPlace DesignCluster DesignClusterStrategy \
	    DesignClusterBestChoice DesignClusterNetCluster DesignClusterKWay DesignClusterLarge \
	    DesignWriteCluster DesignSolveForceDirected DesignDump

HFILES:= Design DesignIter PriorityQueue HyperGraph Cell Pin Net Env
