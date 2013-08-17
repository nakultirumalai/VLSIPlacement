#ifndef _HMETIS_LIB_H_
#define _HMETIS_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
  Note that HMETIS_PartKway() should never be used to compute a
  bisection (i.e., 2-way partitioning) as it produces worse results
  than HMETIS_PartRecursive(). Furthermore, the quality of the
  partitionings produced by HMETIS_PartKway() for small values of k
  will be worse, in general, than the corresponding partitionings
  computed by HMETIS_PartRecursive(). However, HMETIS_PartKway() is
  particularly useful for computing k-way partitionings for
  relatively large values of k, as it often produces better
  partitionings and it can also enforce tighter balancing
  constraints.
*/

void HMETIS_PartRecursive(int nvtxs,      /* [in] nb vertices */
			  int nhedges,    /* [in] nb hyperedges */
			  int* vwgts,     /* [in] array vertex weights */
			  int* eptr,      /* [in] array of indirection on eind (of size nhedges+1) */
			  int* eind,      /* [in] array with all consecutive hyperedges (as vertex set) */
			  int* hewgts,    /* [in] array hyperedge weights */
			  int nparts,     /* [in] nb of desired partitions */
			  int ubfactor,   /* [in] unbalanced factor */
			  int* options,   /* [in] array of 9 integers (options[0]=0 for default options) */
			  int* part,      /* [out] array of computed partitions (of size nvtxs) */
			  int* edgecut);  /* [out] nb hyperedges cut */

void HMETIS_PartKway(int nvtxs,      /* [in] nb vertices */
		     int nhedges,    /* [in] nb hyperedges */
		     int* vwgts,     /* [in] array vertex weights */
		     int* eptr,      /* [in] array of indirection on eind (of size nhedges+1) */
		     int* eind,      /* [in] array with all consecutive hyperedges (as vertex set) */
		     int* hewgts,    /* [in] array hyperedge weights */
		     int nparts,     /* [in] nb of desired partitions */
		     int ubfactor,   /* [in] unbalanced factor */
		     int* options,   /* [in] array of 9 integers (options[0]=0 for default options) */
		     int* part,      /* [out] array of computed partitions (of size nvtxs) */
		     int* edgecut);  /* [out] nb hyperedges cut */
#ifdef __cplusplus
}
#endif

#endif
