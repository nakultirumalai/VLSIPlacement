# include <common.h>
# include <Sort.h>

template<class Type> 
bool default_cmp_func(vector<Type> &thisVector, unsigned int idx1, unsigned int idx2)
{
  bool result;
  
  result = false;
  if (thisVector[idx1] < thisVector[idx2]) {
    result = true;
  }
}

template<class Type, class funcPtr> void
DoMerge(vector<Type> &thisVector, int Left, int Mid, int Right,	funcPtrType this_func) 
{
  vector<Type> tempVector;
  int numElements = (Right - Left + 1);
  int i,j;
  
  for (i = Left; (i < numElements && Mid <= Right); i++) {
    if (this_func == NIL(funcPtrType)) {
      if (default_cmp_func(thisVector, i, Mid)) {
	tempVector.push_back(thisVector[i]);
      } else {
	tempVector.push_back(thisVector[Mid]);
	Mid++;
      }
    } else {
      if (this_func(thisVector, i, Mid)) {
	tempVector.push_back(thisVector[i]);
      } else {
	tempVector.push_back(thisVector[Mid]);
	Mid++;
      }
    }
  }
  
  /* Remaining elements on left to tempVector */
  for (i; i < numElements; i++) {
    tempVector.push_back(thisVector[i]);
  }

  /* Remaining elements on right to tempVector */
  for (i=Mid; i <= Right; i++) {
    tempVector.push_back(thisVector[i]);
  }

  /* Copy elements in tempVector to thisVector */
  for (i=Left, j=0; i <= Right; i++) {
    thisVector[i] = tempVector[j++];
  }
}

template<class Type, class cmp_func> void 
MergeSortVector(Type Left, Type Right, cmp_func this_func)
{
  if (Left < Right) { 
    int Mid = ((Left + Right) / 2);
    MergeSortVector<Type, cmp_func>(thisVector, Left, Mid, this_func);
    MergeSortVector<Type, cmp_func>(thisVector, Mid + 1, Right, this_func);
    DoMerge<Type, cmp_func>(thisVector, Left, Mid, Right);
  }
}
