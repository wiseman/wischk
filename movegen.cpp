/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include "movegen.h"


Coord MoveGenerator::move_dir[6][4];
int MoveGenerator::initialized;


void MoveGenerator::init()
{
    initialized = 1;
    move_dir[Piece::Red][LEFT]  = Coord(-1, +1);
    move_dir[Piece::Red][RIGHT] = Coord(+1, +1);
    move_dir[Piece::Red][BACKLEFT]  = Coord(-1, -1);
    move_dir[Piece::Red][BACKRIGHT] = Coord(+1, -1);
    move_dir[Piece::Black][LEFT]  = Coord(+1, -1);
    move_dir[Piece::Black][RIGHT] = Coord(-1, -1);
    move_dir[Piece::Black][BACKLEFT]  = Coord(+1, +1);
    move_dir[Piece::Black][BACKRIGHT] = Coord(-1, +1);
}

void MoveGenerator::prime(const BoardConfig& board, Piece::Type c, short jump_mandatory)
{
  int i;
  MoveQ *mq;
  Move *m;

  if (!move_queue.isEmpty())
    move_queue.clear();
  if (!extend_queue.isEmpty())
    extend_queue.clear();

  color = c;
  can_jump = 0;
  num_moves = 0;

  if (color & Piece::Red || color & Piece::Black) {
    for (i = 0; i < 32; i++) {
      if (board.getPieceAt(BoardPos[i]) & color) {
	mq = generate_jumps(board, BoardPos[i]);
	if (mq) {
	  can_jump = 1;
	  while ((m = mq->get())) {
	    move_queue.add(m);
	  }
	  delete mq;
	}
      }
    }
    if (!can_jump || !jump_mandatory) {
      for (i = 0; i < 32; i++) {
	if (board.getPieceAt(BoardPos[i]) & color) {
	  mq = generate_regular_moves(board, BoardPos[i]);
	  if (mq) {
	    while ((m = mq->get())) {
	      move_queue.add(m);
	    }
	    delete mq;
	  }
	}
      }
    }
  }
}

MoveQ *MoveGenerator::generate_jumps(const BoardConfig& board, const Coord& from)
{
  Coord thru, to, dir;
  Piece::Type opcolor;
  MoveQ *mq, *ext_mq;
  Move *move, *ext_move;
  BoardConfig newboard;
  short made_move = 0;

  opcolor = (color & Piece::Red) ? Piece::Black : Piece::Red;
  mq = new MoveQ;

  dir = move_dir[color][LEFT];
  thru = from + dir;
  if (board.getPieceAt(thru) & opcolor) {
    to = thru + dir;
    if (board.getPieceAt(to) & Piece::Empty) {
      num_moves++;
      move = new Move(from, dir, 1);
      newboard = board.makeMove(*move, DONT_PROMOTE);
      ext_mq = generate_jumps(newboard, move->getDest());
      if (!ext_mq) {
	// not a multiple jump
	mq->add(move);
      } else {
	while ((ext_move = ext_mq->get())) {
	  Move* move_copy = new Move(*move);
	  move_copy->append(*ext_move);
	  mq->add(move_copy);
	  delete ext_move;
	}
	delete ext_mq;
	delete move;
      }
      made_move = 1;
    }
  }

  dir = move_dir[color][RIGHT];
  thru = from + dir;
  if (board.getPieceAt(thru) & opcolor) {
    to = thru + dir;
    if (board.getPieceAt(to) & Piece::Empty) {
      num_moves++;
      move = new Move(from, dir, 1);
      newboard = board.makeMove(*move, DONT_PROMOTE);
      ext_mq = generate_jumps(newboard, move->getDest());
      if (!ext_mq) {
	// not a multiple jump
	mq->add(move);
      } else {
	while ((ext_move = ext_mq->get())) {
	  Move* move_copy = new Move(*move);
	  move_copy->append(*ext_move);
	  mq->add(move_copy);
	  delete ext_move;
	}
	delete ext_mq;
	delete move;
      }
      made_move = 1;
    }
  }

  if (board.getPieceAt(from) & Piece::King) {
    dir = move_dir[color][BACKLEFT];
    thru = from + dir;
    if (board.getPieceAt(thru) & opcolor) {
      to = thru + dir;
      if (board.getPieceAt(to) & Piece::Empty) {
	num_moves++;
	move = new Move(from, dir, 1);
	newboard = board.makeMove(*move, DONT_PROMOTE);
	ext_mq = generate_jumps(newboard, move->getDest());
	if (!ext_mq) {
	  // not a multiple jump
	  mq->add(move);
	} else {
	  while ((ext_move = ext_mq->get())) {
	    Move* move_copy = new Move(*move);
	    move_copy->append(*ext_move);
	    mq->add(move_copy);
	    delete ext_move;
	  }
	  delete ext_mq;
	  delete move;
	}
	made_move = 1;
      }
    }
    
    dir = move_dir[color][BACKRIGHT];
    thru = from + dir;
    if (board.getPieceAt(thru) & opcolor) {
      to = thru + dir;
      if (board.getPieceAt(to) & Piece::Empty) {
	num_moves++;
	move = new Move(from, dir, 1);
	newboard = board.makeMove(*move, DONT_PROMOTE);
	ext_mq = generate_jumps(newboard, move->getDest());
	if (!ext_mq) {
	  // not a multiple jump
	  mq->add(move);
        } else {
	  while ((ext_move = ext_mq->get())) {
	    Move* move_copy = new Move(*move);
  	    move_copy->append(*ext_move);
	    mq->add(move_copy);
	    delete ext_move;
	  }
	  delete ext_mq;
	  delete move;
	}
	made_move = 1;
      }
    }
  }

  if (!made_move) {
    delete mq;
    mq = NULL;
  }

  return mq;
}

MoveQ *MoveGenerator::generate_regular_moves(const BoardConfig& board, const Coord& from)
{
  Coord to;
  Coord dir;
  MoveQ *mq;

  mq = new MoveQ;

  to = from + (dir = move_dir[color][LEFT]);
  if (board.getPieceAt(to) & Piece::Empty) {
    num_moves++;
    mq->add(new Move(from, dir));
  }
  
  to = from + (dir = move_dir[color][RIGHT]);
  if (board.getPieceAt(to) & Piece::Empty) {
    num_moves++;
    mq->add(new Move(from, dir));
  }

  if (board.getPieceAt(from) & Piece::King) {
    to = from + (dir = move_dir[color][BACKLEFT]);
    if (board.getPieceAt(to) & Piece::Empty) {
      num_moves++;
      mq->add(new Move(from, dir));
    }
    
    to = from + (dir = move_dir[color][BACKRIGHT]);
    if (board.getPieceAt(to) & Piece::Empty) {
      num_moves++;
      mq->add(new Move(from, dir));
    }
  }
  if (!mq->peek()) {
    delete mq;
    mq = NULL;
  }

  return mq;
}

