MODULE = Design
SRCFILES:=  DesignMain DesignUtils DesignRead DesignGraph  DesignAnalysis DesignWrite \
	    DesignProperties DesignSolveFastMosek7 DesignSpread  DesignSpreadILR DesignDebug \
	    DesignSolveFastConjGrad DesignPlace DesignCluster DesignClusterStrategy \
	    DesignClusterBestChoice DesignClusterNetCluster DesignClusterKWay DesignClusterLarge \
	    DesignWriteCluster DesignSolveForceDirected

HFILES:= Design DesignIter PriorityQueue HyperGraph Cell Pin Net Env
