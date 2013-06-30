class TimingPoint;

class PathElem {
  TimingPoint* node;
  int edge;
  PathElem* nextElem;

 public:
 PathElem(TimingPoint*& tp, int trans) : node(tp), edge(trans) {
    nextElem = NULL;
  }

  TimingPoint* getNode() {
    return node;
  }
  int getNodeTrans() {
    return edge;
  }

  void setNext(PathElem*& next) {
    nextElem = next;
  }
  PathElem* getNext() {
    return nextElem;
  }

};

class Path {
  PathElem* startPoint;
  PathElem* endPoint;

 public:
  Path() {
    startPoint = endPoint = NULL;
  }

  void insert(PathElem*& elem) {
    if(startPoint==NULL)
      startPoint = endPoint = elem;
    else {
      startPoint->setNext(elem);
      startPoint = elem;
    }
  }

  PathElem* getStartPoint() {
    return startPoint;
  }
  PathElem* getEndPoint() {
    return endPoint;
  }
};

