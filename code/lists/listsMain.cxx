# include <lists.h>

listNode::listNode()
{
  this->next = NIL(listNode *);
  this->prev = NIL(listNode *);
  this->parent = NIL(dllist *);
  this->data = 0;
}

listNode::listNode(uint data)
{
  this->next = NIL(listNode *);
  this->prev = NIL(listNode *);
  this->parent = NIL(dllist *);
  this->data = data;
}

listNode::listNode(uint data, dllist *parent)
{
  this->next = NIL(listNode *);
  this->prev = NIL(listNode *);
  this->parent = parent;
  this->data = data;
}

void
listNode::SetData(uint data)
{
  this->data = data;
}

void
listNode::SetNext(listNode *nextNode)
{
  this->next = nextNode;
}

void
listNode::SetPrev(listNode *prevNode)
{
  this->prev = prevNode;
}

void
listNode::SetParent(dllist *parent)
{
  this->parent = parent;
}

dllist *
listNode::GetParent(void)
{
  return (this->parent);
}

listNode *
listNode::GetNext(void)
{
  return (this->next);
}

listNode *
listNode::GetPrev(void)
{
  return (this->prev);
}

uint 
listNode::GetData(void)
{
  return (this->data);
}

dllist::dllist()
{
  listSize = 0;
  start = NIL(listNode *);
  end = NIL(listNode *);
}

dllist::~dllist()
{

}

void
dllist::addNode(listNode *thisNode)
{
  if (listSize == 0) {
    this->end = thisNode;
    this->start = thisNode;
  } else {
    (*(*this).end).SetNext(thisNode);
    this->end = thisNode;
  }
  (*thisNode).SetParent(this);
  this->listSize++;
}

listNode *
dllist::addNode(uint data)
{
  listNode *newNode = new listNode(data);
  
  addNode(newNode);

  return newNode;
}

listNode *
dllist::popNode(void)
{
  listNode *topNode = this->start;
  
  deleteNode(topNode);
  
  return (topNode);
}

void
dllist::deleteNode(listNode *thisNode)
{
  if ((*thisNode).GetParent() != this) {
    _ASSERT_TRUE("Error: Parent of node to be deleted is not equal to the list pointer!");
  }

  if (listSize == 1) {
    this->start = NIL(listNode *);
    this->end = NIL(listNode *);
  } else {
    listNode *nextNode = (*thisNode).GetNext();
    listNode *prevNode = (*thisNode).GetPrev();
    
    if (prevNode != NIL(listNode *)) {
      /* this is the first node being deleted */
      (*prevNode).SetNext(nextNode);
    }
    if (nextNode != NIL(listNode *)) {
      (*nextNode).SetPrev(prevNode);
    }
  }

  (*thisNode).SetParent(NIL(dllist*));

  listSize--;
}
