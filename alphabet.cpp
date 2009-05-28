/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <iostream.h>
#include <stdlib.h>
#include <math.h>

#include "globals.h"
#include "boardcon.h"
#include "move.h"
#include "movegen.h"
#include "piece.h"
#include "alphabet.h"



//--------------------------------------------------------------
// Global variables
//--------------------------------------------------------------

int             MaxLevel;	// Max depth to search to
int 		MaxLevelSearched;
int             BranchFactor;	// Number of possible moves at each level
int             NumStaticEvals;	// Number of static evaluations preformed
int             NumABCutoffs;	// Number of alpha-beta cutoffs that
				// occurred



// Determines whether alpha < beta.  If alpha >= beta, then cutoff should
// be made.
int ABcutoff(int a, int b)
{
  if (a >= b) {
    NumABCutoffs++;
    return 0;
  } else {
    return 1;
  }
}



/**************************************
  Function: eval_board

  Implement alpha-beta algorithm (optionally with heuristic continuation:
  don\'t do static eval until no jumps can be made).  Note: root node is
  level *1*.
**************************************/
				      
int eval_board(const BoardConfig& board, short level, short alpha, short beta, Move* move, char jumps_mandatory)
{
  Move *trial_move, null_move;
  BoardConfig newboard;
  Piece::Type color;
  int maximizing;
  int val;
  MoveGenerator movegen;
				      
  
  maximizing = level % 2;
  if (maximizing)
    color = Color;
  else
    color = (Color & Piece::Red) ? Piece::Black : Piece::Red;
  
  // check right off the bat to see if this is a winning/losing board.
  movegen.prime(board, color, jumps_mandatory);
  if (movegen.getNumberMoves() == 0)
    return (maximizing) ? MY_LOSS : MY_WIN;
  
  // is it time to do a static evaluation?
  if (level > MaxLevel) {
    if (NoHeuristicCont || !jumps_mandatory) {
      return static_eval(board);
    } else {
      if (!movegen.canJump()) {
	MoveGenerator movegen2(board, (color & Piece::Red) ? Piece::Black : Piece::Red, jumps_mandatory);
	if (!movegen2.canJump()) {
	  return static_eval(board);
	}
      }
    }
  }
				      
  // are we at the top level, and only one move is possible?
  if (level == 1 && movegen.getNumberMoves() == 1) {
    // yes, so make that move, and return the magic value that will
    // inform make_move_timed that it doesn\'t have to keep trying.
    *move = *movegen.getNextMove();
    return FORCED_MOVE;
  }
				      
  if (level > MaxLevelSearched)
    MaxLevelSearched = level;
				      
  while ((trial_move = movegen.getNextMove()) && ABcutoff(alpha, beta)) {
    newboard = board.makeMove(*trial_move);
    val = eval_board(newboard, level + 1, alpha, beta, &null_move, jumps_mandatory);
    if (maximizing) {
      if (val > alpha) {
	alpha = val;
	*move = *trial_move;
      }
    } else {
      if (val < beta) {
	beta = val;
	*move = *trial_move;
      }
    }
    delete trial_move;
  }
  
  if (trial_move)
    delete trial_move;
  
  if (maximizing)
    return alpha;
  else
    return beta;
}
