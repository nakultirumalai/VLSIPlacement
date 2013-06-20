# include <Cell.h>
# include <Pin.h>

# ifndef PATH_H
# define PATH_H

class Path {
 private:
  vector<Cell *> listOfCells;
  vector<Pin *> listOfPins;
  uint pathLength;
  double slack;
 public:
  /* constructor and destructor */
  Path();
  ~Path();
  
  /* Add functions */
  void PathAddCellPinPair(Cell*, Pin*);
  void PathAddCellPinPair(Cell*, string);
  void PathSetSlack(double);

  /* Get functions */
  pair<Cell *, Pin *> PathGetFirstCellPin();
  pair<Cell *, Pin *> PathGetLastCellPin();
  vector<Cell *> &PathGetCells();
  uint PathGetLength();
  double PathGetSlack();
};
# endif
