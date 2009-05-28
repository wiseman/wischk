/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <math.h>
#include <fstream.h>
#include "boardcon.h"
#include "globals.h"
#include "movegen.h"
#include "alphabet.h"
#include "static.h"


float distance(const Coord& a, const Coord &b);
static int end_game(const BoardConfig& board);
static int home_free(const BoardConfig& board);


int piece_val = 30;
int king_val = 80;
int move_val = 2;
int jumps = 6;
int piece_row_advance = 1;
int piece_middle_center_squares = 4;
int piece_middle_side_squares = -2;
int piece_center_goalies = 10;
int piece_side_goalies = 8;
int piece_double_corner = 4;
int is_home_free = 15;
int dist_factor = 5;


/**************************************
  Function: static_eval

      This function applies all the board evaluation heuristics.  A
return value of MY_LOSS signifies a loss for the computer, MY_WIN
signifies a win.

  Heuristics:

      1. Number/type of men.
             A bonus for each man, larger bonus for each king.
         Corresponding penalty for opponent\'s pieces.

      2. Depth of advance.
             Bonus for each man proportional to distance from home
	 row.  Closer to opponent\'s home row is better.
	 Corresponding penalty for opponent\'s pieces that are near
	 our home row.

      3. Mobility.
             Bonus for each possible legal move.  Higher bonus for
	 jumps.

      4. Center vs. sides.
             The four center squares are better to have men on than
	 the two squares in the middle right and the two on the middle
	 left.  Corresponding penalties for have opponent\'s pieces in
	 the middle positions.

      5. Goalies
             Bonus for having men on home row.  Best are two middle
	 sqaures, but two side squares are good too.

      6. Corner
             "Play from your single corner and attack your opponent\'s
	 double corner.  Keep your own double corner as strong as
	 possible."

      7. Home free
             A bonus if a piece is in a position where it *could*
	 reach the opponent\'s home row and be kinged without the
	 opponent being able to stop it.  There is a corresponding
	 penalty for opponent\'s pieces that are "home free".

      8. End game
             If there are less than 6 pieces on the board, then there is a
	 penalty assessed equal to the maximum distance between any of your
	 kings and any of the opponent\'s pieces.

**************************************/

int static_eval(const BoardConfig& board)
{
  int score, tscore;
  Coord pos;
  short i;
  Piece::Type piece;
  short red_kings, black_kings, black_pieces, red_pieces;
  int black_material, red_material;
  int min_material, max_material;
  short outnumber;
  
  NumStaticEvals++;
  
  red_kings = black_kings = black_pieces = red_pieces = score = 0;
  red_material = black_material = 0;
  
  /*
    
   */
  for (i = 0; i < 32; i++) {
    tscore = 0;
    piece = board.getPieceAt(BoardPos[i]);
    if (piece & Piece::NotEmpty) {
      if (!(piece & Piece::King)) {
	if (i == 13 || i == 14 || i == 17 || i == 18)
	  tscore += piece_middle_center_squares;
	else if (i == 12 || i == 16 || i == 15 || i == 19)
          tscore += piece_middle_side_squares;
	if (piece & Piece::Red) {

	  red_pieces++;
	  red_material += piece_val;

	  if (i == 28 || i == 31)
	    tscore += piece_side_goalies;
	  else if (i == 29 || i == 30)
	    tscore += piece_center_goalies;
	  if (i == 27 || i == 31)
	    tscore += piece_double_corner;
	} else {
	  black_pieces++;
	  black_material += piece_val;

	  if (i == 0 || i == 3)
	    tscore += piece_side_goalies;
	  else if (i == 1 || i == 2)
	    tscore += piece_center_goalies;
	  if (i == 0 || i == 4)
	    tscore += piece_double_corner;
	}
      } else {
	if (piece & Piece::Red) {
	  red_kings++;
	  red_material += king_val;
	} else {
	  black_kings++;
	  black_material += king_val;
	}
      }
      if (piece & Piece::Red)
	score += tscore;
      else
	score -= tscore;
    }
  }

  if (red_material > black_material) {
    max_material = red_material;
    min_material = black_material;
  } else {
    max_material = black_material;
    min_material = red_material;
  }
  if (min_material == 0)
    min_material = 1;

  score += (int) ((red_material - black_material) * (((float) max_material) / min_material));
  
  
  /*
    Bonus for each possible legal move, penalty for each of opponent's
    possible legal moves.
    */
  MoveGenerator movegen(board, Piece::Red);
  tscore = movegen.getNumberMoves();
  if (tscore == 0) {
    tscore = MY_LOSS;
  } else {
  if (movegen.canJump())
    tscore *= jumps;
  else
    tscore *= move_val;
  }
  score += tscore;
  
  movegen.prime(board, Piece::Black);
  tscore = -movegen.getNumberMoves();
  if (tscore == 0) {
    tscore = MY_WIN;
  } else {
    if (movegen.canJump())
      tscore *= jumps;
    else
      tscore *= move_val;
  }
  score += tscore;
  
  
  
  /*
    Bonus for each non-king piece proportional to how close it is to
    opponents home row (closer is better).  Similar penalty for
    opponent's pieces that are close to our home row.
    */
  tscore = 0;
  for (i = 0; i < 32; i++) {
    pos = BoardPos[i];
    piece = board.getPieceAt(pos);
    if (piece & Piece::NotEmpty && !(piece & Piece::King)) {
      if (piece & Piece::Red) {
        tscore += pos.y * piece_row_advance;
      } else {
	tscore -= (7 - pos.y) * piece_row_advance;
      }
    }
  }
  score += tscore;
  
  /* bonus for each man who has a free shot at becoming king, penalty
     for each opponent who has a free path.
     */
  score += home_free(board);
  
  outnumber = red_kings - black_kings;
  if (Color & Piece::Black)
    outnumber = -outnumber;

  if (outnumber > 0 && ((Color & Piece::Red ? black_pieces : red_pieces) < 5))
    score += end_game(board);
  
  return (Color & Piece::Red) ? score : -score;
}


