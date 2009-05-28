/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <iostream.h>
#include "boardcon.h"
#include "piece.h"
#include "dispprim.h"
#include "display.h"
#include "globals.h"
#include "move.h"
#include "alphabet.h"


void status_line(char *s, unsigned char line)
{
  dp_gotoxy(0, 14 + line);
  dp_printf("%s", s);
  dp_refresh();
}


void clear_status_line(void)
{
  dp_gotoxy(0, 14);
  dp_del_eol();
  dp_gotoxy(0, 15);
  dp_del_eol();
  dp_refresh();
}


void display_init(RefreshFlag rflag)
{
  dp_clear();
  display_board_init(DONT_REFRESH);
  display_stats_init(DONT_REFRESH);

  if (rflag == REFRESH)
	dp_refresh();
}


void display_board_init(RefreshFlag rflag)
{
  int i;

  dp_gotoxy(0, 0);
  dp_puts("+-----------------+");
  for (i = 1; i < 9; i++) {
    dp_gotoxy(0, i);
    dp_puts("| - - - - - - - - |");
  }
  dp_gotoxy(0, 9);
  dp_puts("+-----------------+");

  for (i = 1; i < 9; i++) {
    dp_gotoxy(19, i);
    dp_printf("%d", 9 - i);
    dp_gotoxy(2 * i, 10);
    dp_putc(('a' - 1) + i);
  }
  if (rflag == REFRESH)
    dp_refresh();
}


void display_stats_init(RefreshFlag rflag)
{
  dp_gotoxy(22, 1);
  dp_puts("Your last move");
  dp_gotoxy(22, 4);
  dp_puts("My last move");

  dp_gotoxy(43, 0);
  dp_puts("Time spent on last move:");
  dp_gotoxy(43, 1);
  dp_puts("Total time spent thinking:");
  dp_gotoxy(43, 2);
  dp_puts("Max. depth fully searched:");
  dp_gotoxy(43, 3);
  dp_puts("Max. depth:");
  
  dp_gotoxy(43, 5);
  dp_puts("Static value of board:");
  dp_gotoxy(43, 6);
  dp_puts("Dynamic value of board:");

  dp_gotoxy(43, 8);
  dp_puts("Max. lookahead:");
  dp_gotoxy(43, 9);
  dp_puts("Max. thinking time:");
  dp_gotoxy(43, 10);
  dp_puts("Heuristic continuation?");

  dp_gotoxy(1, 13);
  dp_puts("Your move?");

  if (rflag == REFRESH)
    dp_refresh();
}


void display_move(short x, short y, Move *the_move, RefreshFlag rflag)
{
  char sep_char;
  Coord sq, dir;
  unsigned short len = 0;

  sep_char = (the_move->isJump()) ? 'x' : '-';
  dp_gotoxy(x, y);
  
  sq = the_move->getOrigin();
  dp_printf("%c%c", sq.x + 'a', sq.y + '1');
  len += 2;
  the_move->getDir(&dir);
  do {
    if (the_move->isJump())
      dir *= 2;
    sq += dir;
    dp_printf("%c%c%c", sep_char, sq.x + 'a', sq.y + '1');
    len += 3;
  } while (the_move->getNextDir(&dir));
  
  dp_printf("%*s", 21 - len, " ");

  if (rflag == REFRESH)
    dp_refresh();
}


void display_comp_move(Move *move, RefreshFlag rflag)
{
  display_move(22, 5, move, rflag);
}


void display_user_move(Move *move, RefreshFlag rflag)
{
  display_move(22, 2, move, rflag);
}


void display_stats(disp_move_info *m, RefreshFlag rflag)
{
  unsigned long tot_secs;
  unsigned short hours, mins, secs;
  float fsecs;

  /* calculate and print time spent on last move in hh:mm:ss.ss format */
  dp_gotoxy(71, 0);
  fsecs = (float) m->mi->elapsed_cpu_time / clocks_per_sec;
  mins = (int) (fsecs / 60);
  fsecs -= mins * 60;
  dp_printf("%02d:%05.2f", mins, fsecs);

  /* calculate and print total time spent thinking in hh:mm:ss format */
  tot_secs = m->tot_clocks / clocks_per_sec;
  secs = (tot_secs % 60);
  mins = (tot_secs / 60) % 60;
  hours = tot_secs / 3600;
  dp_gotoxy(71, 1);
  dp_printf("%02d:%02d:%02d", hours, mins, secs);

  /* print how deep search went for last move */
  dp_gotoxy(76, 3);
  dp_printf("%3d", MaxLevelSearched);
/*  dp_printf("%3d", m->mi->max_level_fully_searched); */
  dp_gotoxy(76, 2);
  dp_printf("%3d", MaxLevel - 1);
/*  dp_printf("%3d", m->mi->max_level_searched); */

  /* print value of top node */
  dp_gotoxy(70, 6);
  if (m->mi->val == FORCED_MOVE)
    dp_puts("(unknown)");
  else if (m->mi->val >= MY_WIN)
    dp_puts("    (win)");
  else if (m->mi->val <= MY_LOSS)
    dp_puts("   (loss)");
  else
    dp_printf("%9d", m->mi->val);

  /* print static value of board configuration */
  dp_gotoxy(71, 5);
  dp_printf("%8hd", m->static_val);

  dp_gotoxy(67, 10);
  if (m->is_using_hc)
    dp_printf("yes");
  else
    dp_printf("no");

  dp_gotoxy(67, 8);
  if (m->is_time_limited)
    dp_printf("%-5d s", m->mi->max_sec);
  else 
    dp_printf("n/a");

  dp_gotoxy(67, 9);
  if (m->is_time_limited)
    dp_printf("n/a");
  else
    dp_printf("%-5d", m->mi->max_lvl);

  if (rflag == REFRESH)
    dp_refresh();
}


void display_board(const BoardConfig &b, RefreshFlag rflag, CacheFlag cflag)
{
  int x, y, ox, oy;
  static BoardConfig b_buf;
  Piece::Type new_piece;
  
  ox = oy = 0;
  
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      Coord pos(ox + x, oy + y);
      if (b_buf.getPieceAt(pos) != (new_piece = b.getPieceAt(pos)) || cflag == DONT_CACHE) {
	b_buf.setPieceAt(pos, new_piece);
	display_piece(new_piece, ox + (2 * x) + 2, oy + 8 - y, DONT_REFRESH);
      }
    }
  }
  
  if (rflag == REFRESH)
    dp_refresh();
}


void display_piece(Piece::Type piece, short x, short y, RefreshFlag rflag)
{
  dp_gotoxy(x, y);
  switch (piece) {
    case Piece::Red:		dp_putc('r'); break;
    case Piece::Black:		dp_putc('b'); break;
    case Piece::RedKing:	dp_putc('R'); break;
    case Piece::BlackKing:	dp_putc('B'); break;
    case Piece::Empty:		dp_putc('-'); break;
    default:			dp_putc('?'); break;
  }
  if (rflag == REFRESH)
    dp_refresh();
}

void A_get_user_command(char *s)
{
  dp_gotoxy(13, 13);
  dp_del_eol();
  dp_gotoxy(13, 13);
  dp_refresh();
  dp_getstr(s);
}

#ifdef __TURBOC__

#include <string.h>

void dp_getstr(char *s)
{
	char s2[23];

	s2[0] = 19;
	cgets(s2);
	strcpy(s, s2+2);
}

#endif
