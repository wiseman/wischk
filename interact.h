/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include "globals.h"
#include "boardcon.h"
#include "move.h"


enum COMMAND { QUIT, LEVEL, TIME, MOVE, REDRAW, HELP, UNKNOWN };

struct Cmd
{
  char *name;
  COMMAND cmd;
  int has_arg;
  char *help;
};

void show_help(void);
void display_board(const BoardConfig& board);
void display_move(const Move&);
void display_move_info(move_info *);
void get_user_command(char *cmd);
COMMAND parse_command(const char *cmd, char *arg);
Move *parse_user_move(const BoardConfig&, const char *s);
