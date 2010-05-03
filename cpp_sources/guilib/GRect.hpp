//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_RECT_HPP
#define GUILIB_RECT_HPP

#include "dimension.hpp"
#include "point.hpp"

class GRect
{
public:
  GRect() {setBounds(0, 0, 0, 0);}
  GRect(int x, int y, int w, int h) {setBounds(x, y, w, h);}
  
  int getX() const {return x;}
  int getY() const {return y;}
  int getWidth() const {return w;}
  int getHeight() const {return h;}
  
  Dimension getSize() const {return Dimension(w, h);}
  void setSize(int w, int h) {this->w = w; this->h = h;}
  void setSize(Dimension d) {setSize(d.w, d.h);}
  void setSize(Dimension *d) {setSize(d->w, d->h);}

  Point getLocation() const {return Point(x, y);}
  void setLocation(int x, int y) {this->x = x; this->y = y;}
  void setLocation(Point p) {setLocation(p.x, p.y);}
  void setLocation(Point *p) {setLocation(p->x, p->y);}
  void translate(int dx, int dy) {setLocation(x + dx, y + dy);}
    
  void setBounds(int x, int y, int w, int h) {this->x = x; this->y = y; this->h = h; this->w = w;}
  void setBounds(Point p, Dimension &d) {setBounds(p.x, p.y, d.w, d.h);}
  void setBounds(Point *p, Dimension *d) {setBounds(p->x, p->y, d->w, d->h);}

  void addPoint(int pointX, int pointY);

  bool contains(Point p) {return (p.x >= x && p.x < (x + w) && p.y >= y && p.y < (y + h));}

  friend GRect intersection(GRect &r1, GRect &r2);
  friend std::ostream& operator<<(std::ostream&, const GRect&);
  friend bool operator==(const GRect&, const GRect&);
  
  int x;
  int y;
  int w;
  int h;
};

extern bool operator==(const GRect&, const GRect&);

#endif
