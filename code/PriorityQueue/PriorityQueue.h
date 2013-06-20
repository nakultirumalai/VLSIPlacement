# ifndef PRIORITY_QUEUE_H
# define PRIORITY_QUEUE_H

# include <common.h>

class Cell;
template<class T = Cell *> class objPairScore;
class objPairScoreCompare;
template <class T = objPairScore<Cell *>, class C = objPairScoreCompare> class PriorityQueue {
private:
  uint numItems;
  vector<T> container;
  C cmp;
public:
  PriorityQueue();
  ~PriorityQueue();
  /* Get functions */
  bool empty() const;
  uint GetSize() const;
  vector<T> & GetItems(void); /* For iteration */
  T &pop(void); 
  void push(T &);
  void pushAndSort(T &);
  void sortQueue(void);
};

template<class T, class C> 
PriorityQueue<T, C>::PriorityQueue()
{
  this->numItems = 0;
}

template<class T, class C> 
PriorityQueue<T, C>::~PriorityQueue()
{

}

template<class T, class C>
bool
PriorityQueue<T, C>::empty() const
{
  return (this->numItems == 0);
}

template<class T, class C> 
uint
PriorityQueue<T, C>::GetSize(void) const
{
  return (container.size());
}

template<class T, class C>
vector<T> &
PriorityQueue<T, C>::GetItems(void)
{
  return container;
}

template<class T, class C>
T &
PriorityQueue<T, C>::pop() 
{
  T &rtv = container[numItems - 1];
  container.pop_back();
  this->numItems--;

  return (rtv);
}

template<class T, class C>
void 
PriorityQueue<T, C>::push(T &item)
{
  container.push_back(item);
  this->numItems++;
}

template<class T, class C>
void PriorityQueue<T, C>::pushAndSort(T &item)
{
  container.push_back(item);
  std::sort(container.begin(), container.end(), cmp);
  this->numItems++;
}

template<class T, class C>
void PriorityQueue<T, C>::sortQueue(void)
{
  std::sort(container.begin(), container.end(), cmp);
}

# endif
