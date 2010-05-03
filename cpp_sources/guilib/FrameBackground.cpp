//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "FrameBackground.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/GRect.hpp"
#include "guilib/Graphics.hpp"

//--------------------------------------------------------------------------------

void
FrameBackground::paint(Graphics* gfxContext)
{
  GRect bounds;
  bounds.setSize(gfxContext->getClipRectangle().getSize());
  if(mDense)
    gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_FRAME_COLOR_DENSE));
  else
    gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_FRAME_COLOR));

  gfxContext->fillRoundedRect(bounds.x, bounds.y, bounds.w, bounds.h);
  gfxContext->drawBevelRect(bounds.x, bounds.y, bounds.w, bounds.h);
}
