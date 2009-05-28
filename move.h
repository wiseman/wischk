/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef MOVE_H
#define MOVE_H

#include "coord.h"


class MoveFrag;

/**************************************
  Class: Move

  This class represents the concept of a checkers move.  It can deal
  with regular moves, jumps, and multiple jump moves.  If it has to
  keep track of each segment of a multiple jump, it uses a linked list
  of MoveFrag's.  A Move can be sent to an ostream.
**************************************/

class Move
{
public: // Structors
    
    Move ();
    Move (const Coord& origin, const Coord& dir, int jump = 0);
    Move (const Move& m);
    ~Move ();

public: // operators
    
    Move& operator= (const Move&);
    friend ostream& operator<< (ostream& output, const Move& m);

public: // The meat

    short append (const Move&);
    void addDir (const Coord&);
    void setOrigin (const Coord&);
    int setJump (int flag);
    void clear ();
    const Coord& getOrigin () const;
    Coord getDest () const;
    int getDir (Coord *) const;
    int getNextDir (Coord *) const;
    int isMulti () const;
    int isJump () const;
    
private: // Helper methods

    void append_frag_list(const Move&);
    void delete_frag_list();

private: // Members    

    char _is_jump;
    Coord _origin;
    Coord _initial_dir;
    MoveFrag *_subseq_moves;
    MoveFrag *_current_move;
};


/**************************************
  Class: MoveFrag

      This class is used only by the Move class.  It represents either
  an entire "simple" move, or one segment of a multiple jump.
**************************************/

class MoveFrag
{
    friend class Move;
    
public:
    MoveFrag();
    MoveFrag(const Coord& d);
    
    friend ostream& operator<<(ostream& output, const Move& m);

private:
    Coord dir;
    MoveFrag *next;
};


#endif
