/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <assert.h>
#include "move.h"


 Move::Move ()
     : _is_jump(0), _subseq_moves(NULL), _current_move(NULL)
{
}

Move::Move (const Coord& orig, const Coord& dir, int jump)
    : _is_jump(jump), _origin(orig), _initial_dir(dir), _subseq_moves(NULL),
      _current_move(NULL)
{
}

Move::Move (const Move& m)
    : _is_jump(m._is_jump), _origin(m._origin), _initial_dir(m._initial_dir),
      _subseq_moves(NULL), _current_move(m._current_move)
{
  if (m._subseq_moves)
  {
      append_frag_list(m);
  }
}

Move::~Move ()
{
  if (_subseq_moves)
  {
      delete_frag_list();
  }
}




short
Move::append (const Move& m)
{
    assert(_is_jump == 1);
    
    if (this == &m)
    {
	return 1;
    }
    
    if (_initial_dir == Coord(0, 0))
    {
	// Appending Move m to a null move, just copy m
	*this = m;
	return 0;
    }
    else if (m._initial_dir == Coord(0, 0))
    {
	// Appending null Move m to a move. don\'t do anything
	return 0;
    }
  
    // appending two real moves
    // cases:
    //		1 - we have only 1 move, they have only 1 move
    //		2 - we have only 1 move, they have more than 1 move
    //		3 - we have more than one move, they have only 1 move
    //		4 - we have more than one move, they have more than 1 move
    if (!_subseq_moves)
    {
	// we have only 1 movefrag
	if (!m._subseq_moves)
	{
	    // they have only 1 movefrag
	    // case 1
	    // copy their one movefrag
	    _subseq_moves = new MoveFrag(m._initial_dir);
	}
	else
	{
	    // they have more than 1 move
	    // case 2
	    // copy their first movefrag
	    _subseq_moves = new MoveFrag(m._initial_dir);
	    // copy the rest of their movefrags
	    append_frag_list(m);
	}
    }
    else
    {
	// we have more than one movefrag
	if (!m._subseq_moves)
	{
	    // they have only 1 movefrag
	    // case 3
	    MoveFrag *s = _subseq_moves;
	    // find end of our frag list
	    while (s->next)
	    {
		s = s->next;
	    }
	    // copy their first movefrag
	    s->next = new MoveFrag(m._initial_dir);
	}
	else
	{
	    // they have more than one movefrag
	    // case 4
	    MoveFrag *s = _subseq_moves;
	    // find end of our frag list
	    while (s->next)
	    {
		s = s->next;
	    }
	    // copy their first movefrag
	    s->next = new MoveFrag(m._initial_dir);
	    // copy the rest of their movefrags
	    append_frag_list(m);
	}
    }
    return 0;
}



Move&
Move::operator= (const Move& m)
{
  if (this == &m)
  {
      return *this;
  }

  _is_jump = m._is_jump;
  _origin = m._origin;
  _initial_dir = m._initial_dir;
  _current_move = NULL;

  delete_frag_list();

  if (m._subseq_moves)
  {
      delete_frag_list();
      append_frag_list(m);
      _current_move = NULL;
  }

  return *this;
}


void
Move::append_frag_list (const Move& m)
{
    if (m._subseq_moves)
    {
	MoveFrag *s, *t;
	s = m._subseq_moves;
	if (!_subseq_moves)
	{
	    _subseq_moves = t = new MoveFrag(*s);
	    s = s->next;
	}
	else
	{
	    t = _subseq_moves;
	    while (t->next)
	    {
		t = t->next;
	    }
	}
	while (s)
	{
	    t->next = new MoveFrag(*s);
	    t = t->next;
	    s = s->next;
	}
    }
}


void
Move::delete_frag_list ()
{
    if (_subseq_moves)
    {
	MoveFrag *s, *t;
	s = _subseq_moves->next;
	delete _subseq_moves;
	while (s)
	{
	    t = s->next;
	    delete s;
	    s = t;
	}
	_subseq_moves = NULL;
    }
}  


void
Move::addDir (const Coord& d)
{
    assert(d != Coord(0, 0));
    if (_initial_dir == Coord(0, 0))
    {
	assert(_subseq_moves == NULL);
	_initial_dir = d;
    }
    else
    {
	// now we know it's a jump
	_is_jump = 1;
	if (!_subseq_moves)
	{
	    _subseq_moves = new MoveFrag(d);
	}
	else
	{
	    MoveFrag *s;
	    s = _subseq_moves;
	    while (s->next)
	    {
		s = s->next;
	    }
	    s->next = new MoveFrag(d);
	}
    }
}


int
Move::setJump (int j)
{
    int temp;
    
    temp = _is_jump;
    _is_jump = j;
    return temp;
}

void
Move::setOrigin (const Coord& o)
{
    _origin = o;
}


void
Move::clear ()
{
    if (_subseq_moves)
    {
	delete_frag_list();
    }
    _is_jump = 0;
    _origin = _initial_dir = Coord(0, 0);
    _subseq_moves = _current_move = NULL;
}


const
Coord& Move::getOrigin () const
{
    return _origin;
}

Coord
Move::getDest () const
{
    Coord dest;
    MoveFrag *s;
    
    dest = _origin + _initial_dir;
    if (_is_jump)
    {
	dest += _initial_dir;
    }
    
    s = _subseq_moves;
    while (s)
    {
	dest += s->dir + s->dir;
	s = s->next;
    }
    return dest;
}


int
Move::getDir (Coord *d) const
{
    assert(_initial_dir != Coord(0, 0));
    if (_initial_dir == Coord(0, 0))
    {
	return 0;
    }
    Move *not_const_this = (Move *) this;
    not_const_this->_current_move = _subseq_moves;
    *d = _initial_dir;
    return 1;
}


int
Move::getNextDir (Coord *d) const
{
    if (_current_move == NULL)
    {
	return 0;
    }
    Move *not_const_this = (Move *) this;
    *d = _current_move->dir;
    not_const_this->_current_move = _current_move->next;
    return 1;
}
  

int
Move::isMulti () const
{
    return (_subseq_moves == NULL);
}


int
Move::isJump () const
{
    return _is_jump;
}


ostream&
operator<< (ostream& os, const Move& m)
{
    MoveFrag *s;
    Coord d;
    char sep_char;
    
    d = m._initial_dir + m._origin;
    if (m.isJump())
    {
	d += m._initial_dir;
	sep_char = 'x';
    }
    else
    {
	sep_char = '-';
    }
    
    os << (char) (m._origin.x + 'a') << (int) (m._origin.y + 1)
       << sep_char
       << (char) (d.x + 'a') << (int) (d.y + 1);

    if (m._subseq_moves)
    {
	s = m._subseq_moves;
	while (s)
	{
	    d += s->dir;
	    d += s->dir;
	    os << "x" << (char) (d.x + 'a') << (int) (d.y + 1);
	    s = s->next;
	}
    }
    return os;
}


MoveFrag::MoveFrag ()
    : dir(Coord(0,0)), next(NULL)
{
}


MoveFrag::MoveFrag (const Coord& d)
    : dir(d), next(NULL)
{
}
