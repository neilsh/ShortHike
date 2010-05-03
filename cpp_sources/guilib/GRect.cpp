// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com

#include "Common.hpp"

#include "GRect.hpp"

  
GRect
intersection(GRect &r1, GRect &r2)
{
  GRect res;

  int left = (r1.x > r2.x) ? r1.x : r2.x;
  int right = ((r1.x + r1.w) < (r2.x + r2.w)) ? (r1.x + r1.w) : (r2.x + r2.w);
  if (left > right) return res;

  int top = (r1.y > r2.y) ? r1.y : r2.y;
  int bottom = ((r1.y + r1.h) < (r2.y + r2.h)) ? (r1.y + r1.h) : (r2.y + r2.h);
  if (top > bottom) return res;

  res.x = left;
  res.w = right - left;
  res.y = top;
  res.h = bottom - top;
  return res;
}

void
GRect::addPoint(int pointX, int pointY)
{
  if(pointX < x) {
    w += (x - pointX);
    x = pointX;
  }
  if((x + w) < pointX) {
    w = (pointX - x);
  }
  if(pointY < y) {
    h += (y - pointY);
    y = pointY;
  }
  if((y + h) < pointY) {
    h = (pointY - y);
  }  
}



std::ostream&
operator<<(std::ostream& os, const GRect& r)
{
  os << "GRect [" << r.getX() << ", " << r.getY() << ", " << r.getWidth() << ", " << r.getHeight() << "]";
  return os;
}

bool
operator==(const GRect& first, const GRect& second)
{
  return first.x == second.x && first.y == second.y && first.w == second.w && first.h == second.h;
}

