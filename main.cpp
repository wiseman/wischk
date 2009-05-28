/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <new.h>

#ifdef unix
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#endif

#include "boardcon.h"
#include "move.h"
#include "piece.h"
#include "globals.h"
#include "alphabet.h"
#include "static.h"
#include "movegen.h"
#include "getopts.h"
#include "interact.h"
#include "memhand.h"
#include "display.h"


#ifdef unix
const char PATH_SEPARATOR = '/';
#else
const char PATH_SEPARATOR = '\\';
#ifdef __TURBOC__
#define REMOVE_EXECUTABLE_EXTENSION
// We need more than the default 4k stack.
extern unsigned _stklen = 65500U;
#endif
#endif



char *strlower(char *);
int loser(const BoardConfig&);
BoardConfig make_move_timed(move_info *);
BoardConfig make_move(move_info *);
int read_config(const char *name, istream& in, BoardConfig *board);
char *get_prog_name(const char *s);
clock_t get_cpu_time(void);

// some globals
int		clocks_per_sec;
Piece::Type     Color;
int             Interactive;
int             NoHeuristicCont;
int             NoMandatoryJumps;
int             LimitSearchLevel;

// some stuff local to this module
static char copyright[] = "wischk $Revision: 2.6 $: the \"wise\" checkers playing program.\n"
                          "Copyright (C) 1993, 1998  John J. Wiseman\n";
static char first_config_line[256];
static char config_filename[200];
static char stat_config_file[256];
static char *prog_name;
static int show_moves = 0;
static char opponents_play[40];

static opt_t options[] = {
  { "nohc",     OPTBOOL,        &NoHeuristicCont        },
  { "noj",      OPTBOOL,        &NoMandatoryJumps       },
  { "l",        OPTBOOL,        &LimitSearchLevel       },
  { "i",        OPTBOOL,        &Interactive            },
  { "f",        OPTSTR,         config_filename         },
  { "c",        OPTSTR,         stat_config_file        },
  { "show",	OPTBOOL,	&show_moves		},
  { "play",	OPTSTR,		opponents_play		},
  { NULL,       (opt_type)0,    NULL                    }
};