/**************************************
  Function: home_free

      This function attempts to determine how many non-king pieces
  have a free path to the opponent's home row.  If there is a piece
  for which it is true that if the player used every one of his moves
  to move the piece towards the opposing home row, then none of the
  opponent's pieces is in a position from preventing that piece from
  becoming a king, then it has a free row.
      Currently, I think it misses some pieces that have a free path,
  but all the pieces it finds really do have a free path.
**************************************/

int home_free(const BoardConfig& board)
{
  int tscore;
  Piece::Type row[8];
  Piece::Type piece;
  Coord pos;
  int i;
  int min_red_row;
  int max_black_row;

  min_red_row = 8;
  max_black_row = -1;
  memset(row, 0, sizeof row);
  for (i = 0; i < 32; i++) {
    pos = BoardPos[i];
    piece = board.getPieceAt(pos);
    if ((piece & Piece::Red) && pos.y < min_red_row)
      min_red_row = pos.y;
    if ((piece & Piece::Black) && pos.y > max_black_row)
      max_black_row = pos.y;
  }

  tscore = 0;
  for (i = 0; i < 32; i++) {
    pos = BoardPos[i];
    piece = board.getPieceAt(pos);
    if (piece & Piece::NotEmpty) {
      if (piece & Piece::Black && !(piece & Piece::King)) {
	if (pos.y <= min_red_row) {
	  tscore -= is_home_free;
	}
      } else {
	if (!(piece & Piece::King) && (pos.y >= max_black_row)) {
	  tscore += is_home_free;
	}
      }
    }
  }
  return tscore;
}


// find the distance between two coordinates
float distance(const Coord& a, const Coord &b)
{
  int x_dist, y_dist;
  
  x_dist = (a.x - b.x);
  x_dist *= x_dist;
  y_dist = (a.y - b.y);
  y_dist *= y_dist;
  if (x_dist + y_dist > 0) 
    return sqrt(x_dist + y_dist);
  else
    return 0;
}

/**********************************
    Function: end_game
    
        This function implements the \'end_game\' heuristic.  It attempts to
    minimize the distance between pieces (going in for the kill).
**********************************/
    
int end_game(const BoardConfig& board)
{
  int i, j;
  float max_dist, dist;
  Piece::Type piece1, piece2, opcolor;
  int score;
  
  max_dist = dist = 0;
  opcolor = (Color & Piece::Red) ? Piece::Black : Piece::Red;
  for (i = 0; i < 32; i++) {
    piece1 = board.getPieceAt(BoardPos[i]);
    if (piece1 & opcolor) {
      for (j = 0; j < 32; j++) {
	piece2 = board.getPieceAt(BoardPos[j]);
	if (piece2 & (Color | Piece::King)) {
	  dist += distance(BoardPos[i], BoardPos[j]);
	}
      }
    }
  }
  score = (int) dist;
  return -score * dist_factor;
}
