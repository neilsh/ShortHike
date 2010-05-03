//--------------------------------------------------------------------------------
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "RasterImage.hpp"  
#include "Graphics.hpp"

#include "rendering/RenderSystem.hpp"
#include "rendering/Surface.hpp"

// --------------------------------------------------------------------------------

RasterImage::RasterImage(Surface* surface)
  : mPadding(4), mSurface(surface)
{
  if(NULL == mSurface) {
    mSurface = rRenderSystem()->createSurface();
    mSurface->loadCheckerBoard();
  }  
}


//--------------------------------------------------------------------------------


Dimension
RasterImage::getPreferredSize()
{
  if(NULL == mSurface)
    return Widget::getPreferredSize();
  else
    return Dimension(mSurface->getWidth() + mPadding, mSurface->getHeight() + mPadding);
}


//--------------------------------------------------------------------------------

void
RasterImage::paint(Graphics* gfxContext)
{
  gfxContext->drawSurface(mSurface, mPadding/2, mPadding/2);
}




