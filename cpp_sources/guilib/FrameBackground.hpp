//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef USER_INTERFACE_FRAME_BACKGROUND
#define USER_INTERFACE_FRAME_BACKGROUND

#include "guilib/Drawable.hpp"

class FrameBackground : public Drawable
{
public:
  FrameBackground(bool dense = false)
    : mDense(dense)
  {}
  
  virtual void paint(Graphics* gfx);
private:
  bool mDense;
};


#endif
