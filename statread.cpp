/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <fstream.h>
#include <string.h>
#include "static.h"




struct parm
{
  char *name;
  int *var;
};

static parm parms[] = 
{
  { "piece_val",			&piece_val },
  { "king_val",				&king_val },
  { "move",				&move_val },
  { "jumps",				&jumps },
  { "piece_row_advance",		&piece_row_advance },
  { "piece_middle_center_squares",	&piece_middle_center_squares },
  { "piece_middle_side_squares",	&piece_middle_side_squares },
  { "piece_center_goalies",		&piece_center_goalies },
  { "piece_side_goalies",		&piece_side_goalies },
  { "piece_double_corner",		&piece_double_corner },
  { "is_home_free",			&is_home_free },
  { "dist_factor",			&dist_factor },
  { NULL,				NULL },
};



int set_var(const char *name, short val)
{
  int i;

  for (i = 0; parms[i].name != NULL && strcmp(parms[i].name, name); i++ );
  if (parms[i].name == NULL)
    return -1;

  *(parms[i].var) = val;
  
  return 0;
}


/**************************************
  Function: read_stat_val

  This function reads a configuration file that sets the values
of the variables used in static evaluations (above).

**************************************/
int read_stat_val(const char *filename)
{
  ifstream cfgfile;
  char var_name[80];
  short var_value;

  cfgfile.open(filename, std::ios::in);
  if (!cfgfile)
    return 0;

  while (cfgfile) {
    cfgfile >> var_name;
    cfgfile >> var_value;
    set_var(var_name, var_value);
  }

  cfgfile.close();

  if (cfgfile.bad())
    return 0;
  else
    return 1;
}
  

