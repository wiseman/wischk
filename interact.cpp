/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <iostream.h>
#include <ctype.h>
#include <string.h>
#include "globals.h"
#include "boardcon.h"
#include "movegen.h"
#include "move.h"
#include "interact.h"
#include "alphabet.h"
#include "display.h"



#define ARG       1
#define NO_ARG    0




static Cmd commands[] = {
{ "quit",	QUIT,   	NO_ARG,	"Quit the program" },
{ "level",	LEVEL,  	ARG,    "Set maximum level to search to (e.g. \"level 3\")." },
{ "time",	TIME,   	ARG,    "Set maximum time to think (e.g. \"time 30\")." },
{ "redraw",	REDRAW,		NO_ARG, "Redisplay the board." },
{ "m",		MOVE,   	ARG,    "Make a move (e.g. \"m a3c5\" or \"m a3c5e7\")." },
{ "help",	HELP,   	NO_ARG, "Display help." },
{ NULL,		UNKNOWN,	NO_ARG}
};

void show_help(void)
{
  int i;

  for (i = 0; commands[i].name != NULL; i++) {
    dp_gotoxy(0, 16 + i);
    dp_puts(commands[i].name);
    dp_gotoxy(12, 16 + i);
    dp_puts(commands[i].help);
  }
}


void get_user_command(char *s)
{
  cout << "\nCommand? ";
  fgets(s, 50, stdin);
  /*  cin.getline(s, 50, '\n'); */
}


COMMAND parse_command(const char *s, char *arg)
{
  int i;
  char *p = NULL;

  for (i = 0; commands[i].name != NULL; i++) {
    if ((p = strstr(s, commands[i].name)) == s)
      break;
  }
  if (commands[i].name != NULL) {
    if (commands[i].has_arg) {
      p += strlen(commands[i].name);
      if (isspace(*p)) {
	while (isspace(*p)) p++;
	strcpy(arg, p);
      } else {
	strcpy(arg, s);
	return UNKNOWN;
      }
    }
  } else {
    strcpy(arg, s);
    return UNKNOWN;
  }

  return commands[i].cmd;
}


void display_move(const Move& m)
{
  cout << "My move: " << m << endl;
}

void display_move_info(move_info *mi)
{
  cout << "value of board is ";
  if (mi->val == FORCED_MOVE)
    cout << "(unknown)\n";
  else
    cout << mi->val << endl;
  cout << "max level searched is " << MaxLevelSearched << endl;
}

void display_board(const BoardConfig& board)
{
  cout << board;
  cout << "\na b c d e f g h\n\n";
}


int get_col(char a)
{
  if (a < 'a' || a > 'h')
    return -1;
  else
    return a - 'a';
}

int get_row(char a)
{
  if (a < '1' || a > '9')
    return -1;
  else
    return a - '1';
}


/*
  parses moves of the form
  "a1b2"
  "b2c1d3e5"
*/
Move *parse_user_move(const BoardConfig& b, const char *s)
{
  const char *a;
  Coord origin, dest, dir;
  Move *move = new Move;
  Piece::Type opcolor;
  int ok;
  Piece::Type moving_piece;

  opcolor = (Color & Piece::Red) ? Piece::Black : Piece::Red;
  a = s;

  origin.x = get_col(*a++);
  origin.y = get_row(*a++);
  if (origin.x < 0 || origin.y < 0 || (!(b.getPieceAt(origin) & opcolor)))
    goto error;

  move->setOrigin(origin);
  moving_piece = b.getPieceAt(origin);

  do {
    dest.x = get_col(*a++);
    dest.y = get_row(*a++);
    if (dest.x < 0 || dest.y < 0)
      goto error;

    dir = dest - origin;
    if (dir == Coord(0, 0))
      goto error;

    if (!(b.getPieceAt(dest) & Piece::Empty))
      goto error;

    if (dir.x < -1 || dir.x > 1 || dir.y < -1 || dir.y > 1) {
      dir /= 2;
      move->setJump(1);
      if (!(b.getPieceAt(origin+dir) & Color))
	goto error;
    }

    ok = 0;
    if (opcolor == Piece::Red) {
      if (dir == Coord(-1, 1) || dir == Coord(1, 1))
	ok = 1;
      else {
	if (moving_piece & Piece::King) {
	  if (dir == Coord(-1, -1) || dir == Coord(1, -1))
	    ok = 1;
	}
      }
    } else {
      if (dir == Coord(-1, -1) || dir == Coord(1, -1))
	ok = 1;
      else {
	if (moving_piece & Piece::King) {
	  if (dir == Coord(-1, 1) || dir == Coord(1, 1))
	    ok = 1;
	}
      }
    }
    if (!ok)
      goto error;

    move->addDir(dir);
    origin = dest;
  } while (*a != '\0');

  {
    MoveGenerator movegen(b, opcolor);
    if (!NoMandatoryJumps && movegen.canJump() && !move->isJump()) {
      status_line("must jump!", 1);
      goto error;
    }
  }
  return move;

 error:
  delete move;
  return NULL;
}
