/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "dispprim.h"
#include "piece.h"
#include "globals.h"


struct disp_move_info
{
  move_info *mi;

  unsigned long tot_clocks;
  int is_using_hc;
  int is_time_limited;
  short static_val;
};


class BoardConfig;


enum CacheFlag { DONT_CACHE, CACHE };

void status_line(char *s, unsigned char line = 0);
void clear_status_line(void);
void display_init(RefreshFlag rflag = REFRESH);
void display_stats_init(RefreshFlag rflag = REFRESH);
void display_board_init(RefreshFlag rflag = REFRESH);
void display_board(const BoardConfig &b, RefreshFlag rflag = REFRESH, CacheFlag cflag = CACHE);
void display_stats(disp_move_info *m, RefreshFlag rflag = REFRESH);
void display_move(short x, short y, Move *move, RefreshFlag rflag = REFRESH);
void display_user_move(Move *move, RefreshFlag rflag = REFRESH);
void display_comp_move(Move *move, RefreshFlag rflag = REFRESH);
void display_piece(Piece::Type, short x, short y, RefreshFlag rflag = REFRESH);
void A_get_user_command(char *s);

#endif


