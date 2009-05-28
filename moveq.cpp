/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include "moveq.h"


MoveQ::~MoveQ()
{
  MoveQNode *s;

  while (head) {
    s = head->next;
    delete head->move;
    delete head;
    head = s;
  }
}
    
void MoveQ::add(Move *move)
{
  if (!tail) {
    head = new MoveQNode(move);
    tail = head;
  } else {
    tail->next = new MoveQNode(move);
    tail = tail->next;
  }
}

void MoveQ::addToFront(Move *move)
{
  MoveQNode *s = new MoveQNode(move);
  s->next = head;
  head = s;
}

void MoveQ::doToEach(void (*f)(Move *))
{
  MoveQNode *s;
  s = head;
  while (s) {
    f(s->move);
    s = s->next;
  }
}

int MoveQ::deleteIf(int (*f)(const Move *), int del)
{
  MoveQNode *s, *t;
  int count;

  count = 0;
  if (!isEmpty()) {
    while (head && f(head->move)) {
      count++;
      del_head_node(head, del);
    }
    s = head;
    if (s) {
      t = s->next;
      while (t && f(t->move)) {
	count++;
	del_node(s, t, del);
	t = s->next;
      }
    }
    if (head == 0)
      tail = 0;
  }
  return count;
}

void MoveQ::del_head_node(MoveQNode *node, int del)
{
  head = node->next;
  if (del)
    delete node->move;
  delete node;
}

void MoveQ::del_node(MoveQNode *prev, MoveQNode *node, int del)
{
  prev->next = node->next;
  if (del)
    delete node->move;
  delete node;
}

Move *MoveQ::get()
{
  MoveQNode *t;

  if(!head) {
    return NULL;
  } else {
    t = head;
    Move *m = t->move;
    head = t->next;
    delete t;
    return m;
  }
}

Move *MoveQ::peek() const
{
  if (!head)
    return NULL;
  else
    return head->move;
}


