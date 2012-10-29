# ifndef COMMON_ITER_H
# define COMMON_ITER_H

/* Macros for iteration over pins and cells of the net */

# define VECTOR_FOR_ALL_CELLS(VectorObj, Type, Obj) \
  vector<Type>::iterator vectorIter;					\
  for (vectorIter = VectorObj.begin(); vectorIter != VectorObj.end(); vectorIter++) { \
  Obj = (Type)(*vectorIter);

# define END_FOR }
  
# endif

