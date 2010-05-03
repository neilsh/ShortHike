//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#pragma once
#ifndef GUILIB_RASTER_IMAGE_BUTTON_HPP
#define GUILIB_RASTER_IMAGE_BUTTON_HPP

class Surface;

#include "Button.hpp"

class RasterImageButton : public Button
{
public:
  RasterImageButton(Surface* surface = NULL);
  
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
