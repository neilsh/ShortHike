//--------------------------------------------------------------------------------
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "RasterImageButton.hpp"  
#include "Graphics.hpp"

#include "rendering/RenderSystem.hpp"
#include "rendering/Surface.hpp"

//--------------------------------------------------------------------------------

RasterImageButton::RasterImageButton(Surface* surface)
  : mPadding(4), mSurface(surface)
{
  if(NULL == mSurface) {
    mSurface = rRenderSystem()->createSurface();
    mSurface->loadCheckerBoard();
  }  
}


//--------------------------------------------------------------------------------


Dimension
RasterImageButton::getPreferredSize()
{
  if(NULL == mSurface)
    return Widget::getPreferredSize();
  else
    return Dimension(mSurface->getWidth() + mPadding, mSurface->getHeight() + mPadding);
}


//--------------------------------------------------------------------------------

void
RasterImageButton::paint(Graphics* gfxContext)
{
  Button::paint(gfxContext);
  gfxContext->drawSurface(mSurface, mPadding/2, mPadding/2);
}