// The heart of the program.
int
main (int argc, char *argv[])
{
  int opt;
  ifstream config_in;
  ofstream config_out;
  BoardConfig initial_board;
  Move the_move, user_move, *user_move_p;
  int who_lost;
  int time_to_quit = 0;
  char command[80], arg[20];
  move_info minfo;
  disp_move_info mi;
  
  set_new_handler(out_of_memory);
  prog_name = get_prog_name(argv[0]);

#ifdef unix
  clocks_per_sec = sysconf(_SC_CLK_TCK);
#else
  clocks_per_sec = CLK_TCK;
#endif

  // parse the command line
  opt = getopts(argc, argv, options);
  if (!show_moves && (argc - opt != 1)) {
    cerr << "usage: " << prog_name << "          " + strlen(prog_name)
         << " [-i] [-l] [-nohc] [-noj] [-f<config file>]\n"
         << "                  [-c<parm_file>] [-play<move] [-show] <n>\n"
         << "   Defaults to reading/writing checkers.cfg, thinking for at most <n> seconds.\n"
         << "    -i                Play against human interactively instead of\n"
         << "                      reading/writing configuration files.\n"
         << "    -l                Instead of thinking for at most <n> seconds, look\n"
	 << "                      ahead <n> moves.\n"
	 << "    -nohc             Don't use the heuristic continuation enhancement of\n"
	 << "                      the alpha-beta algorithm.\n"
	 << "    -noj              Jumps are not mandatory.\n"
	 << "    -f<config_file>   Instead of reading/writing checkers.cfg, use the\n"
	 << "                      specified filename.\n"
	 << "    -c<parm_file>     Use the values in <parm_file> to evaluate boards.\n"
	 << "    -show             Show what moves are possible for each player, then\n"
	 << "                      quit.\n"
         << "    -play<move>       Make the specified move, then continue playing.\n";
    return 3;
  }
  
  
  if (*stat_config_file != '\0') {
    cout << "Reading static eval. config file " << stat_config_file << endl;
    if (!read_stat_val(stat_config_file)) {
      cerr << prog_name << ": Unable to read static evaluation config. info from " << stat_config_file << ": " << strerror(errno) << endl;
      return 4;
    }
  }
  
  minfo.move = &the_move;
  minfo.board = &initial_board;
  minfo.jumps_mandatory = !NoMandatoryJumps;
  
  if (!Interactive || *config_filename) {
    // read the configuration file
    if (*config_filename == '\0')
      strcpy(config_filename, "checkers.cfg");
    cout << "Reading checkers config file " << config_filename << endl;
    config_in.open(config_filename, ios::in);
    if (!config_in) {
      cerr << prog_name << ": Cannot open " << config_filename
	   << ": " << strerror(errno) << endl;
      return 5;
    }
    if (!read_config(prog_name, config_in, &initial_board)) {
      cerr << prog_name << ": Cannot make sense of the config file "
	   << config_filename << ": " << strerror(errno) << endl;
      return 6;
    }
  } else {
    // playing a human, so set up board for a new game
    initial_board = InitialBoard;
  }
  


  
  if (show_moves) {
    /* all we do is show the possible moves for this board, then exit */
    Move *m;
    
    {
      MoveGenerator movegen(initial_board, Piece::Red, !NoMandatoryJumps);
      if (movegen.getNumberMoves()) {
        cout << "\nRed moves:\n";
	while ((m = movegen.getNextMove()))
          cout << *m << endl;
      } else {
        cout << "none.\n";
      }
    }
    {
      MoveGenerator movegen(initial_board, Piece::Black, !NoMandatoryJumps);
      if (movegen.getNumberMoves()) {
        cout << "\nBlack moves:\n";
	while ((m = movegen.getNextMove()))
	  cout << *m << endl;
      } else {
        cout << "none.\n";
      }
    }
    return 0;
  }
  
  
  if (!LimitSearchLevel) {
    MaxLevel = 1;
    minfo.max_sec = atoi(argv[opt]);
  } else {
    MaxLevel = atoi(argv[opt]);
  }
  
  
  if (!Interactive) {
    // check to see if the board config we just read in represents
    // a situation where someone has already won or lost.  If it is,
    // then delete the config file.
    who_lost = loser(initial_board);
    if (who_lost == -1) {
      cout << "\nOuch!  I lost!\n";
      config_in.close();
      remove(config_filename);
      return 1;
    } else if (who_lost == 1) {
      cout << "Hey!  I won!\n";
      config_in.close();
      remove(config_filename);
      return 2;
    }
    
    if (*opponents_play != '\0') {
      Move *opponents_move;
      opponents_move = parse_user_move(*(minfo.board), opponents_play);
      if (!opponents_move) {
	cerr << "The move \"" << opponents_play << "\" is not valid.\n";
	return 8;
      } else {
	minfo.board = &(minfo.board->makeMove(*opponents_move));
      }
    }

    
    if (!LimitSearchLevel)
      initial_board = make_move_timed(&minfo);
    else
      initial_board = make_move(&minfo);
    
    // print out some statistics
    cout << "Board's static value: "
         << static_eval(initial_board) << endl;
    cout << "Lookahead: "
         << (MaxLevel - 1) << endl;
    cout << "Max. lookahead: "
         << MaxLevelSearched << endl;
    cout << "Number of static evaluations: "
         << NumStaticEvals << endl;
    cout << "Effective branching factor: "
         << pow(NumStaticEvals, 1.0 / (MaxLevel)) << endl;
    cout << "Number of alpha-beta cutoffs: "
         << NumABCutoffs << endl;
    cout << "Value of top-level node: ";
    if (minfo.val == FORCED_MOVE)
      cout << "(unknown)\n";
    else
      cout << minfo.val << endl;
    
    // are we predicting a win or a loss?
    if (minfo.val >= MY_WIN)
      cout << "Aha!  Victory is certain!\n";
    else if (minfo.val <= MY_LOSS)
      cout << "I fear my defeat is at hand...\n";
    
    cout << "move chosen: " << the_move << endl;
    
    config_in.close();
    if (remove(config_filename)) {
      cerr << prog_name << ": Can't delete old config file " << config_filename
	<< ": " << strerror(errno) << endl;
      return 7;
    }

    // did we win (well, check to see if *anyone* won, but we should be
    // the only ones who *can*)?
    if ((who_lost = loser(initial_board)) != 0) {
      // yes, somebody won
      if (who_lost == 1)
	cout << "I won!\n";
      else
        cout << "I lost!\n";
    } else {
      // nobody won, so write out the new config file
      config_out.open(config_filename, ios::out);
      if (!config_out) {
        cerr << "unable to open config file " << config_filename
	     << ": " << strerror(errno) << endl;
        return 7;
      }
      config_out << first_config_line << endl;
      initial_board.setCompatibleOutput();
      config_out << initial_board;
      config_out.close();
    }
  } else {
    // playing human, so need to get some info
    cout << copyright;

    char line[80];
    Color = Piece::Out_Of_Bounds;
    while (Color == Piece::Out_Of_Bounds) {
      cout << "\n What color are you?  ";
            fgets(line, 80, stdin);
	    /*            cin.getline(line, 80, '\n'); */
      if (strstr(line, "r"))
        Color = Piece::Black;
      else if (strstr(line, "b"))
        Color = Piece::Red;
    }
    
#ifndef TTY
      dp_init();
    display_init();
#endif
    
    //this is the main interactive loop
      display_board(initial_board, REFRESH);
    mi.mi = &minfo;
    mi.tot_clocks = 0;
    
    while (!time_to_quit) {
      // check to see if anyone won
      who_lost = loser(initial_board);
      if (who_lost == -1) {
        status_line("Hey, I lost.");
        time_to_quit = 1;
	break;
      } else if (who_lost == 1) {
        status_line("Hey, I won.");
        time_to_quit = 1;
        break;
      }
      A_get_user_command(command);
      switch (parse_command(command, arg)) {
      case HELP:
	clear_status_line();
        show_help();
        break;
      case QUIT:
        time_to_quit = 1;
        break;
      case REDRAW:
        display_init(DONT_REFRESH);
	
	display_board(initial_board, DONT_REFRESH, DONT_CACHE);
	display_stats(&mi, DONT_REFRESH);
	display_user_move(&user_move, DONT_REFRESH);
	display_comp_move(&the_move);
	clear_status_line();
        break;
      case LEVEL:
        LimitSearchLevel = 1;
        MaxLevel = atoi(arg);
	clear_status_line();
        break;
      case TIME:
        LimitSearchLevel = 0;
        minfo.max_sec = atoi(arg);
	clear_status_line();
        break;
      case MOVE:
        user_move_p = parse_user_move(initial_board, arg);
        if (!user_move_p) {
          status_line("Illegal move.");
          break;
        }
      do_move:
	clear_status_line();
        user_move = *user_move_p;
	delete user_move_p;
        initial_board = initial_board.makeMove(user_move);
        display_board(initial_board, DONT_REFRESH);
	display_user_move(&user_move);
	if (!loser(initial_board)) {
	  if (LimitSearchLevel) {
	    initial_board = make_move(&minfo);
	  } else {
	    initial_board = make_move_timed(&minfo);
	  }
	  mi.tot_clocks += minfo.elapsed_cpu_time;
	  mi.static_val = static_eval(initial_board);
	  display_comp_move(&the_move, DONT_REFRESH);
	  display_board(initial_board, DONT_REFRESH);
          display_stats(&mi);
          if (minfo.val <= MY_LOSS)
            status_line("I think I'm gonna lose...");
          else if (minfo.val >= MY_WIN )
            status_line("I think I'm gonna win...");
	}
	break;
      default:
        user_move_p = parse_user_move(initial_board, arg);
        if (user_move_p)
	  goto do_move;
        status_line("What are you trying to tell me?");
        show_help();
        break;
      }
    }
    dp_end();
    cout << "Game over... bye.\n";
  }
  return 0;
}


