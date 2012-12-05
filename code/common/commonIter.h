# ifndef COMMON_ITER_H
# define COMMON_ITER_H

/* Macros for iteration over pins and cells of the net */

# define VECTOR_FOR_ALL_ELEMS(VectorObj, Type, Obj) \
  { \
  for (int i = 0;  i < VectorObj.size(); i++) { \
  Obj = (Type)VectorObj[i];

# define MAP_FOR_ALL_ELEMS(MapObj, KeyType, ValType, Key, Value)	\
  { \
  for(map<KeyType,ValType>::iterator iter = MapObj.begin(); iter != MapObj.end(); ++iter) { \
    Key = (KeyType)iter->first;	\
    Value = (ValType)iter->second;

# define MAP_FOR_ALL_KEYS(MapObj, KeyType, ValType, Key)	\
  { \
  ValType mapValue; \
  for(map<KeyType,ValType>::iterator iter = MapObj.begin(); iter != MapObj.end(); ++iter) { \
    Key = (KeyType)iter->first;	\
    mapValue = (ValType)iter->second;

# define END_FOR }}
  
# endif

