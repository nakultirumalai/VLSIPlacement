DoMerge(vector<vector<unsigned int > > &thisVector, int Left, int Mid, int Right) 
{
  vector<T> tempVector;
  int numElements = Right - Left + 1;
  int i,j;
  
  for (i = left; (i < numElements && Mid <= Right); i++) {
    if (thisVector[i] < thisVector[Mid]) {
      tempVector.push_back(thisVector[i]);
    } else {
      tempVector.push_back(thisVector[Mid]);
      Mid++;
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

void 
MergeSortVector(vector<unsigned int > > &thisVector, 
		int Left, int Right)
{
  if (Left < Right) { 
    int Mid = ((Left + Right) / 2);
    MergeSortVector(thisVector, Left, Mid);
    MergeSortVector(thisVector, Mid + 1, Right);
    DoMerge(thisVector, Left, Mid, Right);
  }
}

