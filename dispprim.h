/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef DISPPRIM_H
#define DISPPRIM_H

enum RefreshFlag { REFRESH = 0, DONT_REFRESH = 1 };

#ifdef DISP_CURSES
#include <curses.h>
#define dp_init initscr
#define dp_del_eol clrtoeol
#define dp_clear clear
#define dp_gotoxy(x, y) move(y, x)
#define dp_puts addstr
#define dp_printf printw
#define dp_putc addch
#define dp_refresh refresh
#define dp_end() { dp_gotoxy(0, LINES-4); endwin();}
#define dp_getstr getstr
#elif __TURBOC__
#include <conio.h>
#define dp_init clrscr
#define dp_clear clrscr
#define dp_del_eol clreol
void dp_getstr(char *s);
void inline dp_gotoxy(short x, short y) { gotoxy(x+1, y+1); }
#define dp_puts cputs
#define dp_printf cprintf
#define dp_putc putch
inline void dp_end(void) { text_info t; gettextinfo(&t); dp_gotoxy(0, t.screenheight - 3); }
inline void dp_refresh(void) {}
#else
#error "I don't know what to do about screen libraries!"
#endif

#endif
