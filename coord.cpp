/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#include "coord.h"


ostream& operator<<(ostream& os, const Coord& c)
{
  os << "(" << (int) c.x << ", " << (int) c.y << ")";
  return os;
}







