# ifndef COMMON_ITER_H
# define COMMON_ITER_H

/* Macros for iteration over pins and cells of the net */

# define VECTOR_FOR_ALL_ELEMS(VectorObj, Type, Obj) \
  {						    \
  uint __size = VectorObj.size();		    \
  for (int i = 0;  i < __size; i++) {		    \
  Obj = (Type)VectorObj[i];

# define VECTOR_FOR_ALL_ELEMS_DOUBLE(VectorObj1, Type1, Obj1, VectorObj2, Type2, Obj2) \
  { \
  _ASSERT("Sizes of the vectors passed to VECTOR_FOR_ALL_ELEMS_DOUBLE are unequal", \
	  VectorObj1.size() != VectorObj2.size()); \
  for (int i = 0;  i < VectorObj1.size(); i++) { \
  Obj1 = (Type1)VectorObj1[i]; \
  Obj2 = (Type2)VectorObj2[i];

# define MAP_FOR_ALL_ELEMS(MapObj, KeyType, ValType, Key, Value)	\
  {									\
  for(map<KeyType,ValType>::iterator iter = MapObj.begin(); iter != MapObj.end(); ++iter) { \
  Key = (KeyType)iter->first;						\
  Value = iter->second;

# define MAP_FOR_ALL_KEYS(MapObj, KeyType, ValType, Key)	\
  {									\
  for(map<KeyType,ValType>::iterator iter = MapObj.begin(); iter != MapObj.end(); ++iter) { \
  Key = (KeyType)iter->first;						\
  
# define END_FOR }}
  
# endif

