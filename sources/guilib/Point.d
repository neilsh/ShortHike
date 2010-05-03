//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


class Point
{
  this() {setLocation(0, 0);}
  this(int xpos, int ypos) {setLocation(xpos, ypos);}
  
  int getX() {return x;}
  int getY() {return y;}
  void setLocation(int xpos, int ypos) {x = xpos; y = ypos;}
  void translate(int dx, int dy) {x += dx; y +=dy;}
  
  int x;
  int y;
}

