/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <errno.h>
#include <stdlib.h>
#include <iostream.h>
#include "globals.h"
#include "boardcon.h"



// array of legal board positions
Coord BoardPos[32] =
{
  Coord(1, 7), Coord(3, 7), Coord(5, 7), Coord(7, 7),
  Coord(0, 6), Coord(2, 6), Coord(4, 6), Coord(6, 6),
  Coord(1, 5), Coord(3, 5), Coord(5, 5), Coord(7, 5),
  Coord(0, 4), Coord(2, 4), Coord(4, 4), Coord(6, 4),
  Coord(1, 3), Coord(3, 3), Coord(5, 3), Coord(7, 3),
  Coord(0, 2), Coord(2, 2), Coord(4, 2), Coord(6, 2),
  Coord(1, 1), Coord(3, 1), Coord(5, 1), Coord(7, 1),
  Coord(0, 0), Coord(2, 0), Coord(4, 0), Coord(6, 0)
};
BoardConfig InitialBoard;  // global object, initial board configuration

int BoardConfig::comp_output;
int BoardConfig::initialized;

void BoardConfig::do_init()
{
  Coord c;
  int x, y;

  initialized = 1;
  for (y = 7; y >= 0; y--) {
    for (x = 0; x < 8; x++) {
      c = Coord(x, y);
      if (x % 2 == y % 2) {
	if (y < 3)
	  InitialBoard.setPieceAt(c, Piece::Red);
	else if (y < 5)
	  InitialBoard.setPieceAt(c, Piece::Empty);
	else
	  InitialBoard.setPieceAt(c, Piece::Black);
      } else
	InitialBoard.setPieceAt(c, Piece::Empty);
    }
  }
}
  

BoardConfig BoardConfig::makeMove(const Move& move, short dont_promote) const
{
  BoardConfig newboard;
  Coord dir, dest, thru, origin;

  origin = move.getOrigin();
  move.getDir(&dir);
  newboard = *this;
  if (!move.isJump()) {
    dest = origin + dir;
    newboard.setPieceAt(dest, newboard.getPieceAt(origin));
    newboard.setPieceAt(origin, Piece::Empty);
  } else {
    do {
      thru = origin + dir;
      dest = thru + dir;
      newboard.setPieceAt(dest, newboard.getPieceAt(origin));
      newboard.setPieceAt(origin, Piece::Empty);
      newboard.setPieceAt(thru, Piece::Empty);
      origin = dest;
    } while (move.getNextDir(&dir));
  }

  if (!dont_promote) {
    if (dest.y == 0 || dest.y == 7)
      promote(newboard);
  }

  return newboard;
}

void BoardConfig::promote(BoardConfig& board) const
{
  int x;
  Coord c(0, 7);
  Coord d(0, 0);
  Piece::Type piece;

  for (x = 1; x < 8; x += 2) {
    c.x = x;
    d.x = x - 1;
    piece = board.getPieceAt(c);
    if ((piece & Piece::Red) && !(piece & Piece::King))
      board.setPieceAt(c, Piece::RedKing);
    piece = board.getPieceAt(d);
    if ((piece & Piece::Black) && !(piece & Piece::King))
      board.setPieceAt(d, Piece::BlackKing);
  }
}

ostream & operator << (ostream & os, const BoardConfig & board)
{
  int x, y;

  for (y = 7; y >= 0; y--) {
    for (x = 0; x < 8; x++) {

      switch (board.getPieceAt(Coord(x, y))) {

      case Piece::Empty:	os << ((BoardConfig::comp_output) ? "e " : "- "); break;
      case Piece::Red: 		os << "r "; break;
      case Piece::RedKing:	os << "R "; break;
      case Piece::Black:	os << "b "; break;
      case Piece::BlackKing:	os << "B "; break;

      default:
	cerr << "trying to output board, got strange piece = "
	     << (int) board.getPieceAt(Coord(x, y)) << " ! ouch!\n";
      }

      if (!os) {
	cerr << "Error while trying to output board. "
	     << strerror(errno) << endl;
      }
    }
    os << endl;
  }

    return os;
}


istream & operator >> (istream & is, BoardConfig & board)
{
  int             x, y;
  char            c;
  Piece::Type     p;

  for (y = 7; y >= 0; y--) {
    for (x = 0; x < 8; x++) {
      if (is >> c) {

	switch (c) {

	case 'e':
	case '-':	p = Piece::Empty;	break;
	case 'b':	p = Piece::Black;	break;
	case 'r':	p = Piece::Red;		break;
	case 'B':	p = Piece::BlackKing;	break;
	case 'R':	p = Piece::RedKing;	break;

	default:
	  cerr << "Trying to read in board, got strange piece \"" << c << "\"." << endl;
	  p = Piece::Empty;
	}
	board.setPieceAt(Coord(x, y), p);
      }
    }
  }

  if (!is) {
    cerr << "Error while trying to input board. " << strerror(errno) << endl;
  }
  return is;
}
