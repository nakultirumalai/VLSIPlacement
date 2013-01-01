# ifndef SORT_H
# define SORT_H

template<typename vectorType>
typedef bool (*cmp_func1)(vector<vectorType>, unsigned int, unsigned int);

typedef bool (*cmp_func2)(vector<unsigned int>, unsigned int, unsigned int);
typedef bool (*cmp_func3)(vector<double>, unsigned int, unsigned int);

template<class T>
extern void MergeSortVector(vector<T>&, int, int, cmp_func1);
template<class T>
extern void MergeSortVector(vector<T>&, int, int, cmp_func2);
template<class T>
extern void MergeSortVector(vector<T>&, int, int, cmp_func3);

# endif

