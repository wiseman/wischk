/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#define MY_LOSS     (-32000)
#define MY_WIN      (32000)
#define FORCED_MOVE (31999)



int eval_board(const BoardConfig& board, short level, short alpha, short beta,
	       Move* move, char jumps_mandatory);

int static_eval(const BoardConfig &);

int read_stat_val(const char *file);