// This function takes argv[0] and creates a new string containing a best guess
// at the "name" of the program (no preceding path, no .exe extension under DOS).
char *get_prog_name(const char *s)
{
  const char *p;
  char *ns, *t;
  int len;

  len = strlen(s);
  for (p = s + len; ((--p) >= s) && (*p != PATH_SEPARATOR););
  p++;
  ns = new char[strlen(p)+1];
  strcpy(ns, p);
  t = ns;
#ifdef REMOVE_EXECUTABLE_EXTENSION
  while (*ns && (*ns != '.')) ns++;
  if (*ns == '.')
	*ns = '\0';
#endif
  return t;
}


// This function reads the configuration file for use in playing against other
// programs.  Mostly it figures out what color we are.
int read_config(const char *name, istream& in, BoardConfig *board)
{
  const char *n;
  char temp_name[80];
  char temp_line[80];

  in.getline(first_config_line, 200, '\n');
  if (!in) {
    cerr << "cannot read " << config_filename << ": "
         << strerror(errno) << endl;
    return 0;
  }

  strcpy(temp_name, name);
  strlower(temp_name);
  strcpy(temp_line, first_config_line);
  strlower(temp_line);
  n = strstr(temp_line, temp_name);
  if (!n) {
    cerr << "I can't tell who I am!\n";
    return 0;
  }

  n = strchr(n, '=');
  if (!n)
    return 0;

  while (isspace(*++n));
  if (*n == 'R' || *n == 'r')
    Color = Piece::Red;
  else if (*n == 'B' || *n == 'b')
    Color = Piece::Black;
  else {
    cerr << "What color am I? Huh?" << n << "?" << endl;
    return 0;
  }

  in >> *board;

  return 1;
}


