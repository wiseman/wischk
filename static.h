/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

extern int piece_val;
extern int king_val;
extern int move_val;
extern int jumps;
extern int piece_row_advance;
extern int piece_middle_center_squares;
extern int piece_middle_side_squares;
extern int piece_center_goalies;
extern int piece_side_goalies;
extern int piece_double_corner;
extern int is_home_free;
extern int dist_factor;

class Coord;

float distance(const Coord& a, const Coord &b);
int read_stat_val(const char *filename);
