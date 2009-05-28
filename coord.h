/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

#ifndef COORD_H
#define COORD_H

#include <iostream.h>

/**************************************
  Class: Coord

    This class is a pretty concrete abstract data type representing
  small integer 2d coordinates.  All sorts of basic arithmetic operators
  are defined.  They can be sent to ostreams.
**************************************/

class Coord
{
public:
  // constructor
  Coord(signed char a = 0, signed char b = 0);
  // copy constructor
  Coord(const Coord& c);

  // assignment
  Coord& operator=(const Coord& c);		

  // unary operators
  Coord& operator+=(const Coord&);
  Coord& operator-=(const Coord&);
  Coord operator+(void) const;			
  Coord operator-(void) const;		       
  Coord& operator*=(int);
  Coord& operator/=(int);

  // binary operators
  Coord operator+(const Coord&) const;
  Coord operator-(const Coord&) const;
  // equality/inequality
  int operator==(const Coord&) const;
  int operator!=(const Coord&) const;

friend ostream& operator<<(ostream& output, const Coord& c);

  signed char x, y;
};

inline Coord::Coord(signed char a, signed char b)
: x(a), y(b)
{
}

inline Coord::Coord(const Coord& c)
: x(c.x), y(c.y)
{
}

inline Coord& Coord::operator=(const Coord& c)
{
  x = c.x;
  y = c.y;
  return *this;
}

inline Coord& Coord::operator+=(const Coord& c)
{
  x += c.x;
  y += c.y;
  return *this;
}

inline Coord& Coord::operator-=(const Coord& c)
{
  x -= c.y;
  y -= c.y;
  return *this;
}

inline Coord& Coord::operator*=(int n)
{
  x *= n;
  y *= n;
  return *this;
}

inline Coord& Coord::operator/=(int n)
{
  x /= n;
  y /= n;
  return *this;
}

inline Coord Coord::operator+(void) const
{
  signed char px, py;

  px = (x >= 0) ? x : -x;
  py = (y >= 0) ? y : -y;
  return Coord(px, py);
}

inline Coord Coord::operator-(void) const
{
  return Coord(-x, -y);
}

inline Coord Coord::operator+(const Coord& c) const
{
  return Coord(x + c.x, y + c.y);
}

inline Coord Coord::operator-(const Coord& c) const
{
  return Coord(x - c.x, y - c.y);
}

inline int Coord::operator==(const Coord& c) const
{
  return (x == c.x && y == c.y);
}

inline int Coord::operator!=(const Coord& c) const
{
  return !(x == c.x && y == c.y);
}


#endif
