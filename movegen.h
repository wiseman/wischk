/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "boardcon.h"
#include "coord.h"
#include "piece.h"
#include "move.h"
#include "moveq.h"


/**************************************
  Class: MoveGenerator

      Once 'primed' with a board configuration and a color (red or black),
  this class will spit out all legal moves that can be made, one after
  another, upon request.  It follows the rule that jumps are mandatory.
      It has the infrastructure to allow it to generate multiple jumps,
  but currently only generates simple moves/jumps.
***************************************/

class MoveGenerator {
public:

  enum dir {
    LEFT      = 0,
    RIGHT     = 1,
    BACKLEFT  = 2,
    BACKRIGHT = 3
  };
  
  MoveGenerator();
  MoveGenerator(const BoardConfig&, Piece::Type color, short jump_mandatory = 1);
  ~MoveGenerator();
  void prime(const BoardConfig&, Piece::Type color, short jump_mandatory = 1);
  Move *getNextMove();
  short getNumberMoves() const { return num_moves; };
  int canJump() const { return can_jump; };
  
 private:
  MoveGenerator(const MoveGenerator&);
  MoveGenerator& operator=(const MoveGenerator&);
  void init();
  MoveQ* generate_jumps(const BoardConfig& board, const Coord& from);
  MoveQ* generate_regular_moves(const BoardConfig& board, const Coord& from);

  static Coord move_dir[6][4];
  static int initialized;

  Piece::Type color;
  int can_jump;
  short num_moves;
  MoveQ move_queue;
  MoveQ extend_queue;

};


inline MoveGenerator::MoveGenerator()
{
  if (!initialized)
    init();
}

inline MoveGenerator::MoveGenerator(const BoardConfig& board, Piece::Type c, short jump_mandatory)
: color(c)
{
  if (!initialized)
    init();
  prime(board, c, jump_mandatory);
}

inline MoveGenerator::~MoveGenerator()
{
}

inline Move *MoveGenerator::getNextMove()
{
  if (move_queue.isEmpty())
    return NULL;
  else
    return move_queue.get();
}


#endif
