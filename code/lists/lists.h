# ifndef LISTS_H
# define LISTS_H

class Node {
 public:
  Node *next;
  Node *prev;
  uint data;
};

class dllist {
 public:
  Node *start;
  Node *end;
  uint listSize;
  void addNode(Node *);
  Node *addNode(uint);
  void deleteNode(Node *);
};

# endif
