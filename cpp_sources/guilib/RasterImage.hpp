//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#pragma once
#ifndef GUILIB_RASTER_IMAGE_HPP
#define GUILIB_RASTER_IMAGE_HPP

class Surface;

#include "Widget.hpp"

class RasterImage : public Widget
{
public:
  RasterImage(Surface* surface = NULL);
  
  virtual void paint(Graphics* graphicsContext);
  virtual Dimension getPreferredSize();

  Surface* getSurface() {return mSurface;}

  void setPadding(int padding) {mPadding = padding;}
  int getPadding() {return mPadding;}
private:
  int mPadding;
  Surface* mSurface;
};

#endif
