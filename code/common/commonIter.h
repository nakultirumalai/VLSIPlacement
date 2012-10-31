# ifndef COMMON_ITER_H
# define COMMON_ITER_H

/* Macros for iteration over pins and cells of the net */

# define VECTOR_FOR_ALL_ELEMS(VectorObj, Type, Obj) \
  for (int i = 0;  i < VectorObj.size(); i++) { \
    Obj = (Type)VectorObj[i];

# define END_FOR }
  
# endif

