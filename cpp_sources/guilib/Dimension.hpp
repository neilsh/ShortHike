//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_DIMENSION_HPP
#define GUILIB_DIMENSION_HPP

class Dimension
{
public:
  Dimension() {setSize(0, 0);}
  Dimension(int width, int height) {setSize(width, height);}
  
  int getWidth() const {return w;}
  int getHeight() const {return h;}
  void setSize(int width, int height) {w = width; h = height;}
  
  int w, h;

  friend std::ostream& operator<<(std::ostream&, const Dimension&);
  friend bool operator==(Dimension&, Dimension&);
};

inline std::ostream&
operator<<(std::ostream& os, const Dimension& d)
{
  os << "Dimension [" << d.getWidth() << ", " << d.getHeight() <<  "]";
  return os;
}

inline bool
operator==(Dimension& first, Dimension& second)
{
  return first.w == second.w && first.h == second.h;
}



#endif
