/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef MOVEQUEUE_H
#define MOVEQUEUE_H

#include "move.h"


class MoveQ;

/**************************************
  Class: MoveQNode

      This class is used only by the MoveQ class.  A MoveQNode is one
  node in the queue represented by a MoveQ.
**************************************/

class MoveQNode
{
friend class MoveQ;
public:
  MoveQNode(Move *m);
  ~MoveQNode();

private:
  MoveQNode(const MoveQNode&);
  MoveQNode& operator=(const MoveQNode&);

  Move *move;
  MoveQNode *next;
};


/**************************************
  Class: MoveQ

      This class represents a queue of Moves (with the added capability
  of adding members to the front of the queue).
***************************************/

class MoveQ
{
public:
  MoveQ();
  ~MoveQ();
  void add(Move *);
  void addToFront(Move *);
  void clear(int del = 1);
  Move* get();
  Move* peek() const;
  void doToEach(void (*f)(Move *));
  int deleteIf(int (*f)(const Move *), int del = 1);
  void delet(Move *m, int del = 1);
  int isEmpty() const { return head == 0; };
  
private:
  MoveQ(const MoveQ&);
  MoveQ& operator=(const MoveQ);

  void del_head_node(MoveQNode *node, int del = 1);
  void del_node(MoveQNode *prev, MoveQNode *node, int del = 1);
  static int true_for_all(const Move *);

  MoveQNode *head;
  MoveQNode *tail;
};


inline MoveQNode::MoveQNode(Move *m)
{
  move = m;
  next = NULL;
}

inline MoveQNode::~MoveQNode()
{
}

inline MoveQ::MoveQ()
{
  head = tail = 0;
}

inline int MoveQ::true_for_all(const Move *)
{
  return 1;
}

inline void MoveQ::clear(int del)
{
  deleteIf(MoveQ::true_for_all, del);
}


#endif
  


  