// This function figures out who, if anyone, has lost given the current board configuration.
int loser(const BoardConfig& b)
{
  MoveGenerator m;

  m.prime(b, Color);
  if (m.getNumberMoves() == 0)
    // we lost
    return -1;
  m.prime(b, (Color & Piece::Red) ? Piece::Black : Piece::Red);
  if (m.getNumberMoves() == 0)
    // opponent lost
    return 1;
  return 0;
}


// This function figures out a move for the computer while enforcing a time limit.
BoardConfig make_move_timed(move_info *mi)
{
  clock_t start_clk, elapsed_clk, next_clk, total_clk, max_clk;
  float eff_branch_factor;
  Move tmove;
  int val;

  MaxLevel = 1;
  max_clk = mi->max_sec * clocks_per_sec;
  start_clk = elapsed_clk = total_clk = 0;
  next_clk = 0;
  val = 0;

  // this is the main non-interactive loop
  while (total_clk + next_clk <= max_clk && val != FORCED_MOVE) {
    start_clk = get_cpu_time();
    NumStaticEvals = NumABCutoffs = MaxLevelSearched = 0;

    val = eval_board(*(mi->board), 1, SHRT_MIN, SHRT_MAX, &tmove, mi->jumps_mandatory);

    elapsed_clk = get_cpu_time() - start_clk;
    total_clk += elapsed_clk;
    eff_branch_factor = pow(NumStaticEvals, 1.0 / MaxLevel);
    next_clk = (int) (eff_branch_factor * elapsed_clk);
    MaxLevel++;
  }
  *(mi->move) = tmove;
  mi->val = val;
  mi->elapsed_cpu_time = total_clk;

  return mi->board->makeMove(*(mi->move));
}


// This function figures out a move for the computer
BoardConfig
make_move (move_info *mi)
{
  mi->val = eval_board(*(mi->board), 1, SHRT_MIN, SHRT_MAX, mi->move, mi->jumps_mandatory);
  return mi->board->makeMove(*(mi->move));
}


clock_t
get_cpu_time (void)
{
#ifdef unix
  struct tms buffer;
  
  times(&buffer);
  return buffer.tms_utime;
  
#else
  return clock();
  
#endif
}
