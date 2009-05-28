/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string.h>
#include <time.h>
#include "piece.h"


// This is gross.
#ifdef sun
#define cerr cout
#endif




class BoardConfig;
class Move;

struct move_info {
  BoardConfig	*board;
  union {
    unsigned max_sec;
    unsigned max_lvl;
  };
  char          jumps_mandatory;
  Move 		*move;
  int 		val;
  clock_t	elapsed_cpu_time;
  short		max_level_fully_searched;
  short		max_level_searched;
};


//--------------------------------------------------------------
// Global variables
//--------------------------------------------------------------

extern Piece::Type	Color;
extern int      	MaxLevel;	// Max depth to search to
extern int              MaxLevelSearched; // Max depth actually searched to
extern int      	NumStaticEvals;	// Number of static evaluations preformed
extern int      	NumABCutoffs;	// Number of alpha-beta cutoffs that
                                	// occurred
extern int		NoHeuristicCont;
extern int              NoMandatoryJumps;
extern int		clocks_per_sec;
#endif
