/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include <ctype.h>


char *strlower(char *s)
{
  while (*s) {
    *s = tolower(*s);
    s++;
  }
  return s;
}
