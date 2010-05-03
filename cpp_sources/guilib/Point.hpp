//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_POINT_HPP
#define GUILIB_POINT_HPP


class Point
{
public:
  Point() {setLocation(0, 0);}
  Point(int xpos, int ypos) {setLocation(xpos, ypos);}
  
  int getX() const {return x;}
  int getY() const {return y;}
  void setLocation(int xpos, int ypos) {x = xpos; y = ypos;}
  void translate(int dx, int dy) {x += dx; y +=dy;}
  
  int x;
  int y;
};


#endif
