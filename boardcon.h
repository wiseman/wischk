/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef BOARDCONFIG_H
#define BOARDCONFIG_H

#include <iostream.h>
#include <string.h>
#include "piece.h"
#include "move.h"
#include "coord.h"


#define DONT_PROMOTE 1


/**************************************
  Class: BoardConfig

  This class represents a checkers board configuration.  It knows how
  to make moves (although it doesn't check for illegal moves), and can
  be sent to/read from streams.
**************************************/

class BoardConfig
{
public:
    BoardConfig();
    BoardConfig(const BoardConfig&);
    BoardConfig& operator=(const BoardConfig&);
    ~BoardConfig();

    Piece::Type getPieceAt(const Coord&) const;
    void setPieceAt(const Coord&, const Piece::Type);
    BoardConfig makeMove(const Move&, short dont_promote = 0) const;
    int moveIsLegal(const Move&) const;
    void setCompatibleOutput() const;

    friend istream & operator >> (istream & input, BoardConfig & board);
    friend ostream& operator << (ostream & output, const BoardConfig & board);

private:
    void promote(BoardConfig&) const;
    static void do_init();
    
private:
    unsigned char b[8][8];
    
    static int comp_output;
    static int initialized;

};


inline BoardConfig::BoardConfig()
{
  memset(b, Piece::Out_Of_Bounds, sizeof b);
  if (!initialized) {
    do_init();
  }
}

inline BoardConfig::~BoardConfig()
{}

inline BoardConfig::BoardConfig(const BoardConfig& board)
{
  if (!initialized) {
    do_init();
  }
  memcpy(b, board.b, sizeof b);
}

inline BoardConfig& BoardConfig::operator=(const BoardConfig& board)
{
  memcpy(b, board.b, sizeof b);
  return *this;
}

inline void BoardConfig::setPieceAt(const Coord& c, const Piece::Type p)
{
    b[c.x][c.y] = p;
}

inline Piece::Type BoardConfig::getPieceAt(const Coord& c) const
{
  if (c.x >= 0 && c.x <= 7 && c.y >= 0 && c.y <= 7)
    return (Piece::Type) b[c.x][c.y];
  else
    return Piece::Out_Of_Bounds;
}

inline int BoardConfig::moveIsLegal(const Move&) const
{
  return 1;
}

inline void BoardConfig::setCompatibleOutput() const
{
  comp_output = 1;
}


extern Coord BoardPos[32];
extern BoardConfig InitialBoard;

#endif
