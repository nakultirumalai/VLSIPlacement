# ifndef LISTS_H
# define LISTS_H
# include <common.h>

class dllist;

class listNode {
 public:
  listNode *next;
  listNode *prev;
  dllist *parent;
  uint data;

  /* Constructors */
  listNode();
  listNode(uint);
  listNode(uint, dllist *);

  /* Destructor */
  ~listNode();

  /* Set functions */
  void SetData(uint);
  void SetNext(listNode *);
  void SetPrev(listNode *);
  void SetParent(dllist *);

  /* Get functions */
  dllist* GetParent(void);
  listNode *GetNext(void);
  listNode *GetPrev(void);
  uint GetData(void);
};

class dllist {
 public:
  listNode *start;
  listNode *end;
  uint listSize;
  
  dllist();
  ~dllist();
  void addNode(listNode *);
  listNode *addNode(uint);
  void deleteNode(listNode *);
  listNode *popNode(void);
};

# endif
