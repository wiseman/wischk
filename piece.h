/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef PIECE_H
#define PIECE_H

/**************************************
  Class: Piece

      This class is just a wrapper for an enum.  The enum types have to
  be prefixed with "Piece::", which I thought would prevent name space
  collision, since "Red" might be used in lots of programs.
***************************************/

class Piece
{
public:
  enum Type {
    Empty	  = 0x01, 
    Red		  = 0x02,
    Black	  = 0x04,
    King          = 0x08,
    RedKing       = 0x0a,	// these two are really
    BlackKing     = 0x0c,	// just for convenience
    NotEmpty	  = 0xfe,
    Out_Of_Bounds = 0x40
  };
};


#endif
