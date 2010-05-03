//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_DRAWABLE_HPP
#define GUILIB_DRAWABLE_HPP

class Graphics;

class Drawable
{
public:
  virtual ~Drawable() {}
  virtual void paint(Graphics* graphicsContext) = 0;
  virtual void update(Time) {};
};

#endif
